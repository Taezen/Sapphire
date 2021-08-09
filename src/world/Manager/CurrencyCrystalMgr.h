#ifndef SAPPHIRE_CURRENCYCRYSTALMGR_H
#define SAPPHIRE_CURRENCYCRYSTALMGR_H

#include <Common.h>
#include "ForwardsZone.h"

namespace Sapphire::World::Manager
{

  class CurrencyCrystalMgr
  {
  public:
    CurrencyCrystalMgr() = default;

    CurrencyCrystalPtr loadCurrency( int8_t slotId, uint32_t quantity );
    CurrencyCrystalPtr loadCrystal( int8_t slotId, uint32_t quantity );
    CurrencyCrystalPtr loadExtraCurrency( int8_t slotId, uint32_t quantity );

    uint32_t getNextUId();

    static Common::ContainerType getContainerType();
  };

}

#endif //SAPPHIRE_ITEMMGR_H
