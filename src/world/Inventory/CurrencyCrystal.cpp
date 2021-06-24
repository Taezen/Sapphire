#include <Common.h>
#include <Exd/ExdDataGenerated.h>
#include <CommonGen.h>
#include <Service.h>

#include "CurrencyCrystal.h"

const std::array< std::pair< Sapphire::Common::CurrencyCrystalType, uint8_t >, 29 > Sapphire::CurrencyCrystal::m_extraCurrencyIds =
{
  //Item IDs | Slot IDs / only for the first 7
  {
    { Common::CurrencyCrystalType::CenturioSeal, 5 },
    { Common::CurrencyCrystalType::RedCraftersScrip, 1 },
    { Common::CurrencyCrystalType::RedGatherersScrip, 3 },
    { Common::CurrencyCrystalType::YellowCraftersScrip, 2 },
    { Common::CurrencyCrystalType::YellowGatherersScrip, 4 },
    { Common::CurrencyCrystalType::WhiteCraftersScrip, 6 },
    { Common::CurrencyCrystalType::WhiteGatherersScrip, 7 },
    { Common::CurrencyCrystalType::FauxLeaf, 0 },
    { Common::CurrencyCrystalType::Venture, 0 },
    { Common::CurrencyCrystalType::IxaliOaknot, 0 },
    { Common::CurrencyCrystalType::VanuWhitebone, 0 },
    { Common::CurrencyCrystalType::SylphicGoldleaf, 0 },
    { Common::CurrencyCrystalType::SteelAmaljok, 0 },
    { Common::CurrencyCrystalType::RainbowtidePsashp, 0 },
    { Common::CurrencyCrystalType::TitanCobaltpiece, 0 },
    { Common::CurrencyCrystalType::BlackCopperGil, 0 },
    { Common::CurrencyCrystalType::CarvedKupoNut, 0 },
    { Common::CurrencyCrystalType::KojinSango, 0 },
    { Common::CurrencyCrystalType::FaeFancy, 0 },
    { Common::CurrencyCrystalType::QitariCompliment, 0 },
    { Common::CurrencyCrystalType::HammeredFrogment, 0 },
    { Common::CurrencyCrystalType::SkybuildersScrip, 0 },
    { Common::CurrencyCrystalType::SackOfNuts, 0 },
    { Common::CurrencyCrystalType::AnantaDreamstaff, 0 },
    { Common::CurrencyCrystalType::AchievementCertificate, 0 },
    { Common::CurrencyCrystalType::UnknownItem0, 0 },
    { Common::CurrencyCrystalType::BicolorGemstone, 0 },
    { Common::CurrencyCrystalType::NamazuKoban, 0 },
    { Common::CurrencyCrystalType::UnknownItem1, 0 }
  }
};

const std::array< std::pair< Sapphire::Common::CurrencyCrystalType, uint8_t >, 11 > Sapphire::CurrencyCrystal::m_currencyIds =
{
  //Item IDs | Slot IDs
  {
    { Common::CurrencyCrystalType::Gil, 0 },
    { Common::CurrencyCrystalType::StormSeal, 1 }, //Red Crafters' Scrip
    { Common::CurrencyCrystalType::SerpentSeal, 2 }, //Red Gatherers' Scrip
    { Common::CurrencyCrystalType::FlameSeal, 3 }, //Yellow Crafters' Scrip
    { Common::CurrencyCrystalType::WolfMark, 4 }, //Yellow Gatherers' Scrip
    { Common::CurrencyCrystalType::TomestoneRev, 5 }, //White Crafters' Scrip
    { Common::CurrencyCrystalType::TomestonePoet, 6 }, //White Gatherers' Scrip
    { Common::CurrencyCrystalType::TomestoneAlle, 7 }, //Faux Leaf
    { Common::CurrencyCrystalType::AlliedSeal, 8 }, //Venture
    { Common::CurrencyCrystalType::Mgp, 9 }, //Ixali Oaknot
    { Common::CurrencyCrystalType::TomestonePhan, 10 }
  }
};

Sapphire::CurrencyCrystal::CurrencyCrystal( uint8_t uId, Common::CurrencyCrystalType catalogId ) :
  m_id( catalogId ),
  m_uId( uId )
{
  auto& exdData = Common::Service< Data::ExdDataGenerated >::ref();
  auto currencyInfo = exdData.get< Sapphire::Data::Item >( static_cast< uint16_t >( catalogId ) );

  m_filterGroup = currencyInfo->filterGroup;
  m_category = static_cast< Common::ItemUICategory >( currencyInfo->itemUICategory );
  m_maxAmount = currencyInfo->stackSize;
}

Sapphire::Common::CurrencyCrystalType Sapphire::CurrencyCrystal::getId() const
{
  return m_id;
}

uint8_t Sapphire::CurrencyCrystal::getUId() const
{
  return m_uId;
}

void Sapphire::CurrencyCrystal::setAmount( uint32_t size )
{
  m_amount = std::min< uint32_t >(size, m_maxAmount);
}

uint32_t Sapphire::CurrencyCrystal::getAmount() const
{
  return m_amount;
}

uint32_t Sapphire::CurrencyCrystal::getMaxAmount() const
{
  return m_maxAmount;
}
