#ifndef _CURRENCYCRYSTALCONTAINER_H_
#define _CURRENCYCRYSTALCONTAINER_H_

#include <map>
#include <Common.h>
#include "ForwardsZone.h"

namespace Sapphire
{

  using CurrencyCrystalMap = std::map< uint8_t, CurrencyCrystalPtr >;

  class CurrencyCrystalContainer
  {

  public:
    CurrencyCrystalContainer( Common::InventoryType storageId, uint8_t maxSize, const std::string& tableName);

    ~CurrencyCrystalContainer();

    Common::InventoryType getId() const;

    uint8_t getEntryCount() const;

    void removeCurrencyCrystal( uint8_t slotId, bool removeFromDb = true );

    CurrencyCrystalMap& getCurrencyCrystalMap();

    const CurrencyCrystalMap& getCurrencyCrystalMap() const;

    CurrencyCrystalPtr getCurrencyCrystal( uint8_t slotId );

    void setCurrencyCrystal( CurrencyCrystalPtr pCurrencyCrystal );

    uint8_t getMaxSize() const;

    std::string getTableName() const;

    bool isMultiStorage() const;

    bool isPersistentStorage() const;

  private:
    Common::InventoryType m_id;
    uint8_t m_size;
    std::string m_tableName;
    CurrencyCrystalMap m_currencyCrystalMap;
    Entity::PlayerPtr m_pOwner;
  };

}

#endif
