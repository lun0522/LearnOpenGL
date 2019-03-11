//
//  loader.h
//
//  Created by Pujun Lun on 3/31/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef WRAPPER_OPENGL_LOADER_H
#define WRAPPER_OPENGL_LOADER_H

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace wrapper {
namespace opengl {
namespace loader {

struct Character {
  GLuint texture;
  glm::ivec2 size;
  glm::ivec2 bearing;
  GLuint advance;
};

const Character& LoadCharacter(char character);
GLuint LoadTexture(const std::string& path, bool gamma_correction);
GLuint LoadCubemap(const std::string& directory,
                   const std::vector<std::string>& filenames,
                   bool gamma_correction);

} /* namespace loader */
} /* namespace opengl */
} /* namespace wrapper */

#endif /* WRAPPER_OPENGL_LOADER_H */
