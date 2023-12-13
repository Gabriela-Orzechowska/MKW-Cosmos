#include <System/RichPresenceManager.hpp>
#include <core/System/RKSystem.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <LeCode/LeCodeManager.hpp>
#include <UI/BMG/MessageGroup.hpp>
#include <System/Sha1.hpp>

using namespace IOS::Dolphin;

RichPresenceManager * RichPresenceManager::sInstance = nullptr;

IOS::Dolphin::DiscordRichPresence RichPresenceManager::presence = {
    .state = "",
    .details = "In Main Menu",
    .startTimestamp = 0,
    .endTimestamp = 0,
    .largeImageKey = "icon1",
    .largeImageText = "Mario Kart Wii Deluxe",
    .smallImageKey = "",
    .smallImageText = "MKW Deluxe",
    .partySize = 0,
    .partyMax = 0,
};

char * RichPresenceManager::applicationId = "1175707208042954773";

RichPresenceManager * RichPresenceManager::CreateStaticInstance()
{
    if(sInstance == NULL)
        sInstance = new(RKSystem::mInstance.EGGSystem) RichPresenceManager;
    sInstance->rEnabled = false;
    return sInstance;
}

RichPresenceManager * RichPresenceManager::GetStaticInstance()
{
    if(sInstance == NULL)
        return CreateStaticInstance();
    return sInstance;
}

s32 RichPresenceManager::InitConnection()
{
    if(sInstance == NULL)
        return -1;
    if(!IOS::Dolphin::Open() < 0)
        return -2;

    return IOS::Dolphin::DiscordSetClient(applicationId);
}

void RichPresenceManager::Init()
{
    if(sInstance == NULL)
        CreateStaticInstance();
    s32 ret = InitConnection();
    if(ret < 0) return;
    rEnabled = true;
    DXLog("Rich Presence Enabled\n");

    u64 time = IOS::Dolphin::GetSystemTime();
    presence.startTimestamp = time / 1000;

    ret = UpdateStatus();
    if(ret < 0)
    {
        DXLog("Rich Presence Update Failed\n");
    }
}

void RichPresenceManager::CloseConnection()
{
    if(IOS::Dolphin::Open())
        IOS::Dolphin::DiscordReset();
}

s32 RichPresenceManager::UpdateStatus()
{
    if(!rEnabled) return -1;

    return IOS::Dolphin::DiscordSetPresence(&this->presence);
}

u32 UpdateTrackImage(u32 param_1)
{
    
    static char finalLink[0x80];
    ArchiveFile * file = &ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COURSE]->archives[0];
    void * buffer = file->decompressedArchive;
    u32 fileSize = file->decompressedarchiveSize;
    char * trackSha = SHA1::GetFileSha1(buffer,fileSize);

    snprintf(finalLink, 0x80, "https://ct.wiimm.de/api/get-start-image?sha1=%s", trackSha);
    DXLog("Setting image to:%s\n",finalLink);
    RichPresenceManager * manager = RichPresenceManager::sInstance;
    if(manager != nullptr)
    {
        manager->presence.largeImageKey = finalLink;
        manager->presence.smallImageKey = "icon1";
    }

    return param_1;
}

kmBranch(0x80540914, UpdateTrackImage);

void RPCSectionChange()
{
    MenuId menuId = MenuData::sInstance->curScene->menuId;
    RichPresenceManager * manager = RichPresenceManager::sInstance;
    if(manager == nullptr) return;
    DiscordRichPresence * presence = &manager->presence;
    char * message = presence->details;
    char * status = "";


    u32 trackId = LeCode::LeCodeManager::GetStaticInstance()->GetTrackID();

    if(menuId < GRAND_PRIX_PANORAMA || menuId > GHOST_RACE_GAMEPLAY_2)
    {
        if(menuId < P1_WIFI_VS_GAMEPLAY || menuId > P2_WIFI_FRIEND_COIN_BT_GAMEPLAY)
        {
            manager->presence.largeImageKey = "icon1";
            manager->presence.smallImageKey = "";
        }
    }

    switch(menuId){
        case CREATE_NEW_SAVE:
            message = "Creating new license";
            break;
        case GRAND_PRIX_PANORAMA:
            message = "Starting Grand Prix";
            break;
        case VS_RACE_PANORAMA:
            message = "Starting VS Race";
            break;
        case BALLOON_BATTLE_PANORAMA:
            message = "Starting Battle Match";
            break;
        case GRAND_PRIX_GAMEPLAY:
            message = "Playing a Grand Prix";
            break;
        case TIME_TRIAL_GAMEPLAY:
        case GHOST_RACE_GAMEPLAY_1:
        case GHOST_RACE_GAMEPLAY_2:
            message = "Playing a Time Trial";
            break;
        case P1_VS_RACE_GAMEPLAY:
        case P2_VS_RACE_GAMEPLAY:
        case P3_VS_RACE_GAMEPLAY:
        case P4_VS_RACE_GAMEPLAY:
            message = "Playing a VS Race";
            break;
        case P1_TEAM_VS_RACE_GAMEPLAY:
        case P2_TEAM_VS_RACE_GAMEPLAY:
        case P3_TEAM_VS_RACE_GAMEPLAY:
        case P4_TEAM_VS_RACE_GAMEPLAY:
            message = "Playing a Team VS Race";
            break;
        case P1_BATTLE_GAMEPLAY:
        case P2_BATTLE_GAMEPLAY:
        case P3_BATTLE_GAMEPLAY:
        case P4_BATTLE_GAMEPLAY:
            message = "Playing a Battle Match";
            break;
        case GRAND_PRIX_REPLAY:
            message = "Watching a Grand Prix Replay";
            break;
        case TIME_TRIAL_REPLAY:
            message = "Watching a Time Trial Replay";
            break;
        case WATCH_GHOST_FROM_CHANNEL:
        case WATCH_GHOST_FROM_DOWNLOADS:
        case WATCH_GHOST_FROM_MENU:
            message = "Watching a Ghost Replay";
            break;
        case GRAND_PRIX_AWARD:
        case VS_RACE_AWARD:
        case AWARD_37:
        case AWARD_38:
            message = "Watching an Award Ceremony";
            break;
        case MAIN_MENU_FROM_BOOT:
        case MAIN_MENU_FROM_RESET:
        case MAIN_MENU_FROM_MENU:
        case MAIN_MENU_FROM_NEW_LICENSE:
        case MAIN_MENU_FROM_LICENSE_43:
            message = "In Main Menu";
            break;
        case LICENSE_SETTINGS_MENU:
        case MII_SELECT_1:
        case MII_SELECT_2:
            message = "In License Settings";
            break;
        case SINGLE_PLAYER_TT_CHANGE_CHARA:
        case SINGLE_PLAYER_FROM_MENU:
        case SINGLE_PLAYER_TT_CHANGE_COURSE:
        case SINGLE_PLAYER_VS_NEXT_RACE:
        case SINGLE_PLAYER_BT_NEXT_BATTLE:
        case SINGLE_PLAYER_CHAN_RACE_GHOST:
        case SINGLE_PLAYER_LB_RACE_GHOST:
        case SINGLE_PLAYER_LIST_RACE_GHOST:
            message = "In Singleplayer Menu";
            break;
        case LOCAL_MULTIPLAYER:
            message = "In Local Multiplayer Menu";
            break;
        case P1_WIFI:
        case P1_WIFI_GLOBE_DISCONNECT:
        case P2_WIFI:
        case P2_WIFI_GLOBE_DISCONNECT:
            message = "Connected to Wiimmfi";
            break;
        case P1_WIFI_VS_VOTING:
        case P2_WIFI_VS_VOTING:
        case P1_WIFI_BATTLE_VOTING:
        case P2_WIFI_BATTLE_VOTING:
            message = "Voting for a track in CTWW";
            break;
        case P1_WIFI_FRIEND_ROOM_VS_VOTING:
        case P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING:
        case P1_WIFI_FRIEND_ROOM_BALLOON_VOTING:
        case P1_WIFI_FRIEND_ROOM_COIN_VOTING:
        case P2_WIFI_FRIEND_ROOM_VS_VOTING:
        case P2_WIFI_FRIEND_ROOM_TEAM_VS_VOTING:
        case P2_WIFI_FRIEND_ROOM_BALLOON_VOTING:
        case P2_WIFI_FRIEND_ROOM_COIN_VOTING:
            message = "Voting for a track in Friend Room";
            break;
        case P1_WIFI_VS_GAMEPLAY:
        case P2_WIFI_VS_GAMEPLAY:
            message = "In a CTWW Race";
            break;
        case P1_WIFI_BT_GAMEPLAY:
        case P2_WIFI_BT_GAMEPLAY:
            message = "In a CTWW Battle";
            break;
        case P1_WIFI_VS_LIVE_VIEW:
        case P2_WIFI_VS_LIVE_VIEW:
            message = "Waiting for a CTWW Race";
            break;
        case P1_WIFI_BT_LIVE_VIEW:
        case P2_WIFI_BT_LIVE_VIEW:
            message = "Waiting for a CTWW Battle";
            break;
        default:
            break;
    }
    switch(menuId)
    {
        case GRAND_PRIX_PANORAMA:
        case VS_RACE_PANORAMA:
        case BALLOON_BATTLE_PANORAMA:
        case GRAND_PRIX_GAMEPLAY:
        case TIME_TRIAL_GAMEPLAY:
        case GHOST_RACE_GAMEPLAY_1:
        case GHOST_RACE_GAMEPLAY_2:
        case P1_VS_RACE_GAMEPLAY:
        case P2_VS_RACE_GAMEPLAY:
        case P3_VS_RACE_GAMEPLAY:
        case P4_VS_RACE_GAMEPLAY:
        case P1_TEAM_VS_RACE_GAMEPLAY:
        case P2_TEAM_VS_RACE_GAMEPLAY:
        case P3_TEAM_VS_RACE_GAMEPLAY:
        case P4_TEAM_VS_RACE_GAMEPLAY:
        case P1_BATTLE_GAMEPLAY:
        case P2_BATTLE_GAMEPLAY:
        case P3_BATTLE_GAMEPLAY:
        case P4_BATTLE_GAMEPLAY:
        case GRAND_PRIX_REPLAY:
        case TIME_TRIAL_REPLAY:
        case P1_WIFI_VS_GAMEPLAY:
        case P2_WIFI_VS_GAMEPLAY:
        case P1_WIFI_BT_GAMEPLAY:
        case P2_WIFI_BT_GAMEPLAY:
        case P1_WIFI_VS_LIVE_VIEW:
        case P2_WIFI_VS_LIVE_VIEW:
        case P1_WIFI_BT_LIVE_VIEW:
        case P2_WIFI_BT_LIVE_VIEW:
            status = GetTextFromMessage(trackId + 0x7000);
            //UpdateTrackImage(trackId);
            break;
        default:    
            status = "";
            break;
    }
    manager->presence.details = message;
    manager->presence.state = status;
    manager->UpdateStatus();
    return;
}


static MenuLoadHook mlhRPCSectionChange(RPCSectionChange);

void InitRPC()
{
    RichPresenceManager * instance = RichPresenceManager::GetStaticInstance();
    instance->Init();
}
static BootHook bhInitRPC(InitRPC, LOW);