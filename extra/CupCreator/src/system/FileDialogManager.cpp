//
// Created by Gabi on 3/28/2024.
//

#include "FileDialogManager.h"
#include <algorithm>

std::string FileDialogManager::OpenFileDialog(const std::string& title, const std::string &filter) {

    char filename[ MAX_PATH ];

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if(GetOpenFileNameA(&ofn)){
        return {filename};
    }

    return {};
}

std::string FileDialogManager::OpenFileDialog() {
    return OpenFileDialog("Open File Dialog", "");
}

std::string FileDialogManager::OpenFileDialog(const std::string &title) {
    return OpenFileDialog(title, "");
}
