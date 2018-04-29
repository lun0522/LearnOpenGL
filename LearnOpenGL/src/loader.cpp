//
//  loader.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/31/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include <image/stb_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "loader.hpp"

using std::vector;
using std::string;
using glm::vec2;

namespace Loader {
    std::unordered_map<string, GLuint> loadedTexture;
    std::unordered_map<char, Character> loadedCharacter;
    
    void set2DTexParameter(const GLenum wrapMode, const GLenum interpMode) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpMode);
    }
    
    void loadCharLib() {
        FT_Library lib;
        if (FT_Init_FreeType(&lib)) throw "Failed to init FreeType library";
        
        FT_Face face;
        if (FT_New_Face(lib, "/Users/lun/Desktop/Code/LearnOpenGL/LearnOpenGL/src/texture/georgia.ttf", 0, &face)) throw "Failed to load font";
        
        FT_Set_Pixel_Sizes(face, 0, 48); // set width to 0 for auto adjustment
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment for texture storage
        for (int c = 0; c < 128; ++c) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) throw "Failed to load glyph";
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            set2DTexParameter(GL_CLAMP_TO_EDGE, GL_LINEAR);
            Character ch = {
                texture,
                vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (GLuint) face->glyph->advance.x >> 6, // advance is number of 1/64 pixels
            };
            loadedCharacter.insert({ c, ch });
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        
        FT_Done_Face(face);
        FT_Done_FreeType(lib);
    }
    
    Character& loadCharacter(const char character) {
        if (loadedCharacter.empty()) loadCharLib();
        auto loaded = loadedCharacter.find(character);
        if (loaded == loadedCharacter.end()) throw "Failed to retrieve character";
        else return loaded->second;
    }
    
    GLuint loadImage(const string& path,
                     const GLenum target,
                     const bool shouldBind,
                     const bool gammaCorrection) {
        int width, height, channel;
        stbi_uc *data = stbi_load(path.c_str(), &width, &height, &channel, 0);
        if (!data) throw "Failed to load texture from " + path;
        
        GLenum internalFormat, format;
        switch (channel) {
            case 1:
                internalFormat = format = GL_RED;
                break;
            case 3:
                internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
                format = GL_RGB;
                break;
            case 4:
                internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
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
        glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        stbi_image_free(data);
        return texture;
    }
    
    GLuint loadTexture(const string& path, const bool gammaCorrection) {
        auto loaded = loadedTexture.find(path);
        if (loaded == loadedTexture.end()) {
            GLuint texture = loadImage(path, GL_TEXTURE_2D, true, gammaCorrection);
            
            glGenerateMipmap(GL_TEXTURE_2D); // automatically generate all required minmaps
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            loadedTexture.insert({ path, texture });
            return texture;
        } else {
            return loaded->second;
        }
    }
    
    GLuint loadCubemap(const string& path,
                       const vector<string>& filename,
                       const bool gammaCorrection) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        
        for (int i = 0; i < filename.size(); ++i) {
            string filepath = path + '/' + filename[i];
            loadImage(filepath, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, false, gammaCorrection);
        }
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return texture;
    }
}
