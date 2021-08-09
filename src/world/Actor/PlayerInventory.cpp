#include <Common.h>
#include <Logging/Logger.h>
#include <Network/CommonActorControl.h>
#include <algorithm>

#include "Territory/Territory.h"

#include "Network/PacketWrappers/ActorControlPacket.h"
#include "Network/PacketWrappers/ActorControlSelfPacket.h"
#include "Network/PacketWrappers/UpdateInventorySlotPacket.h"

#include "Inventory/Item.h"
#include "Inventory/ItemContainer.h"


#include "Player.h"

#include <Network/PacketDef/Zone/ServerZoneDef.h>

#include <Exd/ExdDataGenerated.h>
#include <Logging/Logger.h>
#include <Database/DatabaseDef.h>

#include "Actor/Player.h"

#include "Network/PacketWrappers/ServerNoticePacket.h"
#include "Network/PacketWrappers/ActorControlSelfPacket.h"

#include "Manager/InventoryMgr.h"
#include "Manager/ItemMgr.h"

#include <Service.h>

using namespace Sapphire::Common;
using namespace Sapphire::Network::Packets;
using namespace Sapphire::Network::Packets::Server;
using namespace Sapphire::Network::ActorControl;


void Sapphire::Entity::Player::initInventory()
{
  auto setupContainer = [ this ]( InventoryType type, uint8_t maxSize, const std::string& tableName,
                                  bool isMultiStorage, bool isPersistentStorage = true )
  { m_storageMap[ type ] = make_ItemContainer( type, maxSize, tableName, isMultiStorage, isPersistentStorage ); };

  // main bags
  setupContainer( InventoryType::Bag0, 35, "charaiteminventory", true );
  setupContainer( InventoryType::Bag1, 35, "charaiteminventory", true );
  setupContainer( InventoryType::Bag2, 35, "charaiteminventory", true );
  setupContainer( InventoryType::Bag3, 35, "charaiteminventory", true );

  // gear set
  setupContainer( InventoryType::GearSet0, 14, "charaitemgearset", true );

  // armory weapons - 0
  setupContainer( InventoryType::ArmoryMain, 35, "charaiteminventory", true );

  // armory offhand - 1
  setupContainer( InventoryType::ArmoryOff, 35, "charaiteminventory", true );

  //armory head - 2
  setupContainer( InventoryType::ArmoryHead, 35, "charaiteminventory", true );

  //armory body - 3
  setupContainer( InventoryType::ArmoryBody, 35, "charaiteminventory", true );

  //armory hand - 4
  setupContainer( InventoryType::ArmoryHand, 35, "charaiteminventory", true );

  //armory waist - 5
  setupContainer( InventoryType::ArmoryWaist, 35, "charaiteminventory", true );

  //armory legs - 6
  setupContainer( InventoryType::ArmoryLegs, 35, "charaiteminventory", true );

  //armory feet - 7
  setupContainer( InventoryType::ArmoryFeet, 35, "charaiteminventory", true );

  //neck
  setupContainer( InventoryType::ArmoryNeck, 35, "charaiteminventory", true );

  //earring
  setupContainer( InventoryType::ArmoryEar, 35, "charaiteminventory", true );

  //wrist
  setupContainer( InventoryType::ArmoryWrist, 35, "charaiteminventory", true );

  //armory rings - 11
  setupContainer( InventoryType::ArmoryRing, 35, "charaiteminventory", true );

  //soul crystals - 13
  setupContainer( InventoryType::ArmorySoulCrystal, 35, "charaiteminventory", true );

  // item hand in container
  // non-persistent container, will not save its contents
  setupContainer( InventoryType::HandIn, 10, "", true, false );

  loadInventory();

}

void Sapphire::Entity::Player::sendItemLevel()
{
  queuePacket( makeActorControl( getId(), SetItemLevel, getItemLevel(), 0 ) );
}

void Sapphire::Entity::Player::equipWeapon( ItemPtr pItem, bool updateClass )
{
  auto& exdData = Common::Service< Sapphire::Data::ExdDataGenerated >::ref();

  auto itemInfo = exdData.get< Sapphire::Data::Item >( pItem->getId() );
  auto itemClassJob = itemInfo->classJobUse;
  auto classJobInfo = exdData.get< Sapphire::Data::ClassJob >( static_cast< uint32_t >( getClass() ) );
  auto currentParentClass = static_cast< ClassJob >( classJobInfo->classJobParent );
  auto newClassJob = static_cast< ClassJob >( itemClassJob );

  if( ( isClassJobUnlocked( newClassJob ) ) && ( currentParentClass != newClassJob ) )
  {
    if ( updateClass )
      setClassJob( newClassJob );
    else
      return;
  }
}

void Sapphire::Entity::Player::equipSoulCrystal( ItemPtr pItem, bool updateJob )
{
  auto& exdData = Common::Service< Sapphire::Data::ExdDataGenerated >::ref();

  auto itemInfo = exdData.get< Sapphire::Data::Item >( pItem->getId() );
  auto itemClassJob = itemInfo->classJobUse;
  auto newClassJob = static_cast< ClassJob >( itemClassJob );

  if ( isClassJobUnlocked( newClassJob ) && updateJob )
    setClassJob( newClassJob );
}

void Sapphire::Entity::Player::updateModels( GearSetSlot equipSlotId, const Sapphire::ItemPtr& pItem, bool updateClass )
{
  uint64_t model = pItem->getModelId1();
  uint64_t model2 = pItem->getModelId2();
  uint64_t stain = pItem->getStain();

  switch( equipSlotId )
  {
    case MainHand:
      m_modelMainWeapon = model | ( stain << 48 );

      m_modelSubWeapon = model2;

      if( m_modelSubWeapon > 0 )
      {
        m_modelSubWeapon = m_modelSubWeapon | ( stain << 48 );
      }

      equipWeapon( pItem, updateClass );
      break;

    case OffHand:
      m_modelSubWeapon = model | ( stain << 48 );
      break;

    case SoulCrystal:
      equipSoulCrystal( pItem, updateClass );
      break;

    case Waist:
      break;

    default: // any other slot
      auto modelSlot = equipSlotToModelSlot( equipSlotId );
      if( modelSlot == GearModelSlot::ModelInvalid )
        break;

      model = model | stain << 24;
      m_modelEquip[ static_cast< uint8_t >( modelSlot ) ] = static_cast< uint32_t >( model );
      break;

  }
}

Sapphire::Common::GearModelSlot Sapphire::Entity::Player::equipSlotToModelSlot( Common::GearSetSlot slot )
{
  switch( slot )
  {
    case MainHand:
    case OffHand:
    case Waist:
    case SoulCrystal:
    default:
      return GearModelSlot::ModelInvalid;
    case Head:
      return GearModelSlot::ModelHead;
    case Body:
      return GearModelSlot::ModelBody;
    case Hands:
      return GearModelSlot::ModelHands;
    case Legs:
      return GearModelSlot::ModelLegs;
    case Feet:
      return GearModelSlot::ModelFeet;
    case Neck:
      return GearModelSlot::ModelNeck;
    case Ear:
      return GearModelSlot::ModelEar;
    case Wrist:
      return GearModelSlot::ModelWrist;
    case Ring1:
      return GearModelSlot::ModelRing1;
    case Ring2:
      return GearModelSlot::ModelRing2;
  }
}

// equip an item
void Sapphire::Entity::Player::equipItem( Common::GearSetSlot equipSlotId, ItemPtr pItem, bool sendUpdate )
{

  //g_framework.getLogger().debug( "Equipping into slot " + std::to_string( equipSlotId ) );
  if( sendUpdate )
  {
    updateModels( equipSlotId, pItem, true );
    sendModel();
    m_itemLevel = calculateEquippedGearItemLevel();
    sendItemLevel();
  }
  else
    updateModels( equipSlotId, pItem, false );

  auto baseParams = pItem->getBaseParams();
  for( auto i = 0; i < 6; ++i )
  {
    if( baseParams[ i ].baseParam != static_cast< uint8_t >( Common::BaseParam::None ) )
      m_bonusStats[ baseParams[ i ].baseParam ] += baseParams[ i ].value;
  }

  m_bonusStats[ static_cast< uint8_t >( Common::BaseParam::Defense ) ] += pItem->getDefense();
  m_bonusStats[ static_cast< uint8_t >( Common::BaseParam::MagicDefense ) ] += pItem->getDefenseMag();

  calculateStats();
  if( sendUpdate )
  {
    sendStats();
    sendStatusUpdate();
  }
}


void Sapphire::Entity::Player::unequipItem( Common::GearSetSlot equipSlotId, ItemPtr pItem, bool sendUpdate )
{
  auto modelSlot = equipSlotToModelSlot( equipSlotId );
  if( modelSlot != GearModelSlot::ModelInvalid )
    m_modelEquip[ static_cast< uint8_t >( modelSlot ) ] = 0;

  if( sendUpdate )
  {
    sendModel();

    m_itemLevel = calculateEquippedGearItemLevel();
    sendItemLevel();
  }

  if ( equipSlotId == SoulCrystal )
    unequipSoulCrystal( pItem );

  auto baseParams = pItem->getBaseParams();
  for( auto i = 0; i < 6; ++i )
  {
    if( baseParams[ i ].baseParam != static_cast< uint8_t >( Common::BaseParam::None ) )
      m_bonusStats[ baseParams[ i ].baseParam ] -= baseParams[ i ].value;
  }

  m_bonusStats[ static_cast< uint8_t >( Common::BaseParam::Defense ) ] -= pItem->getDefense();
  m_bonusStats[ static_cast< uint8_t >( Common::BaseParam::MagicDefense ) ] -= pItem->getDefenseMag();

  calculateStats();

  if( sendUpdate )
  {
    sendStats();
    sendStatusUpdate();
  }
}

void Sapphire::Entity::Player::unequipSoulCrystal( ItemPtr pItem )
{
  auto& exdData = Common::Service< Sapphire::Data::ExdDataGenerated >::ref();

  auto currentClassJob = exdData.get< Sapphire::Data::ClassJob >( static_cast< uint32_t >( getClass() ) );
  auto parentClass = static_cast< ClassJob >( currentClassJob->classJobParent );
  setClassJob( parentClass );
}

void Sapphire::Entity::Player::sendInventory()
{
  auto& invMgr = Common::Service< World::Manager::InventoryMgr >::ref();

  sendExtraCurrency();

  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::Bag0 ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::Bag1 ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::Bag2 ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::Bag3 ] );
  
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::GearSet0 ] );

  sendCurrencyCrystal();

  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryMain ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryOff ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryHead ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryBody ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryHand ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryWaist ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryLegs ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryFeet ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryEar ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryNeck ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryWrist ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmoryRing ] );
  invMgr.sendInventoryContainer( *this, m_storageMap[ InventoryType::ArmorySoulCrystal ] );
}

Sapphire::Entity::Player::InvSlotPairVec Sapphire::Entity::Player::getSlotsOfItemsInInventory( uint32_t catalogId )
{
  InvSlotPairVec outVec;
  for( auto i : { InventoryType::Bag0, InventoryType::Bag1, InventoryType::Bag2, InventoryType::Bag3 } )
  {
    auto inv = m_storageMap[ i ];
    for( auto item : inv->getItemMap() )
    {
      if( item.second && item.second->getId() == catalogId )
        outVec.push_back( std::make_pair( i, static_cast< int8_t >( item.first ) ) );
    }
  }
  return outVec;
}

Sapphire::Entity::Player::InvSlotPair Sapphire::Entity::Player::getFreeBagSlot()
{
  for( auto i : { InventoryType::Bag0, InventoryType::Bag1, InventoryType::Bag2, InventoryType::Bag3 } )
  {
    auto freeSlot = static_cast< int8_t >( m_storageMap[ i ]->getFreeSlot() );

    if( freeSlot != -1 )
      return std::make_pair( i, freeSlot );
  }
  // no room in inventory
  return std::make_pair( InventoryType::Bag0, -1 );
}

Sapphire::ItemPtr Sapphire::Entity::Player::getItemAt( Common::InventoryType containerId, uint8_t slotId )
{
  return m_storageMap[ containerId ]->getItem( slotId );
}

void Sapphire::Entity::Player::writeInventory( InventoryType type )
{
  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();

  auto storage = m_storageMap[ type ];

  if( !storage->isPersistentStorage() )
    return;

  std::string query = "UPDATE " + storage->getTableName() + " SET ";

  for( int32_t i = 0; i < storage->getMaxSize(); i++ )
  {
    auto currItem = storage->getItem( i );

    if( i > 0 )
      query += ", ";

    query += "container_" + std::to_string( i ) + " = " + std::to_string( currItem ? currItem->getUId() : 0 );
  }

  query += " WHERE CharacterId = " + std::to_string( getId() );

  if( storage->isMultiStorage() )
    query += " AND storageId = " + std::to_string( static_cast< uint16_t >( type ) );

  db.execute( query );
}

void Sapphire::Entity::Player::updateItemDb( Sapphire::ItemPtr pItem ) const
{
  if( pItem->getUId() == 0 )
    writeItemDb( pItem );

  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto stmt = db.getPreparedStatement( Db::CHARA_ITEMGLOBAL_UP );

  // todo: add more fields
  stmt->setInt( 1, pItem->getStackSize() );
  stmt->setInt( 2, pItem->getDurability() );
  stmt->setInt( 3, pItem->getStain() );

  stmt->setInt64( 4, pItem->getUId() );

  db.directExecute( stmt );
}

void Sapphire::Entity::Player::deleteItemDb( Sapphire::ItemPtr item ) const
{
  if( item->getUId() == 0 )
    return;

  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto stmt = db.getPreparedStatement( Db::CHARA_ITEMGLOBAL_DELETE );

  stmt->setInt64( 1, item->getUId() );

  db.directExecute( stmt );
}


bool Sapphire::Entity::Player::isObtainable( uint32_t catalogId, uint8_t quantity )
{
  return true;
}

Sapphire::ItemPtr Sapphire::Entity::Player::addItem( ItemPtr itemToAdd, bool silent, bool canMerge, bool sendLootMessage )
{
  auto& exdData = Common::Service< Data::ExdDataGenerated >::ref();
  auto itemInfo = exdData.get< Sapphire::Data::Item >( itemToAdd->getId() );

  // if item data doesn't exist or it's a blank field
  if( !itemInfo || itemInfo->levelItem == 0 )
  {
    return nullptr;
  }

  itemToAdd->setStackSize( std::min< uint32_t >( itemToAdd->getStackSize(), itemInfo->stackSize ) );

  // used for item obtain notification
  uint32_t originalQuantity = itemToAdd->getStackSize();

  std::pair< InventoryType, uint8_t > freeBagSlot;
  bool foundFreeSlot = false;

  std::vector< InventoryType > bags = { InventoryType::Bag0, InventoryType::Bag1, InventoryType::Bag2, InventoryType::Bag3 };
  sendDebug( "adding item: {}, equipSlotCategory: {}, stackSize: {}", itemToAdd->getId(), itemInfo->equipSlotCategory, itemInfo->stackSize );
  // add the related armoury bag to the applicable bags and try and fill a free slot there before falling back to regular inventory
  if( itemInfo->equipSlotCategory > 0 && getEquipDisplayFlags() & StoreNewItemsInArmouryChest )
  {
    auto bag = World::Manager::ItemMgr::getCharaEquipSlotCategoryToArmoryId( static_cast< Common::EquipSlotCategory >( itemInfo->equipSlotCategory ) );

    bags.insert( bags.begin(), bag );
  }

  for( auto bag : bags )
  {
    auto storage = m_storageMap[ bag ];

    for( uint8_t slot = 0; slot < storage->getMaxSize(); slot++ )
    {
      if( !canMerge && foundFreeSlot )
        break;

      auto item = storage->getItem( slot );

      // add any items that are stackable
      if( canMerge && item && item->getMaxStackSize() > 1 && item->getId() == itemToAdd->getId() )
      {
        uint32_t count = item->getStackSize();
        uint32_t maxStack = item->getMaxStackSize();

        // if slot is full, skip it
        if( count >= maxStack )
          continue;

        // check slot is same quality
        if( item->isHq() != itemToAdd->isHq() )
          continue;

        // update stack
        uint32_t newStackSize = count + itemToAdd->getStackSize();
        if( newStackSize > maxStack )
        {
          itemToAdd->setStackSize( newStackSize - maxStack );
          newStackSize = maxStack;
        }
        else
          itemToAdd->setStackSize( 0 );

        item->setStackSize( newStackSize );
        updateItemDb( item );

        if( !silent )
        {
          auto slotUpdate = std::make_shared< UpdateInventorySlotPacket >( getId(), slot, bag, *item );
          queuePacket( slotUpdate );
        }

        // return existing stack if we have no overflow - items fit into a preexisting stack
        if( itemToAdd->getStackSize() == 0 )
        {
          if( sendLootMessage )
          {
            auto lootMsg = makeZonePacket< FFXIVIpcLootMessage >( getId() );
            lootMsg->data().msgType = Common::LootMessageType::GetItem2;
            lootMsg->data().param1 = getId();
            lootMsg->data().param2 = itemToAdd->isHq() ? itemToAdd->getId() + 1000000 : itemToAdd->getId();
            lootMsg->data().param3 = originalQuantity;
            queuePacket( lootMsg );
          }
          if( !silent )
          {
            auto soundEffectPacket = makeZonePacket< FFXIVIpcInventoryActionAck >( getId() );
            soundEffectPacket->data().sequence = 0xFFFFFFFF;
            soundEffectPacket->data().type = 6;
            queuePacket( soundEffectPacket );
          }
          return item;
        }

      }
      else if( !item && !foundFreeSlot )
      {
        freeBagSlot = { bag, slot };
        foundFreeSlot = true;
      }
    }
  }

  // couldn't find a free slot and we still have some quantity of items left, shits fucked
  if( !foundFreeSlot )
    return nullptr;

  writeItemDb( itemToAdd );

  auto storage = m_storageMap[ freeBagSlot.first ];
  storage->setItem( freeBagSlot.second, itemToAdd );

  writeInventory( static_cast< InventoryType >( freeBagSlot.first ) );

  if( !silent )
  {
    auto invUpdate = std::make_shared< UpdateInventorySlotPacket >( getId(), freeBagSlot.second, freeBagSlot.first, *itemToAdd );
    queuePacket( invUpdate );

    auto soundEffectPacket = makeZonePacket< FFXIVIpcInventoryActionAck >( getId() );
    soundEffectPacket->data().sequence = 0xFFFFFFFF;
    soundEffectPacket->data().type = 6;
    queuePacket( soundEffectPacket );
  }

  if( sendLootMessage )
  {
    auto lootMsg = makeZonePacket< FFXIVIpcLootMessage >( getId() );
    lootMsg->data().msgType = Common::LootMessageType::GetItem2;
    lootMsg->data().param1 = getId();
    lootMsg->data().param2 = itemToAdd->isHq() ? itemToAdd->getId() + 1000000 : itemToAdd->getId();
    lootMsg->data().param3 = originalQuantity;
    queuePacket( lootMsg );
  }

  return itemToAdd;
}


Sapphire::ItemPtr Sapphire::Entity::Player::addItem( uint32_t catalogId, uint32_t quantity, bool isHq, bool silent, bool canMerge, bool sendLootMessage )
{
  if( catalogId == 0 )
    return nullptr;
  auto item = createTempItem( catalogId, quantity );
  item->setHq( isHq );
  return addItem( item, silent, canMerge, sendLootMessage );
}

void
Sapphire::Entity::Player::moveItem( InventoryType fromInventoryId, uint8_t fromSlotId, InventoryType toInventoryId, uint8_t toSlot )
{

  auto tmpItem = m_storageMap[ fromInventoryId ]->getItem( fromSlotId );
  auto& itemMap = m_storageMap[ fromInventoryId ]->getItemMap();

  if( tmpItem == nullptr )
    return;

  itemMap[ fromSlotId ].reset();

  m_storageMap[ toInventoryId ]->setItem( toSlot, tmpItem );

  writeInventory( static_cast< InventoryType >( toInventoryId ) );

  if( fromInventoryId != toInventoryId )
    writeInventory( static_cast< InventoryType >( fromInventoryId ) );

  if( static_cast< InventoryType >( toInventoryId ) == InventoryType::GearSet0 )
    equipItem( static_cast< GearSetSlot >( toSlot ), tmpItem, true );

  if( static_cast< InventoryType >( fromInventoryId ) == InventoryType::GearSet0 )
    unequipItem( static_cast< GearSetSlot >( fromSlotId ), tmpItem, true );

  if( static_cast< InventoryType >( toInventoryId ) == InventoryType::GearSet0 ||
      static_cast< InventoryType >( fromInventoryId ) == InventoryType::GearSet0 )
    sendStatusEffectUpdate(); // send if any equip is changed
}

bool Sapphire::Entity::Player::updateContainer( InventoryType storageId, uint8_t slotId, ItemPtr pItem )
{
  auto containerType = World::Manager::ItemMgr::getContainerType( storageId );

  auto pOldItem = getItemAt( storageId, slotId );
  m_storageMap[ storageId ]->setItem( slotId, pItem );

  switch( containerType )
  {
    case Armory:
    case Bag:
    {
      writeInventory( storageId );
      break;
    }

    case GearSet:
    {
      if( pItem )
      {
        if( pOldItem )
          unequipItem( static_cast< GearSetSlot >( slotId ), pOldItem, false );
        equipItem( static_cast< GearSetSlot >( slotId ), pItem, true );
      }
      else
        unequipItem( static_cast< GearSetSlot >( slotId ), pItem, true );

      writeInventory( storageId );
      break;
    }
    default:
      break;
  }

  return true;
}

void Sapphire::Entity::Player::splitItem( InventoryType fromInventoryId, uint8_t fromSlotId,
                                          InventoryType toInventoryId, uint8_t toSlot, uint16_t itemCount )
{
  if( itemCount == 0 )
    return;

  auto fromItem = m_storageMap[ fromInventoryId ]->getItem( fromSlotId );
  if( !fromItem )
    return;

  // check we have enough items in the origin slot
  // nb: don't let the client 'split' a whole stack into another slot
  if( fromItem->getStackSize() < itemCount )
    // todo: correct the invalid item split? does retail do this or does it just ignore it?
    return;

  // make sure toInventoryId & toSlot are actually free so we don't orphan an item
  if( m_storageMap[ toInventoryId ]->getItem( toSlot ) )
    // todo: correct invalid move? again, not sure what retail does here
    return;

  auto newItem = addItem( fromItem->getId(), itemCount, fromItem->isHq(), true, false );
  if( !newItem )
    return;

  fromItem->setStackSize( fromItem->getStackSize() - itemCount );

  updateContainer( fromInventoryId, fromSlotId, fromItem );
  updateContainer( toInventoryId, toSlot, newItem );

  updateItemDb( fromItem );
}

void Sapphire::Entity::Player::mergeItem( InventoryType fromInventoryId, uint8_t fromSlotId,
                                          InventoryType toInventoryId, uint8_t toSlot )
{
  auto fromItem = m_storageMap[ fromInventoryId ]->getItem( fromSlotId );
  auto toItem = m_storageMap[ toInventoryId ]->getItem( toSlot );

  if( !fromItem || !toItem )
    return;

  if( fromItem->getId() != toItem->getId() )
    return;

  uint32_t stackSize = fromItem->getStackSize() + toItem->getStackSize();
  uint32_t stackOverflow = stackSize - std::min< uint32_t >( fromItem->getMaxStackSize(), stackSize );

  // we can destroy the original stack if there's no overflow
  if( stackOverflow == 0 )
  {
    m_storageMap[ fromInventoryId ]->removeItem( fromSlotId );
    deleteItemDb( fromItem );
  }
  else
  {
    fromItem->setStackSize( stackOverflow );
    updateItemDb( fromItem );
    updateContainer( fromInventoryId, fromSlotId, fromItem );
  }


  toItem->setStackSize( stackSize );
  updateItemDb( toItem );

  updateContainer( toInventoryId, toSlot, toItem );
}

void Sapphire::Entity::Player::swapItem( InventoryType fromInventoryId, uint8_t fromSlotId,
                                         InventoryType toInventoryId, uint8_t toSlot )
{
  auto fromItem = m_storageMap[ fromInventoryId ]->getItem( fromSlotId );
  auto toItem = m_storageMap[ toInventoryId ]->getItem( toSlot );
  auto& itemMap = m_storageMap[ fromInventoryId ]->getItemMap();

  if( fromItem == nullptr || toItem == nullptr )
    return;

  // An item is being moved from bag0-3 to equippment, meaning
  // the swapped out item will be placed in the matching armory.
  if( World::Manager::ItemMgr::isEquipment( toInventoryId )
      && !World::Manager::ItemMgr::isEquipment( fromInventoryId )
      && !World::Manager::ItemMgr::isArmory( fromInventoryId ) )
  {
    updateContainer( fromInventoryId, fromSlotId, nullptr );
    fromInventoryId = World::Manager::ItemMgr::getCharaEquipSlotCategoryToArmoryId( static_cast< Common::EquipSlotCategory >( toSlot ) );
    fromSlotId = static_cast < uint8_t >( m_storageMap[ fromInventoryId ]->getFreeSlot() );
  }

  auto containerTypeFrom = World::Manager::ItemMgr::getContainerType( fromInventoryId );
  auto containerTypeTo = World::Manager::ItemMgr::getContainerType( toInventoryId );

  updateContainer( toInventoryId, toSlot, fromItem );
  updateContainer( fromInventoryId, fromSlotId, toItem );

  if( static_cast< InventoryType >( toInventoryId ) == InventoryType::GearSet0 ||
    static_cast< InventoryType >( fromInventoryId ) == InventoryType::GearSet0 )
    sendStatusEffectUpdate(); // send if any equip is changed
}

void Sapphire::Entity::Player::discardItem( InventoryType fromInventoryId, uint8_t fromSlotId )
{
  // i am not entirely sure how this should be generated or if it even is important for us...
  uint32_t sequence = getNextInventorySequence();

  auto fromItem = m_storageMap[ fromInventoryId ]->getItem( fromSlotId );

  deleteItemDb( fromItem );

  m_storageMap[ fromInventoryId ]->removeItem( fromSlotId );
  updateContainer( fromInventoryId, fromSlotId, nullptr );

  auto invTransPacket = makeZonePacket< FFXIVIpcInventoryTransaction >( getId() );
  invTransPacket->data().sequence = sequence;
  invTransPacket->data().ownerId = getId();
  invTransPacket->data().storageId = fromInventoryId;
  invTransPacket->data().catalogId = fromItem->getId();
  invTransPacket->data().stackSize = fromItem->getStackSize();
  invTransPacket->data().slotId = fromSlotId;
  invTransPacket->data().type = Common::InventoryOperation::Discard;
  queuePacket( invTransPacket );

  auto invTransFinPacket = makeZonePacket< FFXIVIpcInventoryTransactionFinish >( getId() );
  invTransFinPacket->data().sequenceId = sequence;
  invTransFinPacket->data().sequenceId1 = sequence;
  queuePacket( invTransFinPacket );
}

uint16_t Sapphire::Entity::Player::calculateEquippedGearItemLevel()
{
  uint32_t iLvlResult = 0;

  auto gearSetMap = m_storageMap[ InventoryType::GearSet0 ]->getItemMap();

  auto it = gearSetMap.begin();

  while( it != gearSetMap.end() )
  {
    auto currItem = it->second;

    if( currItem && currItem->getCategory() != Common::ItemUICategory::SoulCrystal )
    {
      iLvlResult += currItem->getItemLevel();

      // If item is weapon and isn't one-handed
      if( currItem->isWeapon() && !World::Manager::ItemMgr::isOneHandedWeapon( currItem->getCategory() ) )
      {
        iLvlResult += currItem->getItemLevel();
      }
    }

    it++;
  }

  return static_cast< uint16_t >( std::min( static_cast< int32_t >( iLvlResult / 13 ), 9999 ) );
}

Sapphire::ItemPtr Sapphire::Entity::Player::getEquippedWeapon()
{
  return m_storageMap[ InventoryType::GearSet0 ]->getItem( GearSetSlot::MainHand );
}

uint8_t Sapphire::Entity::Player::getFreeSlotsInBags()
{
  uint8_t slots = 0;
  for( InventoryType container : { InventoryType::Bag0, InventoryType::Bag1, InventoryType::Bag2, InventoryType::Bag3 } )
  {
    const auto& storage = m_storageMap[ container ];
    slots += ( storage->getMaxSize() - storage->getEntryCount() );
  }
  return slots;
}

bool Sapphire::Entity::Player::collectHandInItems( std::vector< uint32_t > itemIds )
{
  // todo: figure out how the game gets the required stack count
  const auto& container = m_storageMap[ InventoryType::HandIn ];

  std::vector< uint8_t > foundItems;

  auto itemMap = container->getItemMap();

  for( auto& item : itemMap )
  {
    for( auto needle : itemIds )
    {
      if( item.second->getId() == needle )
      {
        foundItems.push_back( item.first );
        break;
      }
    }
  }

  // couldn't find all the items required
  if( foundItems.size() != itemIds.size() )
    return false;

  // remove items
  for( auto item : foundItems )
  {
    container->removeItem( item );
  }

  return true;
}

uint32_t Sapphire::Entity::Player::getNextInventorySequence()
{
  return m_inventorySequence++;
}

Sapphire::ItemPtr Sapphire::Entity::Player::dropInventoryItem( Sapphire::Common::InventoryType type, uint16_t slotId, bool silent )
{
  auto& container = m_storageMap[ type ];

  auto item = container->getItem( slotId );
  if( !item )
    return nullptr;

  // unlink item
  container->removeItem( slotId, false );
  updateContainer( type, slotId, nullptr );

  if( !silent )
  {
    auto invUpdate = std::make_shared< UpdateInventorySlotPacket >( getId(), slotId, type );
    queuePacket( invUpdate );
  }

  auto seq = getNextInventorySequence();

  // send inv update
  auto invTransPacket = makeZonePacket< FFXIVIpcInventoryTransaction >( getId() );
  invTransPacket->data().sequence = seq;
  invTransPacket->data().ownerId = getId();
  invTransPacket->data().storageId = type;
  invTransPacket->data().catalogId = item->getId();
  invTransPacket->data().stackSize = item->getStackSize();
  invTransPacket->data().slotId = slotId;
  invTransPacket->data().type = 7;
  queuePacket( invTransPacket );

  auto invTransFinPacket = makeZonePacket< FFXIVIpcInventoryTransactionFinish >( getId() );
  invTransFinPacket->data().sequenceId = seq;
  invTransFinPacket->data().sequenceId1 = seq;
  queuePacket( invTransFinPacket );

  return item;
}

bool Sapphire::Entity::Player::getFreeInventoryContainerSlot( Inventory::InventoryContainerPair& containerPair ) const
{
  for( auto bagId : { InventoryType::Bag0, InventoryType::Bag1, InventoryType::Bag2, InventoryType::Bag3 } )
  {
    auto needle = m_storageMap.find( bagId );
    if( needle == m_storageMap.end() )
      continue;

    auto& container = needle->second;

    for( uint16_t idx = 0; idx < container->getMaxSize(); idx++ )
    {
      auto item = container->getItem( idx );
      if( !item )
      {
        containerPair = std::make_pair( bagId, idx );
        return true;
      }
    }
  }

  return false;
}

void Sapphire::Entity::Player::insertInventoryItem( Sapphire::Common::InventoryType type, uint8_t slot,
                                                    const Sapphire::ItemPtr item )
{
  updateContainer( type, slot, item );

  auto slotUpdate = std::make_shared< UpdateInventorySlotPacket >( getId(), slot, type, *item );
  queuePacket( slotUpdate );

}

bool Sapphire::Entity::Player::findFirstItemWithId( uint32_t catalogId,
                                                    Inventory::InventoryContainerPair& location )
{
  for( auto bagId : { InventoryType::Bag0, InventoryType::Bag1, InventoryType::Bag2, InventoryType::Bag3 } )
  {
    auto& container = m_storageMap[ bagId ];

    for( const auto& item : container->getItemMap() )
    {
      if( item.second->getId() != catalogId )
        continue;

      location = std::make_pair( bagId, item.first );

      return true;
    }
  }

  return false;
}