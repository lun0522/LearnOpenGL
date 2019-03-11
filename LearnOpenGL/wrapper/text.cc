//
//  text.cc
//
//  Created by Pujun Lun on 4/15/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include "text.h"

#include "loader.h"

using glm::vec2;

namespace wrapper {
namespace opengl {

Text::Text() {
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, 6 * (2 + 2) * sizeof(float), NULL,
               GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Text::renderText(const Shader& shader, const std::string& text,
                      float x, float y, float scale, const glm::vec3& color) {
  shader.Use();
  glActiveTexture(GL_TEXTURE0);
  shader.set_int("text", 0);
  shader.set_vec3("color", color);
  glBindVertexArray(vao_);

  for (auto c : text) {
    loader::Character ch = loader::LoadCharacter(c);

    float x_pos = x + ch.bearing.x * scale;
    float y_pos = y - (ch.size.y - ch.bearing.y) * scale;
    vec2 size = vec2(ch.size) * scale;
    x += ch.advance * scale;

    float vertex_attrib[6][4]{
        {x_pos,          y_pos + size.y, 0.0, 0.0},
        {x_pos,          y_pos,          0.0, 1.0},
        {x_pos + size.x, y_pos,          1.0, 1.0},
        {x_pos,          y_pos + size.y, 0.0, 0.0},
        {x_pos + size.x, y_pos,          1.0, 1.0},
        {x_pos + size.x, y_pos + size.y, 1.0, 0.0},
    };

    glBindTexture(GL_TEXTURE_2D, ch.texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_attrib), vertex_attrib);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

} /* namespace opengl */
} /* namespace wrapper */
