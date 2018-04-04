//
//  shadow.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 4/3/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef shadow_hpp
#define shadow_hpp

#include <vector>
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "model.hpp"

class Shadow {
    GLuint FBO;
    GLuint depthMap;
    float width, height;
    glm::mat4 lightSpace;
    void createMap();
public:
    Shadow(const int width,
           const int height,
           const glm::vec3& position,
           const glm::vec3& front,
           const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
           const float left = -20.0f,
           const float right = 20.0f,
           const float bottom = -20.0f,
           const float top = 20.0f,
           const float near = 0.1f,
           const float far = 100.0f);
    void calculate(const Shader& shader,
                   const std::vector<Model>& models,
                   const std::vector<glm::mat4>& modelMatrices,
                   const int prevWidth,
                   const int prevHeight,
                   const GLuint prevFrameBuffer) const;
    void bindShadowMap(const GLuint index) const;
    const glm::mat4& getLightSpaceMatrix() const;
};

#endif /* shadow_hpp */
