#ifndef _CORE_NETWORK_PACKETS_INITUIPACKET_H
#define _CORE_NETWORK_PACKETS_INITUIPACKET_H

#include <Network/GamePacket.h>
#include <Network/PacketDef/Zone/ServerZoneDef.h>
#include "Actor/Player.h"
#include "Forwards.h"

namespace Sapphire::Network::Packets::Server
{

  /**
  * @brief The Client Player Initialization packet. This must be sent to the client
  * once upon connection to configure the UI.
  */
  class PlayerSetupPacket : public ZoneChannelPacket< FFXIVIpcPlayerSetup >
  {
  public:
    PlayerSetupPacket( Entity::Player& player ) :
      ZoneChannelPacket< FFXIVIpcPlayerSetup >( player.getId(), player.getId() )
    {
      initialize( player );
    };

  private:
    void initialize( Entity::Player& player )
    {
      m_data.contentId = player.getContentId();

      // TODO: Support rested experience.
      m_data.restedExp = 0;
      //m_data.padding = 0x100;
      m_data.charId = player.getId();
      m_data.race = player.getLookAt( Common::CharaLook::Race );
      m_data.tribe = player.getLookAt( Common::CharaLook::Tribe );
      m_data.gender = player.getLookAt( Common::CharaLook::Gender );
      m_data.currentClass = static_cast< uint8_t >( player.getClass() );
      m_data.currentJob = static_cast< uint8_t >( player.getClass() );
      m_data.maxLevel = player.getLevel();
      m_data.deity = static_cast< uint8_t >( player.getGuardianDeity() );
      m_data.namedayMonth = player.getBirthMonth();
      m_data.namedayDay = player.getBirthDay();
      // TODO: Support grand company status.
      m_data.cityState = player.getStartTown();
      //m_data.gcRank = GCRank::None;

      m_data.homepoint = player.getHomepoint();
      m_data.pose = player.getPose();

      memset( &m_data.name[ 0 ], 0, sizeof( m_data.name ) );
      strcpy( &m_data.name[ 0 ], player.getName().c_str() );

      memcpy( m_data.aetheryte, player.getAetheryteArray(), sizeof( m_data.aetheryte ) );

      // Set the class levels and exp.
      for ( uint8_t i = 0; i < 30; i++ )
      {
        m_data.levels[ i ] = player.getClassArray()[ i ];
        m_data.exp[ i ] = player.getExpArray()[ i ];
      }

      memcpy( m_data.orchestrionMask, player.getOrchestrionBitmask(), sizeof( m_data.orchestrionMask ) );

      memset( m_data.minions, 0xFF, sizeof( m_data.minions ) );

      memcpy( m_data.mountGuideMask, player.getMountGuideBitmask(), sizeof( m_data.mountGuideMask ) );

      memcpy( m_data.unlockBitmask, player.getUnlockBitmask(), sizeof( m_data.unlockBitmask ) );

      memcpy( m_data.discovery2, player.getDiscoveryBitmask(), sizeof( m_data.discovery2 ) );
      memcpy( m_data.discovery4, player.getDiscoveryBitmask() + sizeof( m_data.discovery2 ), sizeof( m_data.discovery4 ) );

      memcpy( m_data.howto, player.getHowToArray(), sizeof( m_data.howto ) );

      // possibly max level or current level
      m_data.maxLevel = Common::MAX_PLAYER_LEVEL;
      m_data.expansion = Common::CURRENT_EXPANSION_ID;

      // df stuff
      // todo: actually do this properly
  //      m_data.unknown70[4] = 1; // enable df

      // enable all raids/guildhests/dungeons
      m_data.unlockedExtraCFContent = 3;
      memset( m_data.unlockedDungeons, 0xFF, sizeof( m_data.unlockedDungeons ) );
      memset( m_data.unlockedGuildhests, 0xFF, sizeof( m_data.unlockedGuildhests ) );
      m_data.unlockedPvp[0] = 1; //This is enough to enable all pvp content;
      m_data.unlockedPvp2[0] = 1;
      memset( m_data.unlockedRaids, 0xFF, sizeof( m_data.unlockedRaids ) );
      memset( m_data.unlockedTrials, 0xFF, sizeof( m_data.unlockedTrials ) );
    };
  };

}

#endif /*_CORE_NETWORK_PACKETS_CINITUIPACKET_H*/
