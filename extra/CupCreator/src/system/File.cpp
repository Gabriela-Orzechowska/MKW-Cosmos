#include "File.h"

File::File(const std::string& FilePath) {
    filePath = FilePath;
    name = FilePath.substr(FilePath.find_last_of("/\\") + 1);

    std::ifstream ifs (FilePath, std::ios::binary | std::ios::ate );
    std::ifstream::pos_type pos = ifs.tellg();

    if(pos != 0) {
        data = std::vector<char>(pos);
        ifs.seekg(0, std::ios::beg);
        ifs.read(&data[0], pos);
    }

}

void File::UpdatePath(std::string newPath) {
    filePath = newPath;
    name = newPath.substr(newPath.find_last_of("/\\") + 1);
}

