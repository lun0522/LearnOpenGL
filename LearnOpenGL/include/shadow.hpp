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
#include <string>
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "model.hpp"

class Shadow {
    Shadow();
protected:
    GLuint FBO;
    GLuint depthMap;
    float width, height;
    Shader shader;
    glm::mat4 projection;
    Shadow(const int width,
           const int height,
           const glm::mat4& projection,
           const Shader& shader);
    virtual void createDepthMap() = 0;
    virtual void bindShadowMap(const GLuint index) const = 0;
public:
    void calcShadow(const int prevWidth,
                    const int prevHeight,
                    const GLuint prevFrameBuffer,
                    const std::vector<Model>& models,
                    const std::vector<glm::mat4>& modelMatrices) const;
};

class OmniShadow : public Shadow {
    float frustumHeight;
    std::vector<std::string> uniformNames;
    OmniShadow();
    OmniShadow(const float frustumHeight,
               const glm::mat4& projection,
               const std::string& vertPath,
               const std::string& geomPath,
               const std::string& fragPath);
    void createDepthMap();
public:
    static OmniShadow PointLightShadow(const std::string& vertPath,
                                       const std::string& geomPath,
                                       const std::string& fragPath,
                                       const float near = 0.1f,
                                       const float far = 100.0f);
    void moveLight(const glm::vec3& position);
    void bindShadowMap(const GLuint index) const;
    float getFrustumHeight();
};

class UniShadow : public Shadow {
    glm::mat4 lightSpace;
    UniShadow();
    UniShadow(const int width,
              const int height,
              const glm::mat4& projection,
              const std::string& vertPath,
              const std::string& fragPath);
    void createDepthMap();
public:
    static UniShadow DirLightShadow(const std::string& vertPath,
                                    const std::string& fragPath,
                                    const int width,
                                    const int height,
                                    const float left = -10.0f,
                                    const float right = 10.0f,
                                    const float bottom = -10.0f,
                                    const float top = 10.0f,
                                    const float near = 0.1f,
                                    const float far = 100.0f);
    static UniShadow SpotLightShadow(const std::string& vertPath,
                                     const std::string& fragPath,
                                     const int width,
                                     const int height,
                                     const float fov = 45.0f,
                                     const float near = 0.1f,
                                     const float far = 50.0f);
    void moveLight(const glm::vec3& position,
                   const glm::vec3& front,
                   const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4& getLightSpaceMatrix() const;
    void bindShadowMap(const GLuint index) const;
};

#endif /* shadow_hpp */
