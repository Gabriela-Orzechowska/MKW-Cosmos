#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>

class RichPresenceManager
{
    public:
    static RichPresenceManager * sInstance;
    
    static RichPresenceManager * GetStaticInstance() { return sInstance; }
    static void CreateStaticInstance();
    static s32 InitConnection();

    static IOS::Dolphin::DiscordRichPresence presence;

    void Init();
    void CloseConnection();
    s32 UpdateStatus();

    private:
    bool rEnabled;
};
