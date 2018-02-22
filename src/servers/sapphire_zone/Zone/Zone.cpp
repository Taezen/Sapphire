#include <stdio.h>
#include <vector>

#include <common/Logging/Logger.h>
#include <common/Util/Util.h>
#include <common/Util/UtilMath.h>
#include <common/Network/GamePacket.h>
#include <common/Network/GamePacketNew.h>
#include <common/Exd/ExdDataGenerated.h>
#include <common/Network/CommonNetwork.h>
#include <common/Network/PacketDef/Zone/ServerZoneDef.h>
#include <common/Network/PacketContainer.h>
#include <common/Database/DatabaseDef.h>

#include "Zone.h"
#include "TerritoryMgr.h"

#include "Session.h"
#include "Actor/Chara.h"
#include "Actor/Player.h"

#include "Forwards.h"

#include "Network/GameConnection.h"
#include "ServerZone.h"
#include "Script/ScriptManager.h"

#include "CellHandler.h"

#include <time.h>

extern Core::Logger g_log;
extern Core::ServerZone g_serverZone;
extern Core::Data::ExdDataGenerated g_exdDataGen;
extern Core::Scripting::ScriptManager g_scriptMgr;
extern Core::TerritoryMgr g_territoryMgr;

using namespace Core::Common;
using namespace Core::Network::Packets;
using namespace Core::Network::Packets::Server;

/**
* \brief
*/
Core::Zone::Zone() :
   m_territoryId( 0 ),
   m_guId( 0 ),
   m_currentWeather( Weather::FairSkies ),
   m_weatherOverride( Weather::None ),
   m_lastMobUpdate( 0 ),
   m_currentFestivalId( 0 )
{
}

Core::Zone::Zone( uint16_t territoryId, uint32_t guId, const std::string& internalName, const std::string& placeName ) :
   m_currentWeather( Weather::FairSkies )
{
   m_guId = guId;

   m_territoryId = territoryId;
   m_internalName = internalName;
   m_placeName = placeName;
   m_lastMobUpdate = 0;

   m_weatherOverride = Weather::None;
   m_territoryTypeInfo = g_exdDataGen.get< Core::Data::TerritoryType >( territoryId );

   loadWeatherRates();

   m_currentWeather = getNextWeather();
}

void Core::Zone::loadWeatherRates()
{
   if( !m_territoryTypeInfo )
   {
      g_log.error( std::string( __FUNCTION__ ) + " TerritoryTypeInfo not loaded!" );
      return;
   }

   uint8_t weatherRateId = m_territoryTypeInfo->weatherRate > g_exdDataGen.getWeatherRateIdList().size() ?
                           uint8_t{ 0 } : m_territoryTypeInfo->weatherRate;

   uint8_t sumPc = 0;
   auto weatherRateFields = g_exdDataGen.m_WeatherRateDat.get_row( weatherRateId );
   for( size_t i = 0; i < 16; )
   {
      int32_t weatherId = boost::get< int32_t >( weatherRateFields[i] );

      if( weatherId == 0 )
         break;

      sumPc += boost::get< uint8_t >( weatherRateFields[i + 1] );
      m_weatherRateMap[sumPc] = weatherId;
      i += 2;
   }
}

Core::Zone::~Zone()
{
}

bool Core::Zone::init()
{

   if( g_scriptMgr.onZoneInit( shared_from_this() ) )
   {
      // all good
   }


   return true;
}

void Core::Zone::setWeatherOverride( Weather weather )
{
   m_weatherOverride = weather;
}

Weather Core::Zone::getCurrentWeather() const
{
   return m_currentWeather;
}

uint16_t Core::Zone::getCurrentFestival() const
{
   return m_currentFestivalId;
}

void Core::Zone::setCurrentFestival( uint16_t festivalId )
{
   m_currentFestivalId = festivalId;
}


void Core::Zone::loadCellCache()
{

}

Weather Core::Zone::getNextWeather()
{
   uint32_t unixTime = static_cast< uint32_t >( Util::getTimeSeconds() );
   // Get Eorzea hour for weather start
   uint32_t bell = unixTime / 175;
   // Do the magic 'cause for calculations 16:00 is 0, 00:00 is 8 and 08:00 is 16
   int32_t increment = ( ( bell + 8 - ( bell % 8 ) ) ) % 24;

   // Take Eorzea days since unix epoch
   uint32_t totalDays = ( unixTime / 4200 );

   uint32_t calcBase = ( totalDays * 0x64 ) + increment;

   uint32_t step1 = ( calcBase << 0xB ) ^ calcBase;
   uint32_t step2 = ( step1 >> 8 ) ^ step1;

   auto rate = static_cast< uint8_t >( step2 % 0x64 );

   for( auto entry : m_weatherRateMap )
   {
      uint8_t sRate = entry.first;
      auto weatherId = static_cast< Weather >( entry.second );

      if( rate <= sRate )
         return weatherId;
   }

   return Weather::FairSkies;
}

void Core::Zone::pushActor( Entity::CharaPtr pChara )
{
   float mx = pChara->getPos().x;
   float my = pChara->getPos().z;
   uint32_t cx = getPosX( mx );
   uint32_t cy = getPosY( my );

   Cell* pCell = getCell( cx, cy );
   if( !pCell )
   {
      pCell = create( cx, cy );
      pCell->init( cx, cy, shared_from_this() );
   }

   pCell->addChara(pChara);

   pChara->setCell( pCell );

   uint32_t cellX = getPosX( pChara->getPos().x );
   uint32_t cellY = getPosY( pChara->getPos().z );

   uint32_t endX = cellX <= _sizeX ? cellX + 1 : ( _sizeX - 1 );
   uint32_t endY = cellY <= _sizeY ? cellY + 1 : ( _sizeY - 1 );
   uint32_t startX = cellX > 0 ? cellX - 1 : 0;
   uint32_t startY = cellY > 0 ? cellY - 1 : 0;
   uint32_t posX, posY;

   for( posX = startX; posX <= endX; ++posX )
   {
      for( posY = startY; posY <= endY; ++posY )
      {
         pCell = getCell( posX, posY );
         if( pCell )
            updateInRangeSet( pChara, pCell );
      }
   }

   if( pChara->isPlayer() )
   {
      auto pPlayer = pChara->getAsPlayer();

      auto pSession = g_serverZone.getSession( pPlayer->getId() );
      if( pSession )
         m_sessionSet.insert( pSession );
      m_playerMap[pPlayer->getId()] = pPlayer;
      updateCellActivity( cx, cy, 2 );
   }
 }

void Core::Zone::removeActor( Entity::CharaPtr pChara )
{

   if( pChara->m_pCell )
   {
      pChara->m_pCell->removeChara(pChara);
      pChara->m_pCell = nullptr;
   }

   if( pChara->isPlayer() )
   {

      // If it's a player and he's inside boundaries - update his nearby cells
      if( pChara->getPos().x <= _maxX && pChara->getPos().x >= _minX &&
          pChara->getPos().z <= _maxY && pChara->getPos().z >= _minY )
      {
         uint32_t x = getPosX( pChara->getPos().x );
         uint32_t y = getPosY( pChara->getPos().z );
         updateCellActivity( x, y, 3 );
      }
      m_playerMap.erase( pChara->getId() );

      onLeaveTerritory( *pChara->getAsPlayer() );

   }

   // remove from lists of other actors
   pChara->removeFromInRange();
   pChara->clearInRangeSet();

}

void Core::Zone::queueOutPacketForRange( Entity::Player& sourcePlayer, uint32_t range, GamePacketPtr pPacketEntry )
{
   if( g_territoryMgr.isPrivateTerritory( getTerritoryId() ) )
      return;

   for( auto entry : m_playerMap )
   {
      auto player = entry.second;
      float distance = Math::Util::distance( sourcePlayer.getPos().x,
                                             sourcePlayer.getPos().y,
                                             sourcePlayer.getPos().z,
                                             player->getPos().x,
                                             player->getPos().y,
                                             player->getPos().z );

      if( ( distance < range ) && sourcePlayer.getId() != player->getId() )
      {
         auto pSession = g_serverZone.getSession( player->getId() );
         pPacketEntry->setValAt< uint32_t >( 0x08, player->getId() );
         if( pSession )
            pSession->getZoneConnection()->queueOutPacket( pPacketEntry );
      }
   }
}

uint32_t Core::Zone::getTerritoryId() const
{
   return m_territoryId;
}

uint32_t Core::Zone::getGuId() const
{
   return m_guId;
}

const std::string& Core::Zone::getName() const
{
   return m_placeName;
}

const std::string& Core::Zone::getInternalName() const
{
   return m_internalName;
}

std::size_t Core::Zone::getPopCount() const
{
   return m_playerMap.size();
}

bool Core::Zone::checkWeather()
{
   if( m_weatherOverride != Weather::None )
   {
      if( m_weatherOverride != m_currentWeather )
      {
         m_currentWeather = m_weatherOverride;
         g_log.debug( "[Zone:" + m_internalName + "] overriding weather to : " +
                      std::to_string( static_cast< uint8_t >( m_weatherOverride ) ) );
         return true;
      }
   }
   else
   {
      auto nextWeather = getNextWeather();
      if( nextWeather != m_currentWeather )
      {
         m_currentWeather = nextWeather;
         g_log.debug( "[Zone:" + m_internalName + "] changing weather to : " +
                      std::to_string( static_cast< uint8_t >( nextWeather ) ) );
         return true;
      }
   }
   return false;
}

/*
void Core::Zone::updateBnpcs( int64_t tickCount )
{
   if( ( tickCount - m_lastMobUpdate ) > 250 )
   {

      m_lastMobUpdate = tickCount;
      uint32_t currTime = static_cast< uint32_t >( time( nullptr ) );

      for( auto it3 = m_BattleNpcDeadMap.begin(); it3 != m_BattleNpcDeadMap.end(); ++it3 )
      {

         Entity::BattleNpcPtr pBNpc = *it3;

         if( ( currTime - pBNpc->getTimeOfDeath() ) > 60 )
         {

            pBNpc->resetHp();
            pBNpc->resetMp();
            pBNpc->resetPos();
            pushActor( pBNpc );

            m_BattleNpcDeadMap.erase( it3 );

            break;
         }
      }


      for( auto entry : m_BattleNpcMap )
      {
         Entity::BattleNpcPtr pBNpc = entry.second;

         if( !pBNpc )
            continue;

         if( !pBNpc->isAlive() && currTime - pBNpc->getTimeOfDeath() > ( 10 ) )
         {
            removeActor( pBNpc );
            m_BattleNpcDeadMap.insert( pBNpc );
            break;
         }

         pBNpc->update( tickCount );

      }
   }
}
*/

bool Core::Zone::update( uint32_t currTime )
{
   int64_t tickCount = Util::getTimeMs();

   //TODO: this should be moved to a updateWeather call and pulled out of updateSessions
   bool changedWeather = checkWeather();

   updateSessions( changedWeather );
   //updateBnpcs( tickCount );
   onUpdate( currTime );

   return true;
}

void Core::Zone::updateSessions( bool changedWeather )
{
   auto it = m_sessionSet.begin();

   // update sessions in this zone
   for( ; it != m_sessionSet.end(); )
   {

      auto pSession = ( *it );

      if( !pSession )
      {
         it = m_sessionSet.erase(it );
         continue;
      }

      auto pPlayer = pSession->getPlayer();

      // this session is not linked to this area anymore, remove it from zone session list
      if( ( !pPlayer->getCurrentZone() ) || ( pPlayer->getCurrentZone() != shared_from_this() ) )
      {
         if( pPlayer->getCell() )
            removeActor( pSession->getPlayer() );

         it = m_sessionSet.erase(it );
         continue;
      }

      if( changedWeather )
      {
         ZoneChannelPacket< FFXIVIpcWeatherChange  > weatherChangePacket( pPlayer->getId() );
         weatherChangePacket.data().weatherId = static_cast< uint8_t >( m_currentWeather );
         weatherChangePacket.data().delay = 5.0f;
         pSession->getPlayer()->queuePacket( weatherChangePacket );
      }

      // perform session duties
      pSession->update();
      ++it;
   }
}

bool Core::Zone::isCellActive( uint32_t x, uint32_t y )
{
   uint32_t endX = ( ( x + 1 ) <= _sizeX ) ? x + 1 : ( _sizeX - 1 );
   uint32_t endY = ( ( y + 1 ) <= _sizeY ) ? y + 1 : ( _sizeY - 1 );
   uint32_t startX = x > 0 ? x - 1 : 0;
   uint32_t startY = y > 0 ? y - 1 : 0;
   uint32_t posX;
   uint32_t posY;

   Cell* pCell;

   for( posX = startX; posX <= endX; posX++ )
   {
      for( posY = startY; posY <= endY; posY++ )
      {
         pCell = getCell( posX, posY );

         if( pCell && ( pCell->hasPlayers() || pCell->isForcedActive() ) )
            return true;
      }
   }

   return false;
}

void Core::Zone::updateCellActivity( uint32_t x, uint32_t y, int32_t radius )
{

   uint32_t endX = ( x + radius ) <= _sizeX ? x + radius : ( _sizeX - 1 );
   uint32_t endY = ( y + radius ) <= _sizeY ? y + radius : ( _sizeY - 1 );
   uint32_t startX = x - radius > 0 ? x - radius : 0;
   uint32_t startY = y - radius > 0 ? y - radius : 0;
   uint32_t posX, posY;

   Cell* pCell;

   for( posX = startX; posX <= endX; posX++ )
   {
      for( posY = startY; posY <= endY; posY++ )
      {
         pCell = getCell( posX, posY );

         if( !pCell )
         {
            if( isCellActive( posX, posY ) )
            {
               pCell = create( posX, posY );
               pCell->init( posX, posY, shared_from_this() );

               pCell->setActivity( true );

               assert( !pCell->isLoaded() );

            }
         }
         else
         {
            //Cell is now active
            if( isCellActive( posX, posY ) && !pCell->isActive() )
            {
               pCell->setActivity( true );

               if( !pCell->isLoaded() )
               {

               }
            }
            else if( !isCellActive( posX, posY ) && pCell->isActive() )
               pCell->setActivity( false );
         }
      }
   }
}

void Core::Zone::updateActorPosition( Entity::Chara &actor )
{

   if( actor.getCurrentZone() != shared_from_this() )
      return;

   //actor.checkInRangeActors();

   uint32_t cellX = getPosX( actor.getPos().x );
   uint32_t cellY = getPosY( actor.getPos().z );

   if( cellX >= _sizeX || cellY >= _sizeY )
      return;

   Cell* pCell = getCell( cellX, cellY );
   Cell* pOldCell = actor.m_pCell;
   if( !pCell )
   {
      pCell = create( cellX, cellY );
      pCell->init( cellX, cellY, shared_from_this() );
   }

   // If object moved cell
   if( pCell != pOldCell )
   {

      if( pOldCell )
         pOldCell->removeChara(actor.getAsChara());

      pCell->addChara(actor.getAsChara());
      actor.m_pCell = pCell;

      // if player we need to update cell activity
      // radius = 2 is used in order to update both
      // old and new cells
      if( actor.isPlayer() )
      {
         updateCellActivity( cellX, cellY, 2 );
         if( pOldCell != nullptr )
         {
            // only do the second check if theres -/+ 2 difference
            if( abs( ( int32_t ) cellX - ( int32_t ) pOldCell->m_posX ) > 2 ||
                abs( ( int32_t ) cellY - ( int32_t ) pOldCell->m_posY ) > 2 )
               updateCellActivity( pOldCell->m_posX, pOldCell->m_posY, 2 );
         }
      }
   }

   // update in range actor set
   uint32_t endX = cellX <= _sizeX ? cellX + 1 : ( _sizeX - 1 );
   uint32_t endY = cellY <= _sizeY ? cellY + 1 : ( _sizeY - 1 );
   uint32_t startX = cellX > 0 ? cellX - 1 : 0;
   uint32_t startY = cellY > 0 ? cellY - 1 : 0;
   uint32_t posX, posY;

   for( posX = startX; posX <= endX; ++posX )
   {
      for( posY = startY; posY <= endY; ++posY )
      {
         pCell = getCell( posX, posY );
         if( pCell )
            updateInRangeSet( actor.getAsChara(), pCell );
      }
   }
}


void Core::Zone::updateInRangeSet( Entity::CharaPtr pChara, Cell* pCell )
{
   if( pCell == nullptr )
      return;

   // TODO: make sure gms can overwrite this. Potentially temporary solution
   if( g_territoryMgr.isPrivateTerritory( getTerritoryId() ) )
      return;

   auto iter = pCell->m_charas.begin();

   float fRange = 70.0f;
   int32_t count = 0;
   while( iter != pCell->m_charas.end() )
   {
      auto pCurAct = *iter;
      ++iter;

      if( !pCurAct || pCurAct == pChara )
         continue;

      float distance = Math::Util::distance( pCurAct->getPos().x, pCurAct->getPos().y, pCurAct->getPos().z,
                                             pChara->getPos().x, pChara->getPos().y, pChara->getPos().z );

      bool isInRange = ( fRange == 0.0f || distance <= fRange );
      bool isInRangeSet = pChara->isInRangeSet( pCurAct );

      // Add if range == 0 or distance is withing range.
      if( isInRange && !isInRangeSet )
      {

         if( pChara->isPlayer() && !pChara->getAsPlayer()->isLoadingComplete() )
            continue;

         if( pCurAct->isPlayer() && !pCurAct->getAsPlayer()->isLoadingComplete() )
            continue;

         pChara->addInRangeActor( pCurAct );
         pCurAct->addInRangeActor( pChara );

         // this is a hack to limit actor spawn in one packetset
         if( count++ > 12 )
            break;
      }
      else if( !isInRange && isInRangeSet )
      {
         pCurAct->removeInRangeActor( *pChara );
         pChara->removeInRangeActor( *pCurAct );
      }
   }
}

void Core::Zone::onEnterTerritory( Entity::Player& player )
{
   g_log.debug( "Zone::onEnterTerritory: Zone#" + std::to_string( getGuId() ) + "|" + std::to_string( getTerritoryId() ) +
                                                + ", Entity#" + std::to_string( player.getId() ) );
}

void Core::Zone::onLeaveTerritory( Entity::Player& player )
{
   g_log.debug( "Zone::onLeaveTerritory: Zone#" + std::to_string( getGuId() ) + "|" + std::to_string( getTerritoryId() ) +
                                                + ", Entity#" + std::to_string( player.getId() ) );
}

void Core::Zone::onUpdate( uint32_t currTime )
{

}

void Core::Zone::onFinishLoading( Entity::Player& player )
{

}

void Core::Zone::onInitDirector( Entity::Player& player )
{

}

void Core::Zone::registerEObj( Entity::EventObjectPtr object )
{
   if( !object )
      return;

   //object->setParentInstance( InstanceContentPtr( this ) );

   m_eventObjects[object->getId()] = object;

   g_log.debug( "Registered instance eobj: " + std::to_string( object->getId() ) );
}

Core::Entity::EventObjectPtr Core::Zone::getEObj( uint32_t objId )
{
   auto obj = m_eventObjects.find( objId );
   if( obj == m_eventObjects.end() )
      return nullptr;

   return obj->second;
}

// TODO: this is located wrong. state change should happen in the object and it should send to his in range set.
void Core::Zone::updateEObj( Entity::EventObjectPtr object )
{
   if( !object )
      return;

   for( const auto& playerIt : m_playerMap )
   {
      // send that packet with le data
      ZoneChannelPacket< FFXIVIpcObjectSpawn > eobjStatePacket( playerIt.second->getId() );
      eobjStatePacket.data().objKind = object->getObjKind();
      eobjStatePacket.data().state = object->getState();
      eobjStatePacket.data().objId = object->getId();
      eobjStatePacket.data().hierachyId = object->getHierachyId();
      eobjStatePacket.data().position = object->getPos();

      // ????
      //eobjStatePacket.data().levelId = 4236873;
      //eobjStatePacket.data().unknown2 = 5;
      //eobjStatePacket.data().unknown1C = 1065353216;
      //eobjStatePacket.data().unknown20 = 2147423605;
      //eobjStatePacket.data().actorId = 1074105831;
      //eobjStatePacket.data().unknown = 1;

      playerIt.second->queuePacket( eobjStatePacket );
   }
}
