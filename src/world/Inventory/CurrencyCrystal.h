#ifndef _CURRENCYCRYSTAL_H_
#define _CURRENCYCRYSTAL_H_

#include <array>

#include <Common.h>
#include "ForwardsZone.h"

namespace Sapphire
{

  class CurrencyCrystal
  {

  public:

    CurrencyCrystal( uint8_t uId, Common::CurrencyCrystalType catalogId );

    virtual ~CurrencyCrystal() = default;

    Common::CurrencyCrystalType getId() const;

    uint8_t getUId() const;

    uint8_t getFilterGroup() const;

    Common::ItemUICategory getCategory() const;

    void setAmount( uint32_t size );
    uint32_t getAmount() const;

    uint32_t getMaxAmount() const;

  protected:
    Common::CurrencyCrystalType m_id;

    uint8_t m_uId;

    uint8_t m_filterGroup;
    Common::ItemUICategory m_category;

    uint32_t m_amount;
    uint32_t m_maxAmount;

  public:
    static const std::array< std::pair< Sapphire::Common::CurrencyCrystalType, uint8_t >, 29 > m_extraCurrencyIds;
    static const std::array< std::pair< Sapphire::Common::CurrencyCrystalType, uint8_t >, 11 > m_currencyIds;

  };

}

#endif
