#ifndef UIEDITOR_EDITORBASE_H
#define UIEDITOR_EDITORBASE_H

#include "../ImGuiBase.h"
#include <windows.h>
#include <shlobj_core.h>
#include <iostream>
#include <format>
#include <fstream>
#include <boost/uuid/detail/sha1.hpp>
#include "yaz.h"
#include <sstream>
#define CURL_STATICLIB
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <expected>
#include <glad/glad.h>
#include "ImageLoader.h"

const int MAX_FILEPATH_LENGTH = 260;
const int MAX_TRACK_NAME_LENGTH = 42;
const int MAX_FIELD_LENGTH = 0x60;
const int MAX_BUFFER_LENGTH = MAX_TRACK_NAME_LENGTH + MAX_FIELD_LENGTH + 0x10;

const char CREATOR_NAME[] = "Cosmos Cup Creator";
const char CREATOR_VERSION[] = "v2.0";
const char CONFIG_HEADER[] = "CSCF";
const uint32_t CONFIG_VERSION = 3;

const uint8_t music_slot_map[] = {
        0x75, 0x77, 0x79, 0x7B,
        0x7D, 0x7F, 0x81, 0x83,
        0x87, 0x85, 0x8F, 0x8B,
        0x89, 0x8D, 0x91, 0x93,
        0xA5, 0xAD, 0x97, 0x9F,
        0x9D, 0x95, 0xAF, 0xA9,
        0xB1, 0x9B, 0xA1, 0xA7,
        0x99, 0xB3, 0xAB, 0xA3,
        0xB7, 0xB5, 0xB9, 0xBB, 0xBD,
        0xC3, 0xC5, 0xC7, 0xBF, 0xC1,

};

const char slot_names[] = "Luigi Circuit\0Moo Moo Meadows\0Mushroom Gorge\0Toad's Factory\0" \
"Mario Circuit\0Coconut Mall\0DK Summit\0Wario's Gold Mone\0" \
"Daisy Circuit\0Koopa Cape\0Maple Treeway\0Grumble Volcano\0" \
"Dry Dry Ruins\0Moonview Highway\0Bowser's Castle\0Rainbow Road\0" \
"GCN Peach Beach\0DS Yoshi Falls\0SNES Ghost Valley 2\0N64 Mario Raceway\0" \
"N64 Sherbet Land\0GBA Shy Guy Beach\0DS Delfino Square\0GCN Waluigi Stadium\0" \
"DS Desert Hills\0GBA Bowser Castle 3\0N64 DK's Jungle Parkway\0GCN Mario Circuit\0" \
"SNES Mario Circuit 3\0DS Peach Gardens\0GCN DK Mountain\0N64 Bowser's Castle";

const char music_slots[] = "Luigi Circuit\0Moo Moo Meadows\0Mushroom Gorge\0Toad's Factory\0" \
"Mario Circuit\0Coconut Mall\0DK Summit\0Wario's Gold Mone\0" \
"Daisy Circuit\0Koopa Cape\0Maple Treeway\0Grumble Volcano\0" \
"Dry Dry Ruins\0Moonview Highway\0Bowser's Castle\0Rainbow Road\0" \
"GCN Peach Beach\0DS Yoshi Falls\0SNES Ghost Valley 2\0N64 Mario Raceway\0" \
"N64 Sherbet Land\0GBA Shy Guy Beach\0DS Delfino Square\0GCN Waluigi Stadium\0" \
"DS Desert Hills\0GBA Bowser Castle 3\0N64 DK's Jungle Parkway\0GCN Mario Circuit\0" \
"SNES Mario Circuit 3\0DS Peach Gardens\0GCN DK Mountain\0N64 Bowser's Castle\0" \
"Block Plaza\0Delfino Pier\0Funky Stadium\0Chain Chomp Wheel\0Thwomp Desert\0" \
"SNES Battle Course 4\0GBA Battle Course 3\0N64 Skyscraper\0GCN Cookie Land\0DS Twilight House\0";

const int slotValues[] = { 0x08, 0x01, 0x02, 0x04, 0x00, 0x05, 0x06, 0x07, 0x09, 0x0F, 0x0B, 0x03, 0x0E, 0x0A, 0x0C, 0x0D, 0x10, 0x14, 0x19, 0x1A, 0x1B, 0x1F, 0x17, 0x12, 0x15, 0x1E, 0x1D, 0x11, 0x18, 0x16, 0x13, 0x1C, 0x21, 0x20, 0x23, 0x22, 0x24, 0x27, 0x28, 0x29, 0x25, 0x26 };

class TrackInfo;
class GroupDefinition;
class TrackSlot {
public:
    enum class Type {
        TRACK,
        GROUP,
    };
    Type type;
    std::expected<TrackInfo*, std::string> GetTrackInfo() {
        if(type == Type::TRACK) return (TrackInfo*)(this);
        return std::unexpected("Not a TrackInfo");
    };
    std::expected<GroupDefinition*, std::string> GetGroupDefinition() {
        if(type == Type::GROUP) return (GroupDefinition*)(this);
        return std::unexpected("Not a TrackInfo");
    };
};

class TrackInfo : TrackSlot {
public:
    char trackName[MAX_TRACK_NAME_LENGTH];
    char trackAuthor[MAX_FIELD_LENGTH];
    char filepath[MAX_FILEPATH_LENGTH];
    int trackSlot = 0;
    int musicSlot = 0;
    bool isRetro = false;
};
class GroupDefinition : TrackSlot {
public:
    char name[0x60] = {};
    int trackCount = 0;
    std::array<int, 8> trackReferences = {-1,-1,-1,-1,-1,-1,-1,-1};
    std::array<char[MAX_TRACK_NAME_LENGTH], 8> trackLabel = {};
};
struct CupDefinition {
    char name[0x60];
    char imagePath[MAX_FILEPATH_LENGTH];
    GLuint texture = -1;
    bool hasTexture;
    std::array<int, 4> numericalSlots = {-1,-1,-1,-1};
    std::array<TrackSlot*, 4> trackSlots;
};

struct LayoutData {
    char name[MAX_FIELD_LENGTH];
    int cupCount = 0;
    std::vector<CupDefinition> cups;
    bool includeVanilla = true;
    bool reuseFirstIcons = false;
    int firstIconsOffset = 0;
};


class EditorBase : public ImGuiBase{
public:
    static EditorBase * GetInstance() { return sInstance; }

    void Init(GLFWwindow * window, const char * glsl_version) override;
    void Update() override;
    void MarkChange() { hasChanges = true; }


    void RenderMainBar();
    void RenderViewPort();

    void RenderTrackDefWindow();
    void OpenFileDialog(TrackInfo& info);
    void GetWiimmTrackInfo(TrackInfo& info);
    void ImportMultipleFiles();

    void RenderGroupDefWindow();

    void RenderLayoutEditor();
    void OpenImageDialog(CupDefinition& cup);
    void ImportMultipleIcons(LayoutData& data);

    void ExportProjectToJson(bool saveas = false);
    void ImportProjectFromJson();

    void ValidateBinaries() { return; }
    void ExportBinaries();
    void ExportImages(const char* folderPath);
    void ExportCupIcons(const char* folderPath, LayoutData& data, int index = -1);

    void ExportTracks(const char* folderPath);
    void ExportConfig(const char* folderPath);
    void ExportBMGs(const char* folderPath);

    inline void GenerateTrackNameVector(){
        itemNames.resize(0);
        for(int i = 0; i < this->trackDefinitionsCount; i++){
            itemNames.emplace_back(std::format("{} ({})", trackDefinitions[i].trackName, trackDefinitions[i].trackAuthor));
        }
    }
    inline void GenerateSlotNameVector(){
        allSlotsNames.resize(0);
        allSlotsNames.append_range(itemNames);
        for(int i = 0; i < this->groupDefinitionsCount; i++){
            allSlotsNames.emplace_back(std::format("{} [Group]", this->groupDefinitions[i].name));
        }
    }

    static EditorBase * sInstance;
    bool hasChanges = false;
    bool confirmingExit = false;
    bool wiimmDownload = true;
    bool showNoDataWarning = false;
    int noDataCount = 0;

    std::string currentProjectPath{};

    int trackDefinitionsCount = 0;
    std::vector<TrackInfo> trackDefinitions = {};
    std::string nullTerminatedTrackNames;
    std::vector<std::string> itemNames;

    int groupDefinitionsCount = 0;
    std::vector<GroupDefinition> groupDefinitions = {};

    int layoutCount = 0;
    bool updateBool = false;
    bool shareIcons = true;
    bool createAlphabetical = true;
    std::vector<LayoutData> layoutDefinitions;
    std::vector<std::string> allSlotsNames;
};

#endif //UIEDITOR_EDITORBASE_H
