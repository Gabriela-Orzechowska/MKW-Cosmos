#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>

class RichPresenceManager
{
    public:
    static RichPresenceManager * sInstance;
    
    static RichPresenceManager * GetStaticInstance();
    static void CreateStaticInstance();
    static s32 InitConnection();

    static IOS::Dolphin::DiscordRichPresence presence;

    void Init();
    void CloseConnection();
    s32 UpdateStatus();

    private:
    static char * applicationId;
    bool rEnabled;
};