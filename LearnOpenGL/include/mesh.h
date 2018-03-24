//
//  mesh.h
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/22/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef mesh_h
#define mesh_h

#include <string>
#include <vector>
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"

enum TextureType { DIFFUSE, SPECULAR };

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Texture {
    GLuint id;
    TextureType type;
    std::string path;
};

class Mesh {
    GLuint VAO, VBO, EBO;
    const std::vector<Vertex> vertices;
    const std::vector<GLuint> indices;
    const std::vector<Texture> textures;
public:
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<GLuint>& indices,
         const std::vector<Texture>& textures);
    void draw(const Shader& shader) const;
};

#endif /* mesh_h */
