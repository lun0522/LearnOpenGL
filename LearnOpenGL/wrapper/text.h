//
//  text.h
//
//  Created by Pujun Lun on 4/15/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef WRAPPER_OPENGL_TEXT_H
#define WRAPPER_OPENGL_TEXT_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

namespace wrapper {
namespace opengl {

class Text {
 public:
  Text();
  void renderText(const Shader& shader, const std::string& text,
                  float x, float y, float scale, const glm::vec3& color);

 private:
  GLuint vao_, vbo_;
};

} /* namespace opengl */
} /* namespace wrapper */

#endif /* WRAPPER_OPENGL_TEXT_H */
