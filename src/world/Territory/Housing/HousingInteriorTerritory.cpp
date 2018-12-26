#include <Common.h>
#include <Logging/Logger.h>
#include <Util/Util.h>
#include <Util/UtilMath.h>
#include <Database/DatabaseDef.h>
#include <Exd/ExdDataGenerated.h>
#include <Network/GamePacketNew.h>
#include <Network/PacketDef/Zone/ServerZoneDef.h>

#include "Actor/Player.h"
#include "Actor/Actor.h"
#include "Actor/EventObject.h"
#include "Manager/HousingMgr.h"
#include "Territory/Land.h"
#include "Territory/House.h"
#include "Inventory/ItemContainer.h"
#include "Inventory/HousingItem.h"

#include "Forwards.h"
#include "HousingInteriorTerritory.h"
#include "Framework.h"

extern Sapphire::Framework g_fw;

using namespace Sapphire::Common;
using namespace Sapphire::Network::Packets;
using namespace Sapphire::Network::Packets::Server;
using namespace Sapphire::World::Manager;

using namespace Sapphire;
using namespace Sapphire::World::Territory;

Sapphire::World::Territory::Housing::HousingInteriorTerritory::HousingInteriorTerritory( Common::LandIdent ident,
                                                                                         uint16_t territoryTypeId,
                                                                                         uint32_t guId,
                                                                                         const std::string& internalName,
                                                                                         const std::string& contentName ) :
  Zone( territoryTypeId, guId, internalName, contentName ),
  m_landIdent( ident )
{
  m_lastActivityTime = static_cast< uint32_t >( Util::getTimeSeconds() );
}

Housing::HousingInteriorTerritory::~HousingInteriorTerritory() = default;

bool Sapphire::World::Territory::Housing::HousingInteriorTerritory::init()
{
  updateYardObjects();

  return true;
}

void Sapphire::World::Territory::Housing::HousingInteriorTerritory::onPlayerZoneIn( Entity::Player& player )
{
  auto pHousingMgr = g_fw.get< HousingMgr >();
  auto pLog = g_fw.get< Logger >();
  pLog->debug(
    "HousingInteriorTerritory::onPlayerZoneIn: Zone#" + std::to_string( getGuId() ) + "|" + std::to_string( getTerritoryTypeId() ) +
    ", Entity#" + std::to_string( player.getId() ) );

  auto indoorInitPacket = makeZonePacket< Server::FFXIVIpcHousingIndoorInitialize >( player.getId() );
  indoorInitPacket->data().u1 = 0;
  indoorInitPacket->data().u2 = 0;
  indoorInitPacket->data().u3 = 0;
  indoorInitPacket->data().u4 = 0;

  auto landSetId = pHousingMgr->toLandSetId( m_landIdent.territoryTypeId, m_landIdent.wardNum );
  auto pLand = pHousingMgr->getHousingZoneByLandSetId( landSetId )->getLand( m_landIdent.landId );
  auto pHouse = pLand->getHouse();

  for( auto i = 0; i < 10; i++ )
  {
    indoorInitPacket->data().indoorItems[ i ] = pHouse->getInteriorModel(
      static_cast< Common::HousingInteriorSlot >( i ) );
  }

  player.queuePacket( indoorInitPacket );

  auto yardPacketTotal = static_cast< uint8_t >( 2 + pLand->getSize() );
  for( uint8_t yardPacketNum = 0; yardPacketNum < yardPacketTotal; yardPacketNum++ )
  {
    auto objectInitPacket = makeZonePacket< Server::FFXIVIpcHousingObjectInitialize >( player.getId() );
    memcpy( &objectInitPacket->data().landIdent, &m_landIdent, sizeof( Common::LandIdent ) );
    objectInitPacket->data().u1 = 0;
    objectInitPacket->data().u2 = 100;
    objectInitPacket->data().packetNum = yardPacketNum;
    objectInitPacket->data().packetTotal = yardPacketTotal;

    auto yardObjectSize = sizeof( Common::YardObject );
    memcpy( &objectInitPacket->data().object, m_yardObjects.data() + ( yardPacketNum * 100 ), yardObjectSize * 100 );

    player.queuePacket( objectInitPacket );
  }

}

void Sapphire::World::Territory::Housing::HousingInteriorTerritory::onUpdate( uint32_t currTime )
{
  if( m_playerMap.size() > 0 )
    m_lastActivityTime = currTime;
}

uint32_t Sapphire::World::Territory::Housing::HousingInteriorTerritory::getLastActivityTime() const
{
  return m_lastActivityTime;
}

const Common::LandIdent Sapphire::World::Territory::Housing::HousingInteriorTerritory::getLandIdent() const
{
  return m_landIdent;
}

void Sapphire::World::Territory::Housing::HousingInteriorTerritory::updateYardObjects()
{
  auto housingMgr = g_fw.get< Manager::HousingMgr >();

  auto containerIds = {
    InventoryType::HousingInteriorPlacedItems1,
    InventoryType::HousingInteriorPlacedItems2,
    InventoryType::HousingInteriorPlacedItems3,
    InventoryType::HousingInteriorPlacedItems4,
    InventoryType::HousingInteriorPlacedItems5,
    InventoryType::HousingInteriorPlacedItems6,
    InventoryType::HousingInteriorPlacedItems7,
    InventoryType::HousingInteriorPlacedItems8,
  };

  // zero out the array
  // there's some really weird behaviour where *some* values will cause the linkshell invite notification to pop up
  // for some reason
  Common::YardObject obj {};
  memset( &obj, 0x0, sizeof( Common::YardObject ) );
  m_yardObjects.fill( obj );

  auto containers = housingMgr->getEstateInventory( getLandIdent() );

  uint8_t containerIdx = 0;
  for( auto containerId : containerIds )
  {
    auto container = containers.find( containerId );
    if( container == containers.end() )
      // no more containers left
      break;

    for( const auto& item : container->second->getItemMap() )
    {
      auto housingItem = std::dynamic_pointer_cast< Inventory::HousingItem >( item.second );
      assert( housingItem );

      auto offset = item.first + ( containerIdx * 50 );

      auto obj = housingMgr->getYardObjectForItem( housingItem );

      m_yardObjects[ offset ] = obj;
    }

    containerIdx++;
  }
}

void Sapphire::World::Territory::Housing::HousingInteriorTerritory::spawnYardObject( uint8_t containerIdx,
                                                                                     uint16_t slot,
                                                                                     Inventory::HousingItemPtr item )
{
  auto housingMgr = g_fw.get< Manager::HousingMgr >();

  auto offset = ( containerIdx * 50 ) + slot;
  auto obj = housingMgr->getYardObjectForItem( item );

  m_yardObjects[ offset ] = obj;

//  for( const auto& player : m_playerMap )
//  {
//    auto packet = makeZonePacket< Server::FFXIVIpcYardObjectSpawn >( player.second->getId() );
//
//    packet->data().landSetId = 0;
//    packet->data().objectArray = static_cast< uint8_t >( offset );
//    packet->data().object = obj;
//
//    player.second->queuePacket( packet );
//  }
}