#include <Common.h>
#include <Logging/Logger.h>
#include <Network/CommonActorControl.h>
#include <algorithm>

#include "Territory/Territory.h"

#include "Network/PacketWrappers/ActorControlPacket.h"
#include "Network/PacketWrappers/ActorControlSelfPacket.h"
#include "Network/PacketWrappers/UpdateInventorySlotPacket.h"

#include "Inventory/CurrencyCrystal.h"
#include "Inventory/CurrencyCrystalContainer.h"


#include "Player.h"

#include <Network/PacketDef/Zone/ServerZoneDef.h>

#include <Exd/ExdDataGenerated.h>
#include <Logging/Logger.h>
#include <Database/DatabaseDef.h>

#include "Actor/Player.h"

#include "Network/PacketWrappers/ServerNoticePacket.h"
#include "Network/PacketWrappers/ActorControlSelfPacket.h"

#include "Manager/InventoryMgr.h"
#include "Manager/CurrencyCrystalMgr.h"

#include <Service.h>

using namespace Sapphire::Common;
using namespace Sapphire::Network::Packets;
using namespace Sapphire::Network::Packets::Server;
using namespace Sapphire::Network::ActorControl;


void Sapphire::Entity::Player::initCurrencyCrystal()
{
  auto setupContainer = [ this ]( InventoryType type, uint8_t maxSize, const std::string& tableName )
  { m_currencyCrystalMap[ type ] = make_CurrencyCrystalContainer( type, maxSize, tableName ); };

  // currency container
  setupContainer( InventoryType::Currency, 12, "charaitemcurrency" );

  // crystal container
  setupContainer( InventoryType::Crystal, 18, "charaitemcrystal" );

  // extra currency container
  setupContainer( InventoryType::ExtraCurrency, 29, "charaextracurrency" );

  loadCurrency();
  loadCrystal();
  loadExtraCurrency();
}

void Sapphire::Entity::Player::addCurrencyCrystal( CurrencyCrystalType type, uint32_t amount, bool sendLootMessage )
{
  CurrencyCrystalPtr currItem = nullptr;

  auto inventoryType = type < static_cast< CurrencyCrystalType >( 100 ) ?
                       ( ( type == CurrencyCrystalType::Gil || type >= CurrencyCrystalType::StormSeal ) ?
                       InventoryType::Currency :
                       InventoryType::Crystal ) :
                       InventoryType::ExtraCurrency;

  for ( auto itr : m_currencyCrystalMap[ inventoryType ]->getCurrencyCrystalMap() )
  {
    if ( itr.second->getId() == type )
    {
      currItem = itr.second;

      break;
    }
  }

  if( !currItem )
  {
    uint8_t slot;

    if ( inventoryType == InventoryType::Crystal )
    {
      if ( type > Common::CurrencyCrystalType::WaterCluster )
        return;

      slot = static_cast< uint16_t >( type ) - 2;
    }
    else if ( inventoryType == InventoryType::Currency )
    {
      for( slot = 0; slot < CurrencyCrystal::m_currencyIds.size(); slot++ )
      {
        if ( CurrencyCrystal::m_currencyIds[ slot ].first == type )
        {
          break;
        }
      }

      if ( slot == CurrencyCrystal::m_currencyIds.size() )
        return;
    }
    else
    {
      for ( slot = 0; slot < CurrencyCrystal::m_extraCurrencyIds.size(); slot++ )
      {
        if ( CurrencyCrystal::m_extraCurrencyIds[slot].first == type )
        {
          break;
        }
      }

      if ( slot == CurrencyCrystal::m_extraCurrencyIds.size() )
        return;
    }

    currItem = make_CurrencyCrystal( slot, type );

    m_currencyCrystalMap[ inventoryType ]->setCurrencyCrystal( currItem );
  }

  uint32_t currentAmount = currItem->getAmount();

  currItem->setAmount( currentAmount + amount );

  updateCurrencyCrystalDb( m_currencyCrystalMap[ inventoryType ]->getTableName(), currItem );

  if ( inventoryType != InventoryType::ExtraCurrency )
  {
    auto invUpdate = std::make_shared< UpdateInventorySlotPacket >( getId(),
                                                                    currItem->getUId(),
                                                                    inventoryType,
                                                                    *currItem );

    queuePacket(invUpdate);
  }
  else
  {
    queuePacket( makeActorControlSelf( getId(),
                                       ExtraCurrency,
                                       CurrencyCrystal::m_extraCurrencyIds[ currItem->getUId() ].second,
                                       static_cast< uint16_t >( currItem->getId() ),
                                       currItem->getMaxAmount(),
                                       getCurrencyCrystal( currItem->getId() ),
                                       currItem->getMaxAmount() >= 65000 ? 1 : 0 ) );
  }

  if( inventoryType == InventoryType::Currency )
  {
    if ( sendLootMessage )
    {
      switch( type )
      {
        case CurrencyCrystalType::Gil:
        {
          auto lootMsg = makeZonePacket< FFXIVIpcLootMessage >( getId() );
          lootMsg->data().msgType = Common::LootMessageType::GetGil;
          lootMsg->data().param1 = amount;
          queuePacket( lootMsg );
          break;
        }
      }
    }
  }
  else if ( inventoryType == InventoryType::Crystal )
  {
    if( sendLootMessage )
    {
      auto lootMsg = makeZonePacket< FFXIVIpcLootMessage >( getId() );
      lootMsg->data().msgType = Common::LootMessageType::GetItem2;
      lootMsg->data().param1 = getId();
      lootMsg->data().param2 = static_cast< uint16_t >( currItem->getId() );
      lootMsg->data().param3 = amount;
      queuePacket( lootMsg );
    }

    auto soundEffectPacket = makeZonePacket< FFXIVIpcInventoryActionAck >( getId() );
    soundEffectPacket->data().sequence = 0xFFFFFFFF;
    soundEffectPacket->data().type = 6;
    queuePacket( soundEffectPacket );
  }
}

void Sapphire::Entity::Player::removeCurrencyCrystal( Common::CurrencyCrystalType type, uint32_t amount )
{
  CurrencyCrystalPtr currItem = nullptr;

  auto inventoryType = type < static_cast< CurrencyCrystalType >( 100 ) ?
                       ( ( type == CurrencyCrystalType::Gil || type >= CurrencyCrystalType::StormSeal ) ?
                       InventoryType::Currency :
                       InventoryType::Crystal ) :
                       InventoryType::ExtraCurrency;

  for ( auto itr : m_currencyCrystalMap[ inventoryType ]->getCurrencyCrystalMap() )
  {
    if ( itr.second->getId() == type )
    {
      currItem = itr.second;

      break;
    }
  }

  if( !currItem )
    return;

  uint32_t currentAmount = currItem->getAmount();
  if( amount > currentAmount )
    currItem->setAmount( 0 );
  else
    currItem->setAmount( currentAmount - amount );

  updateCurrencyCrystalDb( m_currencyCrystalMap[ inventoryType ]->getTableName(), currItem );

  if ( inventoryType != InventoryType::ExtraCurrency )
  {
    auto invUpdate = std::make_shared< UpdateInventorySlotPacket >( getId(),
                                                                    currItem->getUId(),
                                                                    inventoryType,
                                                                    *currItem );
    queuePacket( invUpdate );
  }
  else
  {
    queuePacket( makeActorControlSelf( getId(),
                                       ExtraCurrency,
                                       CurrencyCrystal::m_extraCurrencyIds[ currItem->getUId() ].second,
                                       static_cast< uint16_t >( currItem->getId() ),
                                       currItem->getMaxAmount(),
                                       getCurrencyCrystal( currItem->getId() ),
                                       currItem->getMaxAmount() >= 65000 ? 1 : 0 ) );
  }
}

uint32_t Sapphire::Entity::Player::getCurrencyCrystal( CurrencyCrystalType type )
{
  CurrencyCrystalPtr currItem = nullptr;

  auto inventoryType = type < static_cast< CurrencyCrystalType >( 100 ) ?
                       ( ( type == CurrencyCrystalType::Gil || type >= CurrencyCrystalType::StormSeal ) ?
                       InventoryType::Currency :
                       InventoryType::Crystal ) :
                       InventoryType::ExtraCurrency;

  for ( auto itr : m_currencyCrystalMap[ inventoryType ]->getCurrencyCrystalMap() )
  {
    if ( itr.second->getId() == type )
    {
      currItem = itr.second;

      break;
    }
  }

  if( !currItem )
    return 0;

  return currItem->getAmount();

}
void Sapphire::Entity::Player::sendCurrencyCrystal()
{
  for (auto it = m_currencyCrystalMap.begin(); it != m_currencyCrystalMap.end(); ++it)
  {
    auto sequence = getNextInventorySequence();
    auto pMap = it->second->getCurrencyCrystalMap();

    for( auto itM = pMap.begin(); itM != pMap.end(); ++itM )
    {
      if( !itM->second )
        return;

      auto currencyInfoPacket = makeZonePacket< Server::FFXIVIpcCurrencyCrystalInfo >( getId() );
      currencyInfoPacket->data().containerSequence = sequence;
      currencyInfoPacket->data().catalogId = itM->second->getId();
      currencyInfoPacket->data().unknown = 1;
      currencyInfoPacket->data().quantity = itM->second->getAmount();
      currencyInfoPacket->data().containerId = it->second->getId();
      currencyInfoPacket->data().slot = itM->first;

      queuePacket( currencyInfoPacket );

      auto containerInfoPacket = makeZonePacket< Server::FFXIVIpcContainerInfo >( getId() );
      containerInfoPacket->data().containerSequence = sequence;
      containerInfoPacket->data().numItems = it->second->getEntryCount();
      containerInfoPacket->data().containerId = it->second->getId();

      queuePacket( containerInfoPacket );
    }
  }
}

void Sapphire::Entity::Player::sendExtraCurrency()
{
  auto& exdData = Common::Service< Data::ExdDataGenerated >::ref();

  if ( isLogin() )
  {
    for ( uint8_t i = 0; i < 28; i++ )
    {
      auto currency = exdData.get< Sapphire::Data::Item >( static_cast< uint16_t >( CurrencyCrystal::m_extraCurrencyIds[ i ].first ) );

      queuePacket( makeActorControlSelf( getId(),
                                         ExtraCurrency,
                                         CurrencyCrystal::m_extraCurrencyIds[ i ].second,
                                         static_cast< uint16_t >( CurrencyCrystal::m_extraCurrencyIds[ i ].first ),
                                         currency->stackSize,
                                         getCurrencyCrystal( CurrencyCrystal::m_extraCurrencyIds[ i ].first ),
                                         currency->stackSize >= 65000 ? 1 : 0 ) );
    }
  }

  auto lastCurrencyId = CurrencyCrystal::m_extraCurrencyIds.back().first;

  auto currency = exdData.get< Sapphire::Data::Item >( static_cast< uint16_t >( lastCurrencyId ) );

  //I don't known what this exactly is, but it's sent on every zone change except when you enter an instance
  queuePacket( makeActorControlSelf( getId(),
                                     ExtraCurrency,
                                     255,
                                     static_cast< uint16_t >( lastCurrencyId ),
                                     currency->stackSize,
                                     getCurrencyCrystal( lastCurrencyId ),
                                     1 ) );
}

void Sapphire::Entity::Player::updateCurrencyCrystalDb( std::string& tableName, Sapphire::CurrencyCrystalPtr pItem) const
{
  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto currencyCrystal = db.query("SELECT CharacterId FROM " + tableName +
                                  " WHERE CharacterId = " + std::to_string( getId() ) + "; ");

  if ( currencyCrystal->rowsCount() == 0 )
    writeCurrencyCrystalDb( tableName );

  if ( tableName == "charaextracurrency")
  {
    db.execute( "UPDATE charaextracurrency SET Currency_" + std::to_string( pItem->getUId() ) +
                " = 0 WHERE CharacterId = " + std::to_string( getId() ) );
  }
  else
  {
    db.execute( "UPDATE " + tableName +
                " SET container_" + std::to_string( pItem->getUId() ) + " = " + std::to_string( pItem->getAmount() ) +
                " WHERE CharacterId = " + std::to_string( getId() ) + ";" );
  }
}