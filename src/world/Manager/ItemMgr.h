#ifndef SAPPHIRE_ITEMMGR_H
#define SAPPHIRE_ITEMMGR_H

#include <Common.h>
#include "ForwardsZone.h"

namespace Sapphire::World::Manager
{

  class ItemMgr
  {
  public:
    ItemMgr() = default;

    ItemPtr loadItem( uint64_t uId );

    uint32_t getNextUId();

    /*! check if weapon category qualifies the weapon as onehanded */
    static bool isOneHandedWeapon( Common::ItemUICategory weaponCategory );
    static bool isArmory( Common::InventoryType containerId );
    static bool isEquipment( Common::InventoryType containerId );
    static Common::InventoryType getCharaEquipSlotCategoryToArmoryId( Common::EquipSlotCategory slot );
    static Common::ContainerType getContainerType( Common::InventoryType containerId );
  };

}

#endif //SAPPHIRE_ITEMMGR_H
