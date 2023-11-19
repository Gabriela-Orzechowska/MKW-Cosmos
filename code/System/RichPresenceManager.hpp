#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>

class RichPresenceManager
{
    public:
    static RichPresenceManager * sInstance;
    
    static RichPresenceManager * GetStaticInstance();
    static RichPresenceManager * CreateStaticInstance();


    static char * applicationId;
    static IOS::Dolphin::DiscordRichPresence presence;

    bool rEnabled;

    s32 InitConnection();
    void Init();
    void CloseConnection();

    s32 UpdateStatus();
};