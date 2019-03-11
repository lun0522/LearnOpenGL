//
//  mesh.h
//
//  Created by Pujun Lun on 3/22/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef WRAPPER_OPENGL_MESH_H
#define WRAPPER_OPENGL_MESH_H

#include <functional>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

namespace wrapper {
namespace opengl {

enum class TextureType {
  kDiffuse, kSpecular, kReflection,
};

struct Texture {
  GLuint id;
  TextureType type;
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coord;
};

class Mesh {
 public:
  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<GLuint>& indices,
       const std::vector<Texture>& textures);
  void Draw(const Shader& shader,
            GLuint tex_offset,
            bool load_texture) const;
  void DrawInstanced(const Shader& shader,
                     GLuint amount,
                     GLuint tex_offset,
                     bool load_texture) const;
  void AppendData(const std::function<void ()>& func) const;

 private:
  GLuint vao_, vbo_, ebo_;
  const std::vector<Vertex> vertices_;
  const std::vector<GLuint> indices_;
  const std::vector<Texture> textures_;
};

} /* namespace opengl */
} /* namespace wrapper */

#endif /* WRAPPER_OPENGL_MESH_H */
