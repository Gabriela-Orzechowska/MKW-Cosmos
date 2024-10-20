#ifndef UIEDITOR_FILE_H
#define UIEDITOR_FILE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class File {
public:
    File(const std::string& FilePath);
    ~File() = default;

    std::vector<char> GetData() { return data; }
    std::string GetFilePath() { return filePath; }
    std::string GetName() { return name; }
    void UpdatePath(std::string newPath);



private:
    std::string name;
    std::string filePath;
    std::vector<char> data;
};



#endif //UIEDITOR_FILE_H
