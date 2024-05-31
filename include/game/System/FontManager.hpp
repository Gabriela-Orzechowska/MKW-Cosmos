#ifndef _FONTMANAGER_
#define _FONTMANAGER_

#include <core/nw4r/ut/Font.hpp>

struct FontInformation{
    nw4r::ut::Font::Type type;
    char lineFeed;
    u16 alterCharIndex;
    nw4r::ut::CharWidths widths;
    nw4r::ut::FontEncoding Encoding;
    void * pGlyths;
    void * pWidth;
    void * pMap;
    u8 height;
    u8 width;
    u8 ascent;
    u8 pad;
};

class ResFontBase{
    public:
    ResFontBase();  //800b1f90
    virtual ~ResFontBase(); //800b2870 vtable //80274ed0
    virtual void func1();
    void Rebuild(); //800b2a20
    bool SetResource(void * resource); //800b28d0
    void Init(); //805d1214
    u32 pmtf[3];
    void * resource;
    FontInformation * fontInformation;
    u16 lastCharCode;
    u16 lastGlyphIndex;
};

class ResFont : public ResFontBase {
public:
    ResFont(); //800b2830
    virtual ~ResFont(); 
    void func1() override;
    u32 unknown1;
    u32 unknown2;
};

extern "C" u32 FontManagerFont__vt;

class FontManagerFont {
public:
    FontManagerFont(); //800b2830
    ~FontManagerFont(); //808b87c0

    void Rebuild(); //800b2a20
    bool SetResource(void * resource); //800b28d0
    void Init(); //805d1214

    

    void* vTable;
    u32 pmtf[3];
    void * resource;
    FontInformation * fontInformation;
    u16 lastCharCode;
    u16 lastGlyphIndex;
    u32 unknown1;
    u32 unknown2;
};

class FontManager
{
    public:
    FontManager(); //805d130c
    ~FontManager(); //805d132c

    static FontManager * sInstance; //809c1bf8
    static FontManager* GetStaticInstance() { return sInstance; }

    static void CreateStaticInstance(); //805d13c4
    static void DestroyStaticInstance(); //805d1414
    void Init(); //805d14a4
    void LoadFont(int index);
    FontManagerFont * GetFontByName(const char * name); //805d1804
    FontManagerFont * fonts[6];
};


#endif