//
//  shadow.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 4/3/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>

#include "shadow.hpp"

using glm::vec3;
using glm::mat4;

void Shadow::createMap() {
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // set border "color" to 1.0, so that if object is outside of cross section of
    // camera view frustum, it will not be shadowed
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // by following two lines, we tell OpenGL there will be no color component
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Shadow::Shadow(const int width,
               const int height,
               const vec3& position,
               const vec3& front,
               const vec3& up,
               const float left,
               const float right,
               const float bottom,
               const float top,
               const float near,
               const float far):
width(width), height(height) {
    mat4 projection = glm::ortho(left, right, bottom, top, near, far);
    mat4 view = glm::lookAt(position, position + front, up);
    lightSpace = projection * view;
    createMap();
}

void Shadow::calculate(const Shader& shader,
                       const std::vector<Model>& models,
                       const std::vector<glm::mat4>& modelMatrices,
                       const int prevWidth,
                       const int prevHeight,
                       const GLuint prevFrameBuffer) const {
    if (models.size() != modelMatrices.size())
        throw "Vector size incompatible";
    
    // to avoid peter panning (side effect of setting bias in fragment shader)
    // (sometimes culling front face instead of back face also works)
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    shader.use();
    shader.setMat4("lightSpace", lightSpace);
    for (int i = 0; i < models.size(); ++i) {
        shader.setMat4("model", modelMatrices[i]);
        models[i].draw(shader, 0, true); // no need to load texture!
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);
    glViewport(0, 0, prevWidth, prevHeight);
    glDisable(GL_CULL_FACE);
}

void Shadow::bindShadowMap(const GLuint index) const {
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}

const glm::mat4& Shadow::getLightSpaceMatrix() const {
    return lightSpace;
}
