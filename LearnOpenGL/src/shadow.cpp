//
//  shadow.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 4/3/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>

#include "shadow.hpp"

using std::vector;
using std::string;
using glm::vec3;
using glm::mat4;
using glm::lookAt;

const int CUBE_MAP_SIDE_LENGTH = 1024;

Shadow::Shadow(const int width,
               const int height,
               const glm::mat4& projection,
               const Shader& shader):
width(width), height(height), projection(projection), shader(shader) {}

OmniShadow::OmniShadow(const float frustumHeight,
                       const glm::mat4& projection,
                       const string& vertPath,
                       const string& geomPath,
                       const string& fragPath):
Shadow(CUBE_MAP_SIDE_LENGTH, CUBE_MAP_SIDE_LENGTH, projection,
Shader(vertPath, fragPath, geomPath)), frustumHeight(frustumHeight) {
    createDepthMap();
    for (int i = 0; i < 6; ++i)
        uniformNames.push_back("lightSpace[" + std::to_string(i) + "]");
    shader.use();
    shader.setFloat("frustumHeight", frustumHeight);
}

OmniShadow OmniShadow::PointLightShadow(const string& vertPath,
                                        const string& geomPath,
                                        const string& fragPath,
                                        const float near,
                                        const float far) {
    mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, near, far);
    return OmniShadow(far - near, projection, vertPath, geomPath, fragPath);
}

UniShadow::UniShadow(const int width,
                     const int height,
                     const glm::mat4& projection,
                     const string& vertPath,
                     const string& fragPath):
Shadow(width, height, projection, Shader(vertPath, fragPath)) {
    createDepthMap();
}

UniShadow UniShadow::DirLightShadow(const string& vertPath,
                                    const string& fragPath,
                                    const int width,
                                    const int height,
                                    const float left,
                                    const float right,
                                    const float bottom,
                                    const float top,
                                    const float near,
                                    const float far) {
    mat4 projection = glm::ortho(left, right, bottom, top, near, far);
    return UniShadow(width, height, projection, vertPath, fragPath);
}

UniShadow UniShadow::SpotLightShadow(const string& vertPath,
                                     const string& fragPath,
                                     const int width,
                                     const int height,
                                     const float fov,
                                     const float near,
                                     const float far) {
    mat4 projection = glm::perspective(glm::radians(fov), (float) width / height, near, far);
    return UniShadow(width, height, projection, vertPath, fragPath);
}

void OmniShadow::createDepthMap() {
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    for (int i = 0; i < 6; ++i)
        // width and height of cubemap must be equal!
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, CUBE_MAP_SIDE_LENGTH,
                     CUBE_MAP_SIDE_LENGTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    // by following two lines, we tell OpenGL there will be no color component
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UniShadow::createDepthMap() {
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

void OmniShadow::moveLight(const glm::vec3 &position) {
    shader.use();
    shader.setVec3("lightPos", position);
    vector<mat4> lightSpace = {
        projection * lookAt(position, position + vec3( 1.0,  0.0,  0.0), vec3( 0.0, -1.0,  0.0)),
        projection * lookAt(position, position + vec3(-1.0,  0.0,  0.0), vec3( 0.0, -1.0,  0.0)),
        projection * lookAt(position, position + vec3( 0.0,  1.0,  0.0), vec3( 0.0,  0.0,  1.0)),
        projection * lookAt(position, position + vec3( 0.0, -1.0,  0.0), vec3( 0.0,  0.0, -1.0)),
        projection * lookAt(position, position + vec3( 0.0,  0.0,  1.0), vec3( 0.0, -1.0,  0.0)),
        projection * lookAt(position, position + vec3( 0.0,  0.0, -1.0), vec3( 0.0, -1.0,  0.0)),
    };
    for (int i = 0; i < 6; ++i)
        shader.setMat4(uniformNames[i], lightSpace[i]);
}

void UniShadow::moveLight(const vec3& position,
                          const vec3& front,
                          const vec3& up) {
    lightSpace = projection * lookAt(position, position + front, up);
    shader.use();
    shader.setMat4("lightSpace", lightSpace);
}

void Shadow::calcShadow(const int prevWidth,
                        const int prevHeight,
                        const GLuint prevFrameBuffer,
                        const std::vector<Model>& models,
                        const std::vector<glm::mat4>& modelMatrices) const {
    if (models.size() != modelMatrices.size())
        throw "Vector size incompatible";
    
    // to avoid peter panning (side effect of setting bias in fragment shader)
    // (sometimes culling front face instead of back face also works)
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    shader.use();
    for (int i = 0; i < models.size(); ++i) {
        shader.setMat4("model", modelMatrices[i]);
        models[i].draw(shader, 0, false); // no need to load texture!
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);
    glViewport(0, 0, prevWidth, prevHeight);
    glDisable(GL_CULL_FACE);
}

const glm::mat4& UniShadow::getLightSpaceMatrix() const {
    return lightSpace;
}

void OmniShadow::bindShadowMap(const GLuint index) const {
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
}

float OmniShadow::getFrustumHeight() {
    return frustumHeight;
}

void UniShadow::bindShadowMap(const GLuint index) const {
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}
