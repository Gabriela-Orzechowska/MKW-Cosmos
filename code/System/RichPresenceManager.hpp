#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>

class RichPresenceManager
{
    public:
    static RichPresenceManager * sInstance;
    
    static RichPresenceManager * GetStaticInstance();
    static RichPresenceManager * CreateStaticInstance();
    static s32 InitConnection();

    static char * applicationId;
    static IOS::Dolphin::DiscordRichPresence presence;

    bool rEnabled;

    void Init();
    void CloseConnection();

    s32 UpdateStatus();
};