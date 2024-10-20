#include "EditorBase.h"

EditorBase * EditorBase::sInstance = nullptr;

void EditorBase::Init(GLFWwindow *window, const char *glsl_version) {
    if(sInstance != nullptr) throw ("Another instance of EditorBase already exists!");
    ImGuiBase::Init(window, glsl_version);
    sInstance = this;

    ImGuiIO& io = ImGui::GetIO();
}


void EditorBase::Update() {
    RenderMainBar();
    RenderViewPort();

//region ConfirmExit

    if(this->confirmingExit)
    {
        bool a = true;
        if(!hasChanges) exit(0);

        if(ImGui::Begin("Confirm Exit", &a, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize))
        {
            ImGui::SetWindowSize(ImVec2(280.0f, 80.0f));
            ImGui::Text("Are you sure you want to exit?");
            if(ImGui::Button("No")){
                this->confirmingExit = false;
            }
            ImGui::SameLine();
            if(ImGui::Button("Yes")){
                exit(0);
            }
            ImGui::End();
        }
        if(!a) this->confirmingExit = false;
//endregion




    }
}

void EditorBase::RenderMainBar() {
    ImGui::BeginMainMenuBar();

    if(ImGui::BeginMenu("File")){
        ImGui::BeginMenuBar();

        if(ImGui::MenuItem("Open")){
            this->ImportProjectFromJson();
        }
        if(ImGui::MenuItem("Save")){
            this->ExportProjectToJson(false);
        };
        if(ImGui::MenuItem("Save as...")){
            this->ExportProjectToJson(true);
        }
        ImGui::Separator();
        if(ImGui::MenuItem("Export binaries")){
            this->ExportBinaries();
        }
        ImGui::Separator();
        if(ImGui::MenuItem("Exit"))
        {
            this->confirmingExit = true;
        }
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Tools")){

        if(ImGui::MenuItem("Import multiple files")){
            this->ImportMultipleFiles();
        }

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void EditorBase::RenderViewPort() {

    ImGuiViewport * viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags rootWindowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
    rootWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Root", nullptr, rootWindowFlags);
    //DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("Root");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    }


    ImGui::End(); //Root
    this->RenderTrackDefWindow();
    this->RenderGroupDefWindow();
    this->RenderLayoutEditor();

    if(this->showNoDataWarning){
        ImGui::Begin("No Data Found", &this->showNoDataWarning);
        ImGui::Text("Couldn't download data for %d track%s.", noDataCount, noDataCount > 1 ? "s": "");
        ImGui::End();
    }
}

//region TrackDefWindow

void EditorBase::RenderTrackDefWindow() {
    ImGui::Begin("Track Definition Editor");

    if(ImGui::InputInt("Track Count", &this->trackDefinitionsCount, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue)){
        this->updateBool = true;
        if(this->trackDefinitionsCount < 0) this->trackDefinitionsCount = 0;
        if(this->trackDefinitionsCount > 1024) this->trackDefinitionsCount = 1024;
        while(this->trackDefinitionsCount > this->trackDefinitions.size()){
            for(LayoutData& layout : this->layoutDefinitions){
                for(CupDefinition& cup : layout.cups){
                    for(int n = 0; n < 4; n++){
                        if(cup.numericalSlots[n] >= this->trackDefinitions.size()) {
                            cup.numericalSlots[n] += 1;
                        }
                    }
                }
            }
            this->trackDefinitions.push_back({});
        }
        /*
        while(this->trackDefinitionsCount < this->trackDefinitions.size()){
            for(auto layout : this->layoutDefinitions){
                for(auto cup : layout.cups){
                    for(int n = 0; n < 4; n++){
                        if(cup.numericalSlots[n] >= this->trackDefinitions.size()) {
                            cup.numericalSlots[n]--;
                        }
                    }
                }
            }
            this->trackDefinitions.pop_back();
        }
         */
        this->GenerateTrackNameVector();
    }
    ImGui::Checkbox("Download track info from ct.wiimm.de", &this->wiimmDownload);

    float width = (230.0f/1350.0f) * ImGui::GetWindowWidth();
    if(width > 240.0f) width = 240.0f;

    ImGui::PushItemWidth(width);
    ImGui::LabelText("##Track Name", "Track Name"); ImGui::SameLine();
    ImGui::LabelText("##Author Name", "Author Name"); ImGui::SameLine();
    ImGui::LabelText("##File Path", "Filename"); ImGui::SameLine(0.0f, 60.0f);
    ImGui::LabelText("##Track Slot", "Track Slot"); ImGui::SameLine();
    ImGui::LabelText("##Music Slot", "Music Slot");

    ImGui::BeginChild("Track listing");
    for(int i = 0; i < this->trackDefinitionsCount; i++){
        ImGui::Separator();
        ImGui::PushItemWidth(width);
        if(ImGui::InputText(std::format("##Track{}", i).c_str(), this->trackDefinitions[i].trackName, MAX_TRACK_NAME_LENGTH)){
            this->GenerateTrackNameVector();
        } ImGui::SameLine();
        if(ImGui::InputText(std::format("##Author{}", i).c_str(), this->trackDefinitions[i].trackAuthor, MAX_FIELD_LENGTH)) {
            this->GenerateTrackNameVector();
        }
        ImGui::SameLine();
        std::string filepath = std::string(this->trackDefinitions[i].filepath);
        std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);
        ImGui::InputText(std::format("##FilePath{}",i).c_str(), (char*)filename.c_str(), MAX_FILEPATH_LENGTH, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
        if(ImGui::Button(std::format("Select##{}", i).c_str())){
            this->OpenFileDialog(this->trackDefinitions[i]);
        }
        ImGui::SameLine();
        ImGui::Combo(std::format("##Slot{}", i).c_str(), &this->trackDefinitions[i].trackSlot, slot_names); ImGui::SameLine();
        ImGui::Combo(std::format("##Music{}",i).c_str(), &this->trackDefinitions[i].musicSlot,  music_slots);
    }

    ImGui::EndChild();
    ImGui::End();
}

void EditorBase::OpenFileDialog(TrackInfo &info) {
    OPENFILENAMEA ofn;
    char filepath[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Nintendo Track Files (*.szs, *.lzma)\0*.szs;*.lzma\0";
    ofn.nFilterIndex = 1;
    ofn.nMaxFile = sizeof(filepath);
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetOpenFileName(&ofn)){
        strcpy_s(info.filepath, MAX_FILEPATH_LENGTH, filepath);
        if(info.trackName[0] == '\0' && info.trackAuthor[0] == '\0'){
            this->GetWiimmTrackInfo(info);
        }
    }

}
size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

void EditorBase::GetWiimmTrackInfo(TrackInfo &info) {
    if(!this->wiimmDownload) return;

    std::ifstream ifs(info.filepath, std::ios::binary | std::ios::ate);
    std::streamsize size = ifs.tellg();
    if(size < 0) return;
    ifs.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    if(ifs.read((char*)buffer.data(), size)){
        auto decompressed = Decompress(buffer);

        boost::uuids::detail::sha1 sha;
        sha.reset();
        sha.process_bytes(decompressed.data(), decompressed.size());
        unsigned hash[5] = {0};
        sha.get_digest(hash);
        char buf[41] = {0};
        for(int i = 0; i < 5; i++) std::sprintf(buf + (i << 3), "%08x", hash[i]);

        auto curl = curl_easy_init();
        if(curl){
            curl_easy_setopt(curl, CURLOPT_URL, std::format("https://ct.wiimm.de/api/get-track-info?sha1={}", buf).c_str());
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

            std::string response_string;
            std::string header_string;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

            char* url;
            long response_code;
            double elapsed;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
            curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            using json = nlohmann::json;
            auto data = json::parse(response_string);
            try {
                std::string name = data["split_name"]["name"].get<std::string>();
                std::string prefix = "";
                bool has_prefix = !data["split_name"]["prefix"].is_null();
                if(has_prefix)
                    prefix = data["split_name"]["prefix"].get<std::string>();
                std::string author = data["split_name"]["authors"].get<std::string>();

                if(has_prefix){
                    snprintf(info.trackName, MAX_TRACK_NAME_LENGTH - 1, "%s %s", prefix.c_str(), name.c_str());
                }
                else strncpy(info.trackName,  name.c_str(), sizeof(info.trackName));
                strncpy(info.trackAuthor, author.c_str(), MAX_FIELD_LENGTH);

                info.trackSlot = ((data["slot"].get<int>() / 10) - 1) * 4 + ((data["slot"].get<int>() % 10) - 1);
                int music_id = data["music_id"].get<int>();
                int music_slot = 0;
                for(int i = 0; i < 42; i++){
                    if(music_id == music_slot_map[i]) music_slot = i;
                }
                info.musicSlot = music_slot;
            }
            catch(std::exception& e){
                return;
            }
        }
    }
}

void EditorBase::ImportMultipleFiles() {
    this->trackDefinitions.resize(this->trackDefinitionsCount);

    OPENFILENAMEA ofn;
    char filepath[1024];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Nintendo Track Files (*.szs, *.lzma)\0*.szs;*.lzma\0";
    ofn.nFilterIndex = 1;
    ofn.nMaxFile = sizeof(filepath);
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    if(GetOpenFileName(&ofn)){
        std::string folderName = std::string(ofn.lpstrFile);
        folderName = folderName.substr(0, ofn.nFileOffset);
        if(folderName[ofn.nFileOffset - 1] == '\\') folderName = folderName.substr(0, ofn.nFileOffset - 1);
        std::vector<std::string> files = {};
        char* ptr = filepath;
        ptr += ofn.nFileOffset;
        while(*ptr){
            if(ptr >= &filepath[1023]) break;
            files.emplace_back(std::format("{}\\{}", folderName, ptr));
            ptr += (strlen(ptr)+1);
        }
        this->noDataCount = 0;

        int empty_defines = 0;
        for(int i = this->trackDefinitions.size(); i > 0; i--){
            TrackInfo& info = this->trackDefinitions[i-1];
            if(info.trackName[0] != '\0' || info.trackAuthor[0] != '\0' || info.filepath[0] != '\0' || info.trackSlot != 0 || info.musicSlot != 0){
                break;
            }
            empty_defines++;
        }
        this->trackDefinitionsCount -= empty_defines;
        for(; empty_defines > 0; empty_defines--) this->trackDefinitions.pop_back();

        std::vector<TrackInfo> temp;
        for(int i = 0; i < files.size(); i++){
            TrackInfo info = {};
            strcpy_s(info.filepath, MAX_FILEPATH_LENGTH, files[i].c_str());
            this->GetWiimmTrackInfo(info);
            if(info.trackName[0] == '\0') this->noDataCount++;
            temp.emplace_back(info);
        }

        std::vector<TrackInfo> tempAlphabetized;
        std::sort(temp.begin(), temp.end(), [](TrackInfo& a, TrackInfo& b){ return std::string(a.trackName) < std::string(b.trackName); });

        this->trackDefinitions.append_range(temp);
        if(this->noDataCount > 0) this->showNoDataWarning = true;

        for(int i = 0; i < this->layoutCount; i++) {
            for(int j = 0; j < this->layoutDefinitions[i].cupCount; j++) {
                for(int u = 0; u < 4; u++) {
                    if(this->layoutDefinitions[i].cups[j].numericalSlots[u] >= this->trackDefinitionsCount) this->layoutDefinitions[i].cups[j].numericalSlots[u] += temp.size();
                }
            }
        }
        this->trackDefinitionsCount += temp.size();

        this->GenerateTrackNameVector();

    }
}

//endregion

//region GroupDefWindow

void EditorBase::RenderGroupDefWindow(){
    ImGui::Begin("Group Definition Editor");

    if(ImGui::InputInt("Group Count", &this->groupDefinitionsCount)){
        if(this->groupDefinitionsCount < 0) this->groupDefinitionsCount = 0;
        while(this->groupDefinitionsCount > this->groupDefinitions.size()){
            this->groupDefinitions.push_back({});
        }
    }

    char** names = new char*[this->trackDefinitionsCount];
    for(int i = 0; i < this->trackDefinitionsCount; i++){
        names[i] = new char[MAX_BUFFER_LENGTH];
        snprintf(names[i], MAX_BUFFER_LENGTH, "%s (%s)", this->trackDefinitions[i].trackName, this->trackDefinitions[i].trackAuthor);
    }

    for(int i = 0; i < groupDefinitionsCount; i++){
        GroupDefinition& definition = this->groupDefinitions[i];
        ImGui::Separator();
        ImGui::PushItemWidth(250.0f);
        ImGui::InputText(std::format("Group Name##{}", i).c_str(), definition.name, MAX_FIELD_LENGTH);
        ImGui::SameLine();
        if(ImGui::InputInt(std::format("Track Count##{}", i).c_str(), &definition.trackCount)){
            if(definition.trackCount < 0) definition.trackCount = 0;
            if(definition.trackCount > 8) definition.trackCount = 8;
        }
        ImGui::PushItemWidth(300.0f);
        for(int j = 0; j < definition.trackCount; j++){
            ImGui::Combo(std::format("Track##{}j{}", i, j).c_str(), &definition.trackReferences[j], names, this->trackDefinitionsCount);
            ImGui::SameLine();
            ImGui::InputText(std::format("Label##{}{}", i, j).c_str(), definition.trackLabel[j], MAX_TRACK_NAME_LENGTH);
        }

        ImGui::PopItemWidth();
    }
    for(int i = 0; i < this->trackDefinitionsCount; i++){
        free(names[i]);
    }
    free(names);
    ImGui::End();
}

//endregion

//region LayoutCreatorWindow

void EditorBase::RenderLayoutEditor(){
    ImGui::Begin("Layout Editor");

    if(ImGui::InputInt("Layout Count", &this->layoutCount)) {
        if (this->layoutCount < 0) this->layoutCount = 0;
        while (this->layoutCount > this->layoutDefinitions.size()) {
            LayoutData layout;
            snprintf(layout.name, MAX_FIELD_LENGTH, "Layout %d", this->layoutDefinitions.size());
            this->layoutDefinitions.push_back(layout);
        }
    }

    char** names = new char*[this->trackDefinitionsCount + this->groupDefinitionsCount];
    for(int i = 0; i < this->trackDefinitionsCount; i++){
        names[i] = new char[MAX_BUFFER_LENGTH];
        snprintf(names[i], MAX_BUFFER_LENGTH, "%s (%s)", this->trackDefinitions[i].trackName, this->trackDefinitions[i].trackAuthor);
    }
    for(int i = 0; i < this->groupDefinitionsCount; i++){
        names[i + this->trackDefinitionsCount] = new char[MAX_BUFFER_LENGTH];
        snprintf(names[i + this->trackDefinitionsCount], MAX_BUFFER_LENGTH, "%s [Group]", this->groupDefinitions[i].name);
    }

    ImGui::BeginTabBar("Tabbar", ImGuiTabBarFlags_AutoSelectNewTabs);

    for(int i = 0; i < this->layoutCount; i++){
        if(updateBool) continue;
        LayoutData& layout = this->layoutDefinitions[i];
        if(ImGui::BeginTabItem(std::format("Layout {}",  i).c_str())){
            ImGui::PushItemWidth(250.0f);
            ImGui::InputText(std::format("Layout Name##{}", i).c_str(), layout.name, MAX_FIELD_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            if(ImGui::Button("Import icons")){
                this->ImportMultipleIcons(layout);
            }
            if(ImGui::InputInt(std::format("Cup Count##{}", i).c_str(), &layout.cupCount)){
                if(layout.cupCount < 0) layout.cupCount = 0;
                if(layout.cupCount > 256) layout.cupCount = 256;
                while (layout.cupCount > layout.cups.size()) {
                    CupDefinition cup{};
                    snprintf(cup.name, MAX_FIELD_LENGTH, "Cup %d", layout.cups.size() + 1);
                    layout.cups.push_back(cup);
                }
            }
            ImGui::SameLine();
            if(i != 0) {
                ImGui::Checkbox("Reuse Icons", &layout.reuseFirstIcons);
                if(layout.reuseFirstIcons) {
                    if(ImGui::InputInt("Offset", &layout.firstIconsOffset, 1)) {
                        if(layout.firstIconsOffset < 0) layout.firstIconsOffset = 0;
                        if(layout.firstIconsOffset > (this->layoutDefinitions[0].cupCount - layout.cupCount) ) layout.firstIconsOffset = (this->layoutDefinitions[0].cupCount - layout.cupCount);

                        for(int j = 0; j < layout.cupCount; j++) {
                            layout.cups[j].texture = this->layoutDefinitions[0].cups[j + layout.firstIconsOffset].texture;
                        }
                    }
                }
            }


            for(int j = 0; j < layout.cupCount; j++){
                CupDefinition& cup = layout.cups[j];
                ImGui::Separator();
                if(ImGui::ImageButton(std::format("Button##{}/{}",i,j).c_str(), (ImTextureID) cup.texture, {128.0f, 128.0f})){
                    OpenImageDialog(cup);
                }
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::InputText(std::format("Cup Name##{}/{}",i,j).c_str(), cup.name, MAX_FIELD_LENGTH);
                for(int n = 0; n < 4; n++){
                    ImGui::Combo(std::format("Track {}##Selector{}/{}/{}",n + 1, i,j,n).c_str(), &layout.cups[j].numericalSlots[n], names, this->trackDefinitionsCount + this->groupDefinitionsCount);
                }
                ImGui::EndGroup();
            }
            ImGui::EndTabItem();
        }
    }
    for(int i = 0; i < this->trackDefinitionsCount + this->groupDefinitionsCount; i++){
        free(names[i]);
    }
    free(names);

    ImGui::EndTabBar();

    if(updateBool) updateBool = false;
    ImGui::End();
}

void EditorBase::OpenImageDialog(CupDefinition &cup) {
    OPENFILENAMEA ofn;
    char filepath[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Image File (*.png)\0*.png\0";
    ofn.nFilterIndex = 1;
    ofn.nMaxFile = sizeof(filepath);
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetOpenFileName(&ofn)){
        GLuint texture;
        int width;
        int height;
        if(ImageLoader::LoadTextureFromFile(filepath, &texture, &width, &height)){
            if(cup.hasTexture){
                glDeleteTextures(1, &cup.texture);
            }
            cup.texture = texture;
            cup.hasTexture = true;
            strcpy_s(cup.imagePath, MAX_FILEPATH_LENGTH, filepath);
        }
    }
}

void EditorBase::ImportMultipleIcons(LayoutData& data){
    data.cups.resize(data.cupCount);

    OPENFILENAMEA ofn;
    char filepath[1024];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Image files (*.png)\0*.png\0";
    ofn.nFilterIndex = 1;
    ofn.nMaxFile = sizeof(filepath);
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
    if(GetOpenFileName(&ofn)) {
        std::string folderName = std::string(ofn.lpstrFile);
        folderName = folderName.substr(0, ofn.nFileOffset);
        if (folderName[ofn.nFileOffset - 1] == '\\') folderName = folderName.substr(0, ofn.nFileOffset - 1);
        std::vector<std::string> files{};
        char *ptr = filepath;
        ptr += ofn.nFileOffset;
        while (*ptr) {
            if (ptr >= &filepath[1023]) break;
            files.emplace_back(std::format("{}\\{}", folderName, ptr));
            ptr += (strlen(ptr) + 1);
        }

        int empty_defines = 0;
        for(int i = data.cups.size(); i > 0; i--){
            CupDefinition& cup = data.cups[i -1];
            if(!std::string(cup.name).starts_with("Cup ") || cup.hasTexture) break;
            if(cup.numericalSlots[0] != -1) break;
            if(cup.numericalSlots[1] != -1) break;
            if(cup.numericalSlots[2] != -1) break;
            if(cup.numericalSlots[3] != -1) break;
            empty_defines++;
        }
        data.cupCount -= empty_defines;
        for(; empty_defines > 0; empty_defines--) data.cups.pop_back();
        for(const auto& file : files){
            CupDefinition cup;
            std::string filename = file.substr(file.find_last_of("/\\") + 1);
            std::string cupName = filename.substr(0, filename.find_last_of("."));
            strcpy_s(cup.name, MAX_FIELD_LENGTH, cupName.c_str());

            GLuint texture;
            int width;
            int height;
            if(ImageLoader::LoadTextureFromFile(file.c_str(), &texture, &width, &height)){
                if(cup.hasTexture){
                    glDeleteTextures(1, &cup.texture);
                }
                cup.texture = texture;
                cup.hasTexture = true;
                strcpy_s(cup.imagePath, MAX_FILEPATH_LENGTH, file.c_str());
            }
            data.cups.emplace_back(cup);
            data.cupCount++;
        }
    }
}
//endregion

//region File Export/Import

void EditorBase::ExportProjectToJson(bool saveas) {

    char filepath[260];
    if(saveas || this->currentProjectPath.empty()){
        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = "Cosmos Config File (*.csjs)\0*.csjs\0";
        ofn.nFilterIndex = 1;
        ofn.nMaxFile = sizeof(filepath);
        ofn.lpstrFile = filepath;
        ofn.lpstrFile[0] = '\0';
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if(!GetSaveFileName(&ofn)) return;
    }
    else strcpy_s(filepath, MAX_FILEPATH_LENGTH, this->currentProjectPath.c_str());

    using json = nlohmann::ordered_json;
    json main;
    json info = {
            {"creator", CREATOR_NAME},
            {"version", CREATOR_VERSION},
    };
    json trackDefs = {
            {"count", this->trackDefinitionsCount},
    };

    std::vector<json> tracks{};
    for(int i = 0; i < this->trackDefinitionsCount; i++){
        TrackInfo& def = this->trackDefinitions[i];
        json track = {
                {"name", def.trackName},
                {"author", def.trackAuthor},
                {"filepath", def.filepath},
                {"track_slot", def.trackSlot},
                {"music_slot", def.musicSlot},
                {"is_retro", def.isRetro},
        };
        tracks.emplace_back(track);
    }
    trackDefs["tracks"] = tracks;

    json groupDefs = {
            {"count", this->groupDefinitionsCount},
    };
    std::vector<json> groups{};
    for(int i = 0; i < this->groupDefinitionsCount; i++){
        GroupDefinition& def = this->groupDefinitions[i];
        json group = {
                {"name", def.name},
                {"count", def.trackCount},
                {"references", def.trackReferences},
                {"labels", def.trackLabel}
        };
        groups.emplace_back(group);
    }
    groupDefs["groups"] = groups;

    json layoutData = {
            {"count", this->layoutCount},
            {"create_alphabetical", this->createAlphabetical},
            {"share_icons", this->shareIcons},
    };

    std::vector<json> layoutDatas{};
    for(int i = 0; i < this->layoutCount; i++){
        LayoutData& data = this->layoutDefinitions[i];
        json layout = {
                {"name", data.name},
                {"count", data.cupCount},
                {"include_vanilla", data.includeVanilla},
        };
        std::vector<json> cups{};
        for(int j = 0; j < data.cupCount; j++){
            CupDefinition& cup = data.cups[j];
            json cupData = {
                    {"name", cup.name},
                    {"image_path", cup.imagePath},
                    {"numerical_slots", cup.numericalSlots},
            };
            cups.emplace_back(cupData);
        }
        layout["cups"] = cups;
        layoutDatas.emplace_back(layout);
    }
    layoutData["layouts"] = layoutDatas;

    main["info"] = info;
    main["track_definitions"] = trackDefs;
    main["group_definitions"] = groupDefs;
    main["layout_definitions"] = layoutData;

    auto exported_data = main.dump(4);

    std::ofstream file(filepath);
    if(file.is_open()) {
        file << exported_data;
        file.close();
    }
    this->currentProjectPath = std::string(filepath);
}

void EditorBase::ImportProjectFromJson(){
    OPENFILENAMEA ofn;
    char filepath[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Cosmos Config File (*.csjs)\0*.csjs\0";
    ofn.nFilterIndex = 1;
    ofn.nMaxFile = sizeof(filepath);
    ofn.lpstrFile = filepath;
    ofn.lpstrFile[0] = '\0';
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(!GetOpenFileName(&ofn)) return;

    std::ifstream file(filepath);
    std::stringstream buffer;
    if(file.is_open()){
       buffer << file.rdbuf();
       file.close();
    }

    using json = nlohmann::json;
    auto data = json::parse(buffer.str());

    try{
       if(data["info"]["creator"].get<std::string>() != CREATOR_NAME){
           return;
       }
       auto& tracks = data["track_definitions"]["tracks"];

       this->trackDefinitions.clear();

       for(auto& elem : tracks.items()){
            json track = elem.value();
            TrackInfo info;
            strcpy_s(info.trackName, MAX_TRACK_NAME_LENGTH, track["name"].get<std::string>().c_str());
            strcpy_s(info.trackAuthor, MAX_FIELD_LENGTH, track["author"].get<std::string>().c_str());
            strcpy_s(info.filepath, MAX_FILEPATH_LENGTH, track["filepath"].get<std::string>().c_str());
            info.trackSlot = track["track_slot"].get<int>();
            info.musicSlot = track["music_slot"].get<int>();
            info.isRetro = track["is_retro"].get<int>();

            this->trackDefinitions.emplace_back(info);
       }
       this->trackDefinitionsCount = this->trackDefinitions.size();

       auto& groups = data["group_definitions"]["groups"];

       this->groupDefinitions.clear();

       for(auto& elem : groups.items()){
           json group = elem.value();
           GroupDefinition def;
           strcpy_s(def.name, MAX_FIELD_LENGTH, group["name"].get<std::string>().c_str());
           def.trackCount = group["count"].get<int>();
           int i = 0;
           for(auto& ref : group["references"].items()){
               def.trackReferences[i++] = ref.value().get<int>();
           }
           i = 0;
           for(auto& ref : group["labels"].items()) {
               strcpy_s(def.trackLabel[i++], MAX_TRACK_NAME_LENGTH, ref.value().get<std::string>().c_str());
           }

           this->groupDefinitions.emplace_back(def);
       }
       this->groupDefinitionsCount = this->groupDefinitions.size();

       auto& layouts = data["layout_definitions"]["layouts"];
       this->createAlphabetical = data["layout_definitions"]["create_alphabetical"].get<bool>();
       this->shareIcons = data["layout_definitions"]["share_icons"].get<bool>();

       this->layoutDefinitions.clear();

       for(auto& elem : layouts.items()){
           json layout = elem.value();
           LayoutData def;
           strcpy_s(def.name, MAX_FIELD_LENGTH, layout["name"].get<std::string>().c_str());
           for(auto& c : layout["cups"].items()){
               json cup = c.value();
               CupDefinition cupDef;
               strcpy_s(cupDef.name, MAX_FIELD_LENGTH, cup["name"].get<std::string>().c_str());
               strcpy_s(cupDef.imagePath, MAX_FILEPATH_LENGTH, cup["image_path"].get<std::string>().c_str());
               int j = 0;
               for(auto& t : cup["numerical_slots"].items()){
                  cupDef.numericalSlots[j++] = t.value().get<int>();
               }

               GLuint texture;
               int width;
               int height;
               if(ImageLoader::LoadTextureFromFile(cupDef.imagePath, &texture, &width, &height)){
                   cupDef.texture = texture;
                   cupDef.hasTexture = true;
               }
               def.cups.emplace_back(cupDef);
           }
           def.cupCount = def.cups.size();
           def.includeVanilla = layout["include_vanilla"].get<bool>();
           this->layoutDefinitions.emplace_back(def);
       }
       this->layoutCount = this->layoutDefinitions.size();
       currentProjectPath = std::string(filepath);
    }
    catch(std::exception& e){
        return;
    }

}

void EditorBase::ExportBinaries(){
    this->ValidateBinaries();

    TCHAR dir[MAX_PATH];
    BROWSEINFO info;
    info.hwndOwner = NULL;
    info.pidlRoot = NULL;
    info.pszDisplayName = dir;
    info.ulFlags = BIF_USENEWUI;
    //info.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS ;
    info.lpszTitle = "Select Folder";
    info.lpfn = NULL;
    info.lParam = 0;
    info.iImage = -1;

    LPITEMIDLIST lpItem = SHBrowseForFolder(&info);
    if(lpItem == NULL) return;
    SHGetPathFromIDList(lpItem, dir);
    std::cout << dir << std::endl;
    this->ExportConfig(dir);
    this->ExportImages(dir);
    this->ExportBMGs(dir);
    this->ExportTracks(dir);

}

void EditorBase::ExportImages(const char* folderPath) {
    std::string imageFolder = std::format("{}\\{}", folderPath, "images");

    if(CreateDirectory(imageFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()){
        if(this->shareIcons) this->ExportCupIcons(imageFolder.c_str(), this->layoutDefinitions[0]);
        else {
            for(int i = 0; i < this->layoutCount; i++){
                this->ExportCupIcons(imageFolder.c_str(), this->layoutDefinitions[i], i);
            }
        }
    }

}

void EditorBase::ExportCupIcons(const char* folderPath, LayoutData& data, int index) {
    for(int i = 0; i < data.cupCount; i++){
        if(data.cups[i].imagePath[0] == '\0') continue;

        std::string outputPath = std::format("{}\\icon_cup_{:03x}{}.tpl", folderPath, i + (8*data.includeVanilla), index < 0 ? "" : std::format("_{}", index));
        std::string command = std::format(R"(wimgt encode "{}" --dest "{}" --transform R3 -o)", data.cups[i].imagePath, outputPath);

        system(command.c_str());
    }
}

void EditorBase::ExportTracks(const char* folderPath){
    std::string tracksFolder = std::format("{}\\{}", folderPath, "tracks");

    if(CreateDirectory(tracksFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        for(int i = 0; i < this->trackDefinitionsCount; i++){
            TrackInfo& info = this->trackDefinitions[i];

            std::string outputPath = std::format("{}\\{:03X}.lzma", tracksFolder, 0x100 + i);
            std::string command = std::format(R"(wszst compress "{}" --dest "{}" --lzma -o)", info.filepath, outputPath);

            system(command.c_str());
        }
    }
}

template <typename T>
inline void BE_Write(std::ofstream& file, T val) {
    T correct = std::byteswap(val);
    file.write((char*)&correct, sizeof(T));
}

#define RoundUp(val, round) (val + (round - 1)) & ~(round - 1)

void EditorBase::ExportConfig(const char* folderPath) {
    std::string configFile = std::format("{}\\Config.cscf", folderPath);

    std::ofstream file(configFile, std::ios::binary);
    if(file.is_open()){
        file.write(CONFIG_HEADER, strlen(CONFIG_HEADER));
        BE_Write(file, CONFIG_VERSION);
        BE_Write(file, (uint16_t)this->trackDefinitionsCount);
        BE_Write(file, (uint16_t)this->layoutCount);
        BE_Write(file, (uint16_t)this->groupDefinitionsCount);
        uint16_t flags = (this->createAlphabetical << 1) | (this->shareIcons);
        BE_Write(file, flags);
        int layout_count = this->layoutCount;
        if(this->createAlphabetical) layout_count *= 2;

        std::vector<LayoutData> sortedLayouts;
        for(int i = 0; i < this->layoutCount; i++){
            int cupCount = this->layoutDefinitions[i].cupCount;
            std::vector<int> numericalSlots;
            for(auto cup : this->layoutDefinitions[i].cups){
                numericalSlots.emplace_back(cup.numericalSlots[0]);
                numericalSlots.emplace_back(cup.numericalSlots[1]);
                numericalSlots.emplace_back(cup.numericalSlots[2]);
                numericalSlots.emplace_back(cup.numericalSlots[3]);
            }
            std::sort(numericalSlots.begin(), numericalSlots.end(), [](int a, int b) {
                std::string trackA = a >= EditorBase::GetInstance()->trackDefinitionsCount ? std::string((EditorBase::GetInstance()->groupDefinitions[a -EditorBase::GetInstance()->trackDefinitionsCount].name)) : std::string((EditorBase::GetInstance()->trackDefinitions[a].trackName));
                std::string trackB = b >= EditorBase::GetInstance()->trackDefinitionsCount ? std::string((EditorBase::GetInstance()->groupDefinitions[b -EditorBase::GetInstance()->trackDefinitionsCount].name)) : std::string((EditorBase::GetInstance()->trackDefinitions[b].trackName));
                return trackA < trackB;
            });
            LayoutData sorted;
            sorted.cupCount = cupCount;
            for(int j = 0; j < cupCount * 4; j += 4){
                CupDefinition cup;
                cup.numericalSlots[0] = numericalSlots[j];
                cup.numericalSlots[1] = numericalSlots[j + 1];
                cup.numericalSlots[2] = numericalSlots[j + 2];
                cup.numericalSlots[3] = numericalSlots[j + 3];
                sorted.cups.emplace_back(cup);
            }
            sortedLayouts.emplace_back(sorted);

        }


        uint32_t offsetDefs = 0x18 + (4 * layout_count);
        offsetDefs = RoundUp(offsetDefs, 0x10);
        uint32_t offsetGroups = offsetDefs + 2 * this->trackDefinitionsCount;
        offsetGroups = RoundUp(offsetGroups, 0x10);
        std::vector<uint32_t> offsetLayouts ={ RoundUp(offsetGroups + (0x9 * 2 * this->groupDefinitionsCount) + 2, 0x10) };
        for(int i = 1; i < layout_count; i++){
            if(this->createAlphabetical){
                if(((i & 1) == 1)){
                    offsetLayouts.emplace_back(RoundUp(offsetLayouts[i-1] + 4 +(16 * this->layoutDefinitions[(i/2)].cupCount), 0x10));
                }
                else offsetLayouts.emplace_back(RoundUp(offsetLayouts[i-1] + 4 + (16 * this->layoutDefinitions[(i/2) - 1].cupCount), 0x10));
            }
            else offsetLayouts.emplace_back(RoundUp(offsetLayouts[i-2] + 4 + (16 * this->layoutDefinitions[i - 1].cupCount), 0x10));
        }


        BE_Write(file, offsetDefs); //Definitions Offset;
        BE_Write(file, offsetGroups);
        for(auto a : offsetLayouts){
            BE_Write(file, a);
        }
        file.seekp(offsetDefs);
        for(auto& def : this->trackDefinitions){
            BE_Write(file, (uint8_t)slotValues[def.trackSlot]);
            BE_Write(file, (uint8_t)slotValues[def.musicSlot]);
        }

        file.seekp(offsetGroups);
        for(auto& def : this->groupDefinitions){
            BE_Write(file, (uint16_t)def.trackCount);
            for(int i = 0; i < 8; i++) BE_Write(file, (uint16_t)(def.trackReferences[i] != -1 ? def.trackReferences[i] + 0x100 : -1));
        }

        int i = 0;
        for(auto lay : offsetLayouts){
            file.seekp(lay);
            if(this->createAlphabetical){
                int actualIndex = i/2;
                auto& layout = ((i & 1) == 1) ? sortedLayouts[actualIndex] : this->layoutDefinitions[actualIndex];
                BE_Write(file, (uint32_t)layout.cupCount);
                for(auto& cup : layout.cups){
                    for(int j = 0; j < 4; j++){
                        int trackId = cup.numericalSlots[j] >= this->trackDefinitionsCount ? 0x3000 + (cup.numericalSlots[j] - this->trackDefinitionsCount) : cup.numericalSlots[j] + 0x100;
                        BE_Write(file, trackId);
                    }
                }
                i++;
            }
        }
        int p = file.tellp();
        if((p & 0x1F) != 0){
            p = RoundUp(p, 0x20);
            file.seekp(p - 1);
            BE_Write(file, (uint8_t)0);
        }
        file.close();
    }
};

void EditorBase::ExportBMGs(const char* folderPath) {
    std::string textFile = std::format("{}\\Text.txt", folderPath);

    std::ofstream file(textFile, std::ios::out);

    for(int i = 0; i < this->trackDefinitionsCount; i++) {
        file << std::format("{:x} = {}\n", 0x7000 + 0x100 + i, this->trackDefinitions[i].trackName);
    }
    file << "\n";
    for(int i = 0; i < this->groupDefinitionsCount; i++) {
        file << std::format("{:x} = {}\n", 0xa000 + i, this->groupDefinitions[i].name);
    }
    file << "\n";
    for(int i = 0; i < this->groupDefinitionsCount; i++) {
        for(int j = 0; j < this->groupDefinitions[i].trackCount; j++) {
            file << std::format("{:x} = {}\n", 0x210000 + (i * 0x10) + j, this->groupDefinitions[i].trackLabel[j]);
        }
    }
    file << "\n";
    for(int i = 0; i < this->trackDefinitionsCount; i++) {
        file << std::format("{:x} = {}", 0x13000 + 0x100 + i, this->trackDefinitions[i].trackName);
        file << "\\n\\z{800,40}\\c{blue1}";
        file << this->trackDefinitions[i].trackAuthor;
        file << "\\c{off}\n";
    }
    file << "\n";
    for(int i = 0; i < this->layoutDefinitions[0].cupCount; i++) {
        file << std::format("{:x} = {}\n", 0x6000 + i, this->layoutDefinitions[0].cups[i].name);
    }

    file.close();
}
//endregion