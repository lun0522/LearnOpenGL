//
//  mesh.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/22/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include "mesh.hpp"

using namespace glm;
using namespace std;

namespace opengl {

namespace {

void BindTexture(const vector<Texture> textures,
                 const Shader& shader,
                 GLuint tex_offset) {
    int diff_idx = 0, spec_idx = 0, refl_idx = 0;
    for (int i = 0; i < textures.size(); ++i) {
        string name;
        switch (textures[i].type) {
        case TextureType::kDiffuse:
            name = "material.diffuse" + to_string(diff_idx++);
            break;
        case TextureType::kSpecular:
            name = "material.specular" + to_string(spec_idx++);
            break;
        case TextureType::kReflection:
            name = "material.reflection" + to_string(refl_idx++);
            break;
        default:
            throw runtime_error{"Unknown texture type"};
        }
        GLuint tex_idx = tex_offset + i;
        glActiveTexture(GL_TEXTURE0 + tex_idx);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        shader.set_int(name, tex_idx);
    }
}

} /* namespace */

Mesh::Mesh(const vector<Vertex>& vertices,
           const vector<GLuint>& indices,
           const vector<Texture>& textures)
: vertices_{vertices}, indices_{indices}, textures_{textures} {
    // VAO
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    
    // VBO
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    // GL_STATIC_DRAW: for data that does not change
    // GL_DYNAMIC_DRAW: for data that changes a lot
    // GL_STREAM_DRAW: for data that changes every time when it is drawn
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);
    
    // EBO
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 indices.data(), GL_STATIC_DRAW);
    
    // enable attributes
    GLsizei stride = 8 * sizeof(GLfloat);
    // which vertex attribute to configure (location)
    // size of vertex attribute
    // data type
    // whether we want the data to be normalized
    // space between consecutive vertex attribute sets (stride)
    //     (can set to be 0 if tightly packed)
    // offset of the position data begins in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                          (void *)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void *)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void *)offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw(const Shader& shader,
                GLuint tex_offset,
                bool load_texture) const {
    if (load_texture) BindTexture(textures_, shader, tex_offset);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::DrawInstanced(const Shader& shader,
                         GLuint amount,
                         GLuint tex_offset,
                         bool load_texture) const {
    if (load_texture) BindTexture(textures_, shader, tex_offset);
    glBindVertexArray(vao_);
    glDrawElementsInstanced(
        GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);
}

void Mesh::AppendData(const function<void ()>& func) const {
    glBindVertexArray(vao_);
    func();
    glBindVertexArray(0);
}

} /* opengl */
