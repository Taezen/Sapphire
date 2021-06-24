#ifndef _CORE_NETWORK_PACKETS_IPCS_H
#define _CORE_NETWORK_PACKETS_IPCS_H

#include <stdint.h>

namespace Sapphire::Network::Packets
{

  ////////////////////////////////////////////////////////////////////////////////
  /// Lobby Connection IPC Codes
  /**
  * Server IPC Lobby Type Codes.
  */
  enum ServerLobbyIpcType : uint16_t
  {
    LobbyError = 0x0002,
    LobbyServiceAccountList = 0x000C,
    LobbyCharList = 0x000D,
    LobbyCharCreate = 0x000E,
    LobbyEnterWorld = 0x000F,
    LobbyServerList = 0x0015,
    LobbyRetainerList = 0x0017,

  };

  /**
  * Client IPC Lobby Type Codes.
  */
  enum ClientLobbyIpcType : uint16_t
  {
    ReqCharList = 0x0003,
    ReqEnterWorld = 0x0004,
    ClientVersionInfo = 0x0005,

    ReqCharDelete = 0x000A,
    ReqCharCreate = 0x000B,
  };

  ////////////////////////////////////////////////////////////////////////////////
  /// Zone Connection IPC Codes
  /**
  * Server IPC Zone Type Codes.
  */
  enum ServerZoneIpcType : uint16_t
  {
    Ping = 0x0115, // updated 5.57
    Init = 0x01E9, // updated 5.57

    ActorFreeSpawn = 0x0235, // updated 5.57
    InitZone = 0x01E5, // updated 5.57

    EffectResult = 0x03CF, // updated 5.57
    ActorControl = 0x0164, // updated 5.57
    ActorControlSelf = 0x0356, // updated 5.57
    ActorControlTarget = 0x0281, // updated 5.57

    /*!
     * @brief Used when resting
     */
    UpdateHpMpTp = 0x03BE, // updated 5.57

    ///////////////////////////////////////////////////

    ChatBanned = 0xF06B,
    Playtime = 0x0134, // updated 5.57
    Logout = 0x0360, // updated 5.57
    CFNotify = 0x0172, // updated 5.57
    CFMemberStatus = 0x0079,
    CFDutyInfo = 0x0145, // updated 5.45 hotfix
    CFPlayerInNeed = 0xF07F,
    CFPreferredRole = 0x0108, // updated 5.57
    CFCancel = 0x02A3, // updated 5.57
    SocialRequestError = 0xF0AD,

    CFRegistered = 0x00BA, // updated 5.57
    SocialRequestResponse = 0x03A5, // updated 5.45 hotfix
    SocialMessage = 0x030C, // updated 5.45 hotfix
    SocialMessage2 = 0x00BB, // updated 5.45 hotfix
    CancelAllianceForming = 0x00C6, // updated 4.2

    LogMessage = 0x00D0,

    Chat = 0x0372, // updated 5.57
    PartyChat = 0x0065,
    CWLSChat = 0x0345, // updated 5.57

    WorldVisitList = 0xF0FE, // added 4.5

    SocialList = 0x01BA, // updated 5.57 hotfix

    ExamineSearchInfo = 0x031B, // updated 5.57
    UpdateSearchInfo = 0x0299, // updated 5.57
    InitSearchInfo = 0x00C9, // updated 5.57
    ExamineSearchComment = 0x030F, // updated 5.57

    ServerNoticeShort = 0x03B4, // updated 5.45 hotfix
    ServerNotice = 0x02F7, // updated 5.57
    SetOnlineStatus = 0x0252, // updated 5.57

    CountdownInitiate = 0x0220, // updated 5.57
    CountdownCancel = 0x018C, // updated 5.57

    PlayerAddedToBlacklist = 0x02F2, // updated 5.57
    PlayerRemovedFromBlacklist = 0x00A8, // updated 5.57
    BlackList = 0x0085, // updated 5.57

    LinkshellList = 0x022B, // updated 5.57
    CWLinkshellList = 0x00D5, // updated 5.55 hotfix
    FellowshipList = 0x0142, // updated 5.55 hotfix

    MailDeleteRequest = 0xF12B, // updated 5.0

    // 12D - 137 - constant gap between 4.5x -> 5.0
    ReqMoogleMailList = 0xF138, // updated 5.0
    ReqMoogleMailLetter = 0xF139, // updated 5.0
    MailLetterNotification = 0x013A, // updated 5.0

    MarketTaxRates = 0x01F8, // updated 5.35 hotfix

    MarketBoardSearchResult = 0x02C4, // updated 5.57
    MarketBoardItemListingCount = 0x0075, // updated 5.57
    MarketBoardItemListingHistory = 0x03C5, // updated 5.57
    MarketBoardItemListing = 0x00A7, // updated 5.57
    MarketBoardPurchase = 0x014C, // updated 5.57
    
    CharaFreeCompanyTag = 0x013B, // updated 4.5
    FreeCompanyBoardMsg = 0x02DC, // updated 5.57
    FreeCompanyInfo = 0x01A9, // updated 5.57
    FreeCompanyShortInfo = 0x0226, // updated 5.57
    ExamineFreeCompanyInfo = 0x0313, // updated 5.57
    FreeCompanyRanks = 0x02D1, // updated 5.57

    FreeCompanyUpdateShortMessage = 0xF157, // added 5.0
    FreeCompanyDialog = 0x039B, // updated 5.57

    StatusEffectList = 0x0192, // updated 5.57
    EurekaStatusEffectList = 0x0167, // updated 5.18
    BossStatusEffectList = 0x0312, // added 5.1
    Effect = 0x008F, // updated 5.57
    AoeEffect8 = 0x0247, // updated 5.57
    AoeEffect16 = 0x02C1, // updated 5.57
    AoeEffect24 = 0x0295, // updated 5.57
    AoeEffect32 = 0x034C, // updated 5.57
    PersistantEffect = 0x0244, // updated 5.45 hotfix

    GCAffiliation = 0x012E, // updated 5.57

    PlayerSpawn = 0x018D, // updated 5.57
    NpcSpawn = 0x01CE, // updated 5.57
    NpcSpawn2 = 0x01CB, // ( Bigger statuseffectlist? ) updated 5.3
    ActorMove = 0x0233, // updated 5.57

    ActorSetPos = 0x01A3, // updated 5.57

    ActorCast = 0x02CC, // updated 5.57
    SomeCustomiseChangePacketProbably = 0x00CD, // added 5.18

    PartyList = 0x028E, // updated 5.57
    PartyMessage = 0x0136, // updated 5.57
    HateRank = 0x0217, // updated 5.45 hotfix
    BossHateList = 0x01E1, // updated 5.57
    HateList = 0x0184, // updated 5.57
    ObjectSpawn = 0x0254, // updated 5.57
    ObjectDespawn = 0x0306, // updated 5.47
    UpdateClassInfo = 0x00DB, // updated 5.57
    SilentSetClassJob = 0xF18E, // updated 5.0 - seems to be the case, not sure if it's actually used for anything
    PlayerSetup = 0x01E4, // updated 5.57
    PlayerStats = 0x00E8, // updated 5.57
    ActorOwner = 0x01E7, // updated 5.57
    PlayerStateFlags = 0x039C, // updated 5.57
    PlayerClassInfo = 0x0066, // updated 5.57
    PlayerStatusEffectList = 0x02A6, // updated 5.57
    CharaVisualEffect = 0x026A, // updated 5.45 hotfix

    ModelEquip = 0x01DF, // updated 5.57
    Examine = 0x02B4, // updated 5.57
    CharaNameReq = 0x0380, // updated 5.45 hotfix

    // nb: see #565 on github
    UpdateRetainerItemSalePrice = 0xF19F, // updated 5.0
    RetainerSaleHistory = 0x020E, // updated 5.21 hotfix
    RetainerInformation = 0x024B, // updated 5.57

    SetLevelSync = 0x1186, // not updated for 4.4, not sure what it is anymore

    ItemInfo = 0x023E, // updated 5.57
    ContainerInfo = 0x0362, // updated 5.57
    InventoryTransactionFinish = 0x0243, // updated 5.57
    InventoryTransaction = 0x0241, // updated 5.57
    CurrencyCrystalInfo = 0x03D1, // updated 5.57

    LeveActiveList = 0x024D, // updated 5.57
    LeveCompleteList = 0x024D, // updated 5.57
    LeveAllowanceInfo = 0x0120, // updated 5.57

    InventoryActionAck = 0x02AD, // updated 5.57
    UpdateInventorySlot = 0x01F5, // updated 5.57

    HuntingLogEntry = 0x007B, // updated 5.57

    EventPlay = 0x036A, // updated 5.57
    EventPlay4 = 0x0397, // updated 5.57
    EventPlay8 = 0x00A6, // updated 5.57
    EventPlay16 = 0x023D, // updated 5.57
    EventPlay32 = 0x0204, // updated 5.57
    EventPlay64 = 0x00EF, // updated 5.57
    EventPlay128 = 0x029C, // updated 5.57
    EventPlay255 = 0x021B, // updated 5.57

    EventYield = 0x0268, // updated 5.45 hotfix
    //EventYield4 = 0x0000,
    //EventYield8 = 0x0000,
    //EventYield16 = 0x0000,
    //EventYield32 = 0x0000,
    //EventYield64 = 0x0000,
    //EventYield128 = 0x0000,
    //EventYield255 = 0x0000,

    EventStart = 0x00FB, // updated 5.57
    EventFinish = 0x02CB, // updated 5.57

    EventLinkshell = 0x1169,

    QuestActiveList = 0x0128, // updated 5.57
    QuestUpdate = 0x0389, // updated 5.57
    QuestCompleteList = 0x0103, // updated 5.57

    QuestFinish = 0x0214, // updated 5.57
    MSQTrackerComplete = 0x03C6, // updated 5.57
    MSQTrackerProgress = 0xF1CD, // updated 4.5 ? this actually looks like the two opcodes have been combined, see #474

    QuestMessage = 0x0399, // updated 5.55 hotfix

    QuestTracker = 0x016A, // updated 5.57

    GathererGuide = 0x009F, // updated 5.57
    GathererGuideViewedMask = 0x02F5, // updated 5.57
    CrafterGuide = 0x0222, // updated 5.57
    CrafterGuideViewedMask = 0x0327, // updated 5.57

    Mount = 0x009C, // updated 5.57

    DirectorVars = 0x00DA, // updated 5.57
    DirectorMessage = 0x0318, // updated 5.57 - display on screen messages, for example, when you obtain something
    DirectorMessage4 = 0x01FE, // updated 5.57
    DirectorMessage8 = 0x01C1, // updated 5.57
    DirectorMessage16 = 0x00DE, // updated 5.57
    DirectorMessage32 = 0x00F3, // updated 5.57
    DirectorPopUp = 0x0394, // updated 5.57 - display dialogue pop-ups in duties and FATEs, for example, Teraflare's countdown
    DirectorPopUp4 = 0x0319, // updated 5.57
    DirectorPopUp8 = 0x0209, // updated 5.57

    CFAvailableContents = 0xF1FD, // updated 4.2

    WeatherChange = 0x0157, // updated 5.57
    PlayerTitleList = 0x00B8, // updated 5.57
    Discovery = 0x0247, // updated 5.45 hotfix

    EorzeaTimeOffset = 0x00C5, // updated 5.45 hotfix

    EquipDisplayFlags = 0x0212, // updated 5.57

    MiniCactpotInit = 0x0286, // added 5.31
    ShopMessage = 0x02FC, // updated 5.45 hotfix
    LootMessage = 0x01BF, // updated 5.57
    ResultDialog = 0x01AB, // updated 5.57
    DesynthStats = 0x0304, // updated 5.57
    DesynthResult = 0x0218, // updated 5.57

    /// Housing //////////////////////////////////////

    LandSetInitialize = 0x00FC, // updated 5.57
    LandUpdate = 0x027E, // updated 5.45 hotfix
    YardObjectSpawn = 0x010E, // updated 5.45 hotfix
    HousingIndoorInitialize = 0x0357, // updated 5.57
    LandPriceUpdate = 0x0379, // updated 5.45 hotfix
    LandInfoSign = 0x0398, // updated 5.45 hotfix
    LandRename = 0x00F6, // updated 5.45 hotfix
    HousingEstateGreeting = 0x037D, // updated 5.45 hotfix
    HousingUpdateLandFlagsSlot = 0x03AF, // updated 5.45 hotfix
    HousingLandFlags = 0x00D4, // updated 5.57
    HousingShowEstateGuestAccess = 0x01B3, // updated 5.45 hotfix

    HousingObjectInitialize = 0x0365, // updated 5.57
    HousingInternalObjectSpawn = 0x00BC, // updated 5.45 hotfix

    HousingWardInfo = 0x0146, // updated 5.55 hotfix
    HousingObjectMove = 0x03E6, // updated 5.45 hotfix

    SharedEstateSettingsResponse = 0x03A4, // updated 5.45 hotfix

    LandUpdateHouseName = 0x0215, // updated 5.45 hotfix

    LandSetMap = 0x0311, // updated 5.57

    //////////////////////////////////////////////////

    DuelChallenge = 0x0277, // 4.2; this is responsible for opening the ui
    PerformNote = 0x0248, // updated 5.45 hotfix

    PrepareZoning = 0x0250, // updated 5.57
    ActorGauge = 0x0377, // updated 5.57
    DutyGauge = 0x00D2, // updated 5.45 hotfix

    // daily quest info -> without them sent,  login will take longer...
    DailyQuests = 0x006A, // updated 5.57
    DailyQuestRepeatFlags = 0x008D, // updated 5.57

    DailyQuestFinish = 0x015B, // updatetd 5.57

    /// Doman Mahjong //////////////////////////////////////
    MahjongOpenGui = 0x02A4, // only available in mahjong instance
    MahjongNextRound = 0x02BD, // initial hands(baipai), # of riichi(wat), winds, honba, score and stuff
    MahjongPlayerAction = 0x02BE, // tsumo(as in drawing a tile) called chi/pon/kan/riichi
    MahjongEndRoundTsumo = 0x02BF, // called tsumo
    MahjongEndRoundRon = 0x02C0, // called ron or double ron (waiting for action must be flagged from discard packet to call)
    MahjongTileDiscard = 0x02C1, // giri (discarding a tile.) chi(1)/pon(2)/kan(4)/ron(8) flags etc..
    MahjongPlayersInfo = 0x02C2, // actor id, name, rating and stuff..
    // 2C3 and 2C4 are currently unknown
    MahjongEndRoundDraw = 0x02C5, // self explanatory
    MahjongEndGame = 0x02C6, // finished oorasu(all-last) round; shows a result screen.

    /// Airship & Submarine //////////////////////////////////////
    AirshipExplorationResult = 0x0099, // updated 5.57
    AirshipStatus = 0x01DC, // updated 5.57
    AirshipStatusList = 0x0088, // updated 5.57
    AirshipTimers = 0x013A, // updated 5.57
    SubmarineExplorationResult = 0x027A, // updated 5.57
    SubmarineProgressionStatus = 0x019C, // updated 5.57
    SubmarineStatusList = 0x0345, // updated 5.57
    SubmarineTimers = 0x01AA, // updated 5.57

    PlaceFieldMarkerPreset = 0x02C5, // updated 5.57
    PlaceFieldMarker = 0x0236, // updated 5.57
  };

  /**
  * Client IPC Zone Type Codes.
  */
  enum ClientZoneIpcType : uint16_t
  {
    PingHandler = 0x0202, // updated 5.57
    InitHandler = 0x03B2, // updated 5.57

    FinishLoadingHandler = 0x018B, // updated 5.57

    CFCommenceHandler = 0x0118, // updated 5.35 hotfix

    CFCancelHandler = 0x02F0, // updated 5.57
    CFRegisterDuty = 0x029E, // updated 5.57
    CFRegisterRoulette = 0x015D, // updated 5.57
    PlayTimeHandler = 0x0390, // updated 5.57
    LogoutHandler = 0x02C6, // updated 5.57
    CancelLogout = 0x02A3, // updated 5.57

    CFDutyInfoHandler = 0x0078, // updated 4.2

    SocialReqSendHandler = 0x0288, // updated 5.45 hotfix
    SocialResponseHandler = 0x029B, // updated 5.45 hotfix
    CreateCrossWorldLS = 0x01CB, // updated 5.57

    ChatHandler = 0x00E1, // updated 5.57
    PartyChatHandler = 0x0065,
    PartySetLeaderHandler = 0x02EF, // updated 5.45 hotfix
    LeavePartyHandler = 0x017E, // updated 5.45 hotfix
    KickPartyMemberHandler = 0x0070, // updated 5.45 hotfix
    DisbandPartyHandler = 0x037A, // updated 5.45 hotfix

    SocialListHandler = 0x03D6, // updated 5.57
    SetSearchInfoHandler = 0x01D3, // updated 5.57
    ReqSearchInfoHandler = 0x03CD, // updated 5.57
    ReqExamineSearchCommentHandler = 0x00E7, // updated 5.0

    ReqAddPlayerToBlacklist = 0x0133, //updated 5.57
    ReqRemovePlayerFromBlacklist = 0x0274, // updated 5.57
    BlackListHandler = 0x03C7, // updated 5.57
    PlayerSearchHandler = 0x0174, // updated 5.57

    LinkshellListHandler = 0x00F6, // updated 5.57

    MarketBoardRequestItemListingInfo = 0x025B, // updated 5.57
    MarketBoardRequestItemListings = 0x0103, // updated 4.5
    MarketBoardSearch = 0x00AA, // updated 5.57
    MarketBoardPurchaseHandler = 0x01F3, // updated 5.57

    ReqExamineFcInfo = 0x03A9, // updated 5.55 hotfix

    FcInfoReqHandler = 0x01F1, // updated 5.57

    FreeCompanyUpdateShortMessageHandler = 0x0123, // added 5.0

    ReqMarketWishList = 0x02C0, // updated 5.55 hotfix

    ReqJoinNoviceNetwork = 0x0129, // updated 4.2

    ReqCountdownInitiate = 0x01EC, // updated 5.57
    ReqCountdownCancel = 0x0277, // updated 5.55 hotfix

    ZoneLineHandler = 0x039B, // updated 5.57
    ClientTrigger = 0x02DC, // updated 5.57
    DiscoveryHandler = 0x02D1, // updated 5.57

    PlaceFieldMarkerPresetHandler = 0x01DF, // updated 5.57
    PlaceFieldMarkerHandler = 0x0392, // updated 5.45 hotfix
    SkillHandler = 0x03A0, // updated 5.57
    GMCommand1 = 0x0285, // updated 5.57
    GMCommand2 = 0x02DB, // updated 5.57
    AoESkillHandler = 0x036B, // updated 5.57

    UpdatePositionHandler = 0x0077, // updated 5.57

    InventoryModifyHandler = 0x00C0, // updated 5.57 (Base offset: 0x00C7)

    InventoryEquipRecommendedItems = 0x0266, // updated 5.57

    ReqPlaceHousingItem = 0x0360, // updated 5.45 hotfix
    BuildPresetHandler = 0x01D7, // updated 5.45 hotfix

    TalkEventHandler = 0x03CF, // updated 5.57
    EmoteEventHandler = 0x03D5, // updated 5.45 hotfix
    WithinRangeEventHandler = 0x0171, // updated 5.45 hotfix
    OutOfRangeEventHandler = 0x019B, // updated 5.45 hotfix
    EnterTeriEventHandler = 0x03BE, // updated 5.57
    ShopEventHandler = 0x03D4, // updated 5.45 hotfix
    EventYieldHandler = 0x0230, // updated 5.45 hotfix
    ReturnEventHandler = 0x02C1, // updated 5.57
    TradeReturnEventHandler = 0x00A7, // updated 5.45 hotfix
    TradeMultipleReturnEventHander = 0x035C, // updated 5.35 hotfix

    LinkshellEventHandler = 0x016B, // updated 4.5
    LinkshellEventHandler1 = 0x016C, // updated 4.5

    ReqEquipDisplayFlagsChange = 0x0354, // updated 5.57

    LandRenameHandler = 0x0187, // updated 5.45 hotfix
    HousingUpdateHouseGreeting = 0x0367, // updated 5.45 hotfix
    HousingUpdateObjectPosition = 0x0265, // updated 5.45 hotfix
    HousingEditExterior = 0x0297, // updated 5.45 hotfix

    SetSharedEstateSettings = 0x0146, // updated 5.45 hotfix

    UpdatePositionInstance = 0x0233, // updated 5.57

    PerformNoteHandler = 0x0336, // updated 5.45 hotfix

    WorldInteractionHandler = 0x027D, // updated 5.57
    Dive = 0x01E5, // updated 5.57
  };

  ////////////////////////////////////////////////////////////////////////////////
  /// Chat Connection IPC Codes
  /**
  * Server IPC Chat Type Codes.
  */
  enum ServerChatIpcType : uint16_t
  {
    Tell = 0x0064, // updated for sb
    PublicContentTell = 0x00FB, // added 4.5, this is used when receiving a /tell in PublicContent instances such as Eureka or Bozja
    TellErrNotFound = 0x0066,

    FreeCompanyEvent = 0x012C, // added 5.0
  };

  /**
  * Client IPC Chat Type Codes.
  */
  enum ClientChatIpcType : uint16_t
  {
    TellReq = 0x0064,
    PublicContentTellReq = 0x0326, // updated 5.35 hotfix, this is used when sending a /tell in PublicContent instances such as Eureka or Bozja
  };


}

#endif /*_CORE_NETWORK_PACKETS_IPCS_H*/
