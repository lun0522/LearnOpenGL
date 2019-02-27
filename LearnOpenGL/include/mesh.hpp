//
//  mesh.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/22/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef mesh_hpp
#define mesh_hpp

#include <functional>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"

namespace opengl {

using glm::vec2;
using glm::vec3;
using std::function;
using std::string;
using std::vector;

enum class TextureType {
    kDiffuse, kSpecular, kReflection,
};

struct Texture {
    GLuint id;
    TextureType type;
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 tex_coord;
};

class Mesh {
    GLuint vao_, vbo_, ebo_;
    const vector<Vertex> vertices_;
    const vector<GLuint> indices_;
    const vector<Texture> textures_;
    
public:
    Mesh(const vector<Vertex>& vertices,
         const vector<GLuint>& indices,
         const vector<Texture>& textures);
    void Draw(const Shader& shader,
              GLuint tex_offset,
              bool load_texture) const;
    void DrawInstanced(const Shader& shader,
                       GLuint amount,
                       GLuint tex_offset,
                       bool load_texture) const;
    void AppendData(const function<void ()>& func) const;
};

} /* opengl */

#endif /* mesh_hpp */
