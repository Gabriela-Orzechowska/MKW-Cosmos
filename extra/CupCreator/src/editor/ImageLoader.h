//
// Created by Gabi on 7/19/2024.
//

#ifndef COSMOSCUPCREATOR_IMAGELOADER_H
#define COSMOSCUPCREATOR_IMAGELOADER_H

#include <glad/glad.h>

class ImageLoader {
public:
    static bool LoadTextureFromFile(const char* filename, GLuint* texture, int* width, int* height);
};


#endif //COSMOSCUPCREATOR_IMAGELOADER_H
