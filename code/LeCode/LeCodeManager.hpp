#ifndef _LECODE_MANAGER_
#define _LECODE_MANAGER_

#include <kamek.hpp>
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/rvl_sdk.hpp>
#include <core/rvl/os/OS.hpp>
#include <main.hpp>

#define LECODE_FILE "/rel/lecode-%s.bin"

namespace LeCode{

    typedef struct le_binary_header {
        u32 magic;
        u32 version;
        u32 buildNumber;
        void * baseAddress;
        void *  entryPoint;
        u32 totalSize;
        u32 offsetParam;
        char region;
        char flag;
        char lecodeVersion;
    } le_binary_header;

    typedef struct lpar_header {
        u8 magic[8];
        u32 version;
        u32 size;
        u32 off_eod;
        u32 off_cup_par;	// offset to cup param
        u32 off_cup_track;	// offset of cup-track list
        u32 off_cup_arena;	// offset of cup-arena list
        u32 off_course_par;	// offset of course param
        u32 off_property;	// offset of property list
        u32 off_music;	// offset of music list
        u32 off_flags;	// offset of flags
        u8 engine[3];
    } lpar_header;

    class LeCodeManager{
    private:
        static LeCodeManager * sInstance; //0x80003FD8
        char * GetLeCodeFilepath();
        le_binary_header * loaderHeader;
        lpar_header * settingHeader;

    public:
        static LeCodeManager *GetStaticInstance();
        u32 GetTrackID();
        void * GetLoaderAddress();
        void LoadLeCode();
    };
}



#endif //_LECODE_MANAGER_