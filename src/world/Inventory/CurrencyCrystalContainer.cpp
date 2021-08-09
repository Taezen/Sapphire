#include <Common.h>
#include <Logging/Logger.h>
#include <Database/DatabaseDef.h>
#include <Service.h>

#include "Actor/Player.h"

#include "CurrencyCrystal.h"
#include "Forwards.h"
#include "CurrencyCrystalContainer.h"

Sapphire::CurrencyCrystalContainer::CurrencyCrystalContainer(Common::InventoryType storageId, uint8_t maxSize, const std::string& tableName) :
  m_id( storageId ),
  m_size( maxSize ),
  m_tableName( tableName )
{

}

Sapphire::CurrencyCrystalContainer::~CurrencyCrystalContainer()
{

}

Sapphire::Common::InventoryType Sapphire::CurrencyCrystalContainer::getId() const
{
  return m_id;
}

uint8_t Sapphire::CurrencyCrystalContainer::getEntryCount() const
{
  return static_cast< uint8_t >( m_currencyCrystalMap.size() );
}

void Sapphire::CurrencyCrystalContainer::removeCurrencyCrystal( uint8_t slotId, bool removeFromDb )
{
  auto& db = Common::Service< Db::DbWorkerPool< Db::ZoneDbConnection > >::ref();
  auto it = m_currencyCrystalMap.find( slotId );

  if ( it != m_currencyCrystalMap.end() )
  {
    it->second->setAmount( 0 );

    m_pOwner->updateCurrencyCrystalDb(m_tableName, it->second);

    m_currencyCrystalMap.erase(it);

    Logger::debug( "Dropped currency from slot {0}", slotId );
  }
  else
  {
    Logger::debug( "Currency could not be dropped from slot {0}", slotId );
  }
}

Sapphire::CurrencyCrystalMap& Sapphire::CurrencyCrystalContainer::getCurrencyCrystalMap()
{
  return m_currencyCrystalMap;
}

const Sapphire::CurrencyCrystalMap& Sapphire::CurrencyCrystalContainer::getCurrencyCrystalMap() const
{
  return m_currencyCrystalMap;
}

Sapphire::CurrencyCrystalPtr Sapphire::CurrencyCrystalContainer::getCurrencyCrystal( uint8_t slotId )
{

  if ((slotId >= m_size))
  {
    Logger::error( "Slot out of range {0}", slotId );
    return nullptr;
  }

  return m_currencyCrystalMap[ slotId ];
}

void Sapphire::CurrencyCrystalContainer::setCurrencyCrystal( CurrencyCrystalPtr pCurrencyCrystal )
{
  m_currencyCrystalMap[ pCurrencyCrystal->getUId() ] = pCurrencyCrystal;
}

uint8_t Sapphire::CurrencyCrystalContainer::getMaxSize() const
{
  return m_size;
}

std::string Sapphire::CurrencyCrystalContainer::getTableName() const
{
  return m_tableName;
}
