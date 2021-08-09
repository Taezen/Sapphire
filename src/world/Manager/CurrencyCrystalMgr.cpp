#include "CurrencyCrystalMgr.h"

#include "Inventory/CurrencyCrystalContainer.h"
#include "Inventory/CurrencyCrystal.h"
#include <Network/CommonActorControl.h>

#include <Exd/ExdDataGenerated.h>
#include <Logging/Logger.h>
#include <Database/DatabaseDef.h>
#include <Service.h>

Sapphire::CurrencyCrystalPtr Sapphire::World::Manager::CurrencyCrystalMgr::loadCurrency( int8_t slotId, uint32_t quantity )
{
  try
  {
    Common::CurrencyCrystalType itemId;

    for( auto itr : CurrencyCrystal::m_currencyIds )
    {
      if ( itr.second == slotId )
      {
        itemId = itr.first;

        break;
      }
    }

    CurrencyCrystalPtr pCurrencyCrystal = make_CurrencyCrystal( slotId, itemId );

    pCurrencyCrystal->setAmount( quantity );

    return pCurrencyCrystal;
  }
  catch( ... )
  {
    return nullptr;
  }
}

Sapphire::CurrencyCrystalPtr Sapphire::World::Manager::CurrencyCrystalMgr::loadCrystal( int8_t slotId, uint32_t quantity )
{
  try
  {
    CurrencyCrystalPtr pCurrencyCrystal = make_CurrencyCrystal( slotId, static_cast< Common::CurrencyCrystalType >( slotId + 2 ) );

    pCurrencyCrystal->setAmount( quantity );

    return pCurrencyCrystal;
  }
  catch ( ... )
  {
    return nullptr;
  }
}

Sapphire::CurrencyCrystalPtr Sapphire::World::Manager::CurrencyCrystalMgr::loadExtraCurrency( int8_t slotId, uint32_t quantity )
{
  try
  {
    CurrencyCrystalPtr pCurrencyCrystal = make_CurrencyCrystal( slotId, CurrencyCrystal::m_extraCurrencyIds[ slotId ].first );

    pCurrencyCrystal->setAmount( quantity );

    return pCurrencyCrystal;
  }
  catch ( ... )
  {
    return nullptr;
  }
}


Sapphire::Common::ContainerType Sapphire::World::Manager::CurrencyCrystalMgr::getContainerType()
{
  return Common::CurrencyCrystal;
}

uint32_t Sapphire::World::Manager::CurrencyCrystalMgr::getNextUId()
{
  uint32_t charId = 0;

  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto pQR = db.query( "SELECT MAX(ItemId) FROM charaglobalitem" );

  if( !pQR->next() )
    return 0x00500001;

  charId = pQR->getUInt( 1 ) + 1;
  if( charId < 0x00500001 )
    return 0x00500001;

  return charId;
}
