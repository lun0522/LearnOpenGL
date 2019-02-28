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
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "model.hpp"

namespace opengl {

class Shadow {
  public:
    void CalculateShadow(int prev_width,
                         int prev_height,
                         GLuint prev_frameBuffer,
                         const std::vector<Model>& models,
                         const std::vector<glm::mat4>& model_matrices) const;
    
  protected:
    GLuint fbo_;
    GLuint depth_map_;
    int width_, height_;
    Shader shader_;
    glm::mat4 proj_;
    
    Shadow(int width,
           int height,
           const glm::mat4& projection,
           const Shader& shader);
    virtual void CreateDepthMap() = 0;
    virtual void BindShadowMap(GLuint index) const = 0;
};

class OmniShadow : public Shadow {
  public:
    static OmniShadow PointLightShadow(float near = 0.1f,
                                       float far = 100.0f);
    void MoveLight(const glm::vec3& position);
    void BindShadowMap(GLuint index) const;
    float frustum_height() const { return frustum_height_; }
    
  private:
    float frustum_height_;
    std::vector<std::string> uniform_names_;
    OmniShadow(float frustum_height,
               const glm::mat4& projection);
    void CreateDepthMap();
};

class UniShadow : public Shadow {
  public:
    static UniShadow DirLightShadow(int width,
                                    int height,
                                    float left = -10.0f,
                                    float right = 10.0f,
                                    float bottom = -10.0f,
                                    float top = 10.0f,
                                    float near = 0.1f,
                                    float far = 100.0f);
    static UniShadow SpotLightShadow(int width,
                                     int height,
                                     float fov = 45.0f,
                                     float near = 0.1f,
                                     float far = 50.0f);
    void MoveLight(const glm::vec3& position,
                   const glm::vec3& front,
                   const glm::vec3& up = {0.0f, 1.0f, 0.0f});
    void BindShadowMap(GLuint index) const;
    const glm::mat4& light_space() const { return light_space_; }
    
  private:
    glm::mat4 light_space_;
    UniShadow(int width,
              int height,
              const glm::mat4& projection);
    void CreateDepthMap();
};

} /* namespace opengl */

#endif /* shadow_hpp */
