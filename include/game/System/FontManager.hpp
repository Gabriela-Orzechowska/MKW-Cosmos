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
    ResFontBase();  //800b2830
    ~ResFontBase(); //800b2870
    void Rebuild(); //800b2a20
    bool SetResource(void * resource); //800b28d0
    nw4r::ut::Font * font; 
    void * resource;
    FontInformation * fontInformation;
    u16 lastCharCode;
    u16 lastGlyphIndex;
};

class FontManager
{
    public:
    FontManager(); //805d130c
    ~FontManager(); //805d132c
    static FontManager * sInstance; //809c1bf8
    void CreateStaticInstance(); //805d13c4
    void DestroyStaticInstance(); //805d1414
    void Init(); //805d14a4
    void LoadFont(int index);
    ResFontBase * GetFontByName(const char * name); //805d1804
    ResFontBase * fonts[6];
};


#endif