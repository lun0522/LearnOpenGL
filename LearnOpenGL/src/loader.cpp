//
//  loader.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/31/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "loader.hpp"

using std::vector;
using std::string;

GLuint loadImage(const string& path, GLenum target, bool shouldBind) {
    int width, height, channel;
    stbi_uc *data = stbi_load(path.c_str(), &width, &height, &channel, 0);
    if (!data) throw "Failed to load texture from " + path;
    
    GLenum format;
    switch (channel) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            throw "Unknown texture format (channel=" + std::to_string(channel) + ")";
    }
    
    GLuint texture = 0;
    if (shouldBind) {
        glGenTextures(1, &texture);
        glBindTexture(target, texture);
    }
    
    // texture target, minmap level, texture format, width, height, always 0, image format, dtype, data
    glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    return texture;
}

GLuint Loader::loadTexture(const string& path) {
    GLuint texture = loadImage(path, GL_TEXTURE_2D, true);
    
    glGenerateMipmap(GL_TEXTURE_2D); // automatically generate all required minmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

GLuint Loader::loadCubemap(const string& path, const vector<string>& filename) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    for (int i = 0; i < filename.size(); ++i) {
        string filepath = path + '/' + filename[i];
        loadImage(filepath, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, false);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}


