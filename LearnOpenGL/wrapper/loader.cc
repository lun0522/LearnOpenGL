//
//  loader.cc
//
//  Created by Pujun Lun on 3/31/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include "loader.h"

#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include <image/stb_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using std::vector;
using std::runtime_error;
using std::string;
using std::unordered_map;
using glm::vec2;

namespace wrapper {
namespace opengl {
namespace loader {
namespace {

unordered_map<char, Character> LoadCharLib() {
  unordered_map<char, Character> loaded_chars{};

  FT_Library lib;
  if (FT_Init_FreeType(&lib))
    throw runtime_error{"Failed to init FreeType library"};

  FT_Face face;
  if (FT_New_Face(lib, "/Users/lun/Desktop/Code/LearnOpenGL/LearnOpenGL/texture/georgia.ttf", 0, &face))
    throw runtime_error{"Failed to load font"};

  FT_Set_Pixel_Sizes(face, 0, 48);  // set width to 0 for auto adjustment

  // disable byte-alignment for texture storage
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (int c = 0; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      throw runtime_error{"Failed to load glyph"};

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Character ch{
        texture,
        vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        // advance is number of 1/64 pixels
        (GLuint) face->glyph->advance.x >> 6,
    };
    loaded_chars.insert({c, ch});
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  FT_Done_Face(face);
  FT_Done_FreeType(lib);

  return loaded_chars;
}

void LoadImage(const string& path,
               GLenum target,
               bool gamma_correction) {
  int width, height, channel;
  stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channel, 0);
  if (!data) throw runtime_error{"Failed to load texture from " + path};

  GLenum internal_format, format;
  switch (channel) {
    case 1:
      internal_format = format = GL_RED;
      break;
    case 3:
      internal_format = gamma_correction ? GL_SRGB : GL_RGB;
      format = GL_RGB;
      break;
    case 4:
      internal_format = gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
      format = GL_RGBA;
      break;
    default:
      throw runtime_error{"Unknown texture format \
          (channel=" + std::to_string(channel) + ")"};
  }
  // texture target, minmap level, texture format, width, height, always 0,
  //           image format, dtype, data
  glTexImage2D(target, 0, internal_format, width, height, 0,
               format, GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);
}

} /* namespace */

const Character& LoadCharacter(char character) {
  static unordered_map<char, Character> kLoadedCharacter{};
  if (kLoadedCharacter.empty())
    kLoadedCharacter = LoadCharLib();
  return kLoadedCharacter[character];
}

GLuint LoadTexture(const string& path, bool gamma_correction) {
  static unordered_map<string, GLuint> kLoadedTexture{};
  auto loaded = kLoadedTexture.find(path);
  if (loaded == kLoadedTexture.end()) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    LoadImage(path, GL_TEXTURE_2D, gamma_correction);

    // automatically generate all required minmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    loaded = kLoadedTexture.insert({path, texture}).first;
  }
  return loaded->second;
}

GLuint LoadCubemap(const string& directory,
                   const vector<string>& filenames,
                   const bool gamma_correction) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

  for (size_t i = 0; i < filenames.size(); ++i) {
    string filepath = directory + '/' + filenames[i];
    LoadImage(filepath, static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
              gamma_correction);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return texture;
}

} /* namespace loader */
} /* namespace opengl */
} /* namespace wrapper */
