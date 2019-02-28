//
//  shadow.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 4/3/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>

#include "loader.hpp"
#include "shadow.hpp"

using namespace glm;
using namespace std;

namespace opengl {

namespace {
const int kCubeMapSideLength{1024};
const string kOmniShadowVertShader{"shader_omnishadow.vs"};
const string kOmniShadowGeomShader{"shader_omnishadow.gs"};
const string kOmniShadowFragShader{"shader_omnishadow.fs"};
const string kUniShadowVertShader{"shader_unishadow.vs"};
const string kUniShadowFragShader{"shader_unishadow.fs"};
} /* namespace */

Shadow::Shadow(int width,
               int height,
               const mat4& projection,
               const Shader& shader)
: width_{width}, height_{height}, proj_{projection}, shader_{shader} {}

void Shadow::CalculateShadow(int prev_width,
                             int prev_height,
                             GLuint prev_frameBuffer,
                             const vector<Model>& models,
                             const vector<mat4>& model_matrices) const {
    if (models.size() != model_matrices.size())
        throw runtime_error{"Vector size incompatible"};
    
    // to avoid peter panning (side effect of setting bias in fragment shader)
    // (sometimes culling front face instead of back face also works)
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, width_, height_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    shader_.Use();
    for (int i = 0; i < models.size(); ++i) {
        shader_.set_mat4("model", model_matrices[i]);
        models[i].Draw(shader_, 0, false); // no need to load texture!
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, prev_frameBuffer);
    glViewport(0, 0, prev_width, prev_height);
    glDisable(GL_CULL_FACE);
}

OmniShadow::OmniShadow(float frustum_height, const mat4& projection)
: Shadow{kCubeMapSideLength, kCubeMapSideLength, projection,
         {kOmniShadowVertShader, kOmniShadowFragShader, kOmniShadowGeomShader}},
  frustum_height_{frustum_height} {
    CreateDepthMap();
    for (int i = 0; i < 6; ++i)
        uniform_names_.emplace_back("lightSpace[" + to_string(i) + "]");
    shader_.Use();
    shader_.set_float("frustumHeight", frustum_height_);
}

OmniShadow OmniShadow::PointLightShadow(float near, float far) {
    mat4 projection = perspective(radians(90.0f), 1.0f, near, far);
    return OmniShadow{far - near, projection};
}

UniShadow::UniShadow(int width, int height, const mat4& projection)
: Shadow{width, height, projection,
         {kUniShadowVertShader, kUniShadowFragShader}} {
    CreateDepthMap();
}

UniShadow UniShadow::DirLightShadow(int width, int height,
                                    float left, float right,
                                    float bottom, float top,
                                    float near, float far) {
    mat4 projection = ortho(left, right, bottom, top, near, far);
    return UniShadow{width, height, projection};
}

UniShadow UniShadow::SpotLightShadow(int width, int height,
                                     float fov, float near, float far) {
    mat4 projection = perspective(
        radians(fov), (float) width / height, near, far);
    return UniShadow{width, height, projection};
}

void OmniShadow::CreateDepthMap() {
    glGenTextures(1, &depth_map_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map_);
    for (int i = 0; i < 6; ++i)
        // width and height of cubemap must be equal!
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     kCubeMapSideLength, kCubeMapSideLength, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map_, 0);
    // by following two lines, we tell OpenGL there will be no color component
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UniShadow::CreateDepthMap() {
    glGenTextures(1, &depth_map_);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    Loader::set2DTexParameter(GL_NEAREST, GL_CLAMP_TO_BORDER);
    // set border "color" to 1.0, so that if object is outside of cross section
    // of camera view frustum, it will not be shadowed
    float border_color[]{ 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_, 0);
    // by following two lines, we tell OpenGL there will be no color component
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadow::MoveLight(const vec3& position) {
    shader_.Use();
    shader_.set_vec3("lightPos", position);
    vector<mat4> light_spaces = {
        proj_ * lookAt(position, position + vec3{ 1.0,  0.0,  0.0},
                       vec3{ 0.0, -1.0,  0.0}),
        proj_ * lookAt(position, position + vec3{-1.0,  0.0,  0.0},
                       vec3{ 0.0, -1.0,  0.0}),
        proj_ * lookAt(position, position + vec3{ 0.0,  1.0,  0.0},
                       vec3{ 0.0,  0.0,  1.0}),
        proj_ * lookAt(position, position + vec3{ 0.0, -1.0,  0.0},
                       vec3{ 0.0,  0.0, -1.0}),
        proj_ * lookAt(position, position + vec3{ 0.0,  0.0,  1.0},
                       vec3{ 0.0, -1.0,  0.0}),
        proj_ * lookAt(position, position + vec3{ 0.0,  0.0, -1.0},
                       vec3{ 0.0, -1.0,  0.0}),
    };
    for (int i = 0; i < 6; ++i)
        shader_.set_mat4(uniform_names_[i], light_spaces[i]);
}

void UniShadow::MoveLight(const vec3& position,
                          const vec3& front,
                          const vec3& up) {
    light_space_ = proj_ * lookAt(position, position + front, up);
    shader_.Use();
    shader_.set_mat4("lightSpace", light_space_);
}

void OmniShadow::BindShadowMap(GLuint index) const {
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map_);
}

void UniShadow::BindShadowMap(GLuint index) const {
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
}

} /* namespace opengl */
