#ifndef _LECODE_MANAGER_
#define _LECODE_MANAGER_

#include <kamek.hpp>
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/rvl_sdk.hpp>
#include <core/rvl/os/OS.hpp>

#define LECODE_FILE "/rel/lecode-%s.bin"

namespace LeCode{

    typedef struct le_binary_header {
        u32 magic;
        u32 version;
        u32 buildNumber;
        void * baseAddress;
        void (*entryPoint)();;
        u32 totalSize;
        u32 offsetParam;
        char region;
        char flag;
        char lecodeVersion;
    } le_binary_header;

    typedef struct lpar_header {
        u32 magic; //0x00
        u32 padding_; //0x04
        u32 version; //0x08
        u32 size; //0x0C
        u32 off_eod; //0x10
        u32 off_cup_parameters;	// offset to cup param 0x14
        u32 off_track_layout;	// offset of cup-track list
        u32 off_battle_layout;	// offset of cup-arena list
        u32 off_course_parameters;	// offset of course param
        u32 off_property;	// offset of property list
        u32 off_music;	// offset of music list
        u32 off_flags;	// offset of flags
        u8 engine[3];
    } lpar_header;

    typedef struct cup2_header {
        u32 header;
        u32 course_cups;
        u32 battle_cups;
        u32 padding;
    } cup2_header;

    class LeCodeManager{
    private:
        static LeCodeManager * sInstance; //0x80003FD8
        const char * GetLeCodeFilepath();
        le_binary_header * loaderHeader;
        lpar_header * settingHeader;

    public:
        static LeCodeManager *GetStaticInstance();
        u32 GetTrackID();
        void * GetLoaderAddress();
        void * GetCupParamAddress();
        void * GetCupDefAddress();
        void LoadLeCode();
    };
}



#endif //_LECODE_MANAGER_