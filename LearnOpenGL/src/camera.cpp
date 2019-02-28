//
//  camera.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

using namespace glm;
using namespace std;

namespace opengl {

Camera::Camera(const vec3& position, const vec3& front, const vec3& up,
               float fov, float near, float far,
               float yaw, float pitch, float sensitivity)
: position_{position}, front_{front}, up_{up},
  fov_{fov}, near_{near}, far_{far},
  yaw_{yaw}, pitch_{pitch}, sensitivity_{sensitivity} {
    UpdateRightVector();
    UpdateViewMatrix();
}

void Camera::UpdateFrontVector() {
    front_ = vec3(cos(radians(pitch_)) * cos(radians(yaw_)),
                  sin(radians(pitch_)),
                  cos(radians(pitch_)) * sin(radians(yaw_)));
}

void Camera::UpdateRightVector() {
    right_ = normalize(cross(front_, up_));
}

void Camera::UpdateViewMatrix() {
    view_ = lookAt(position_, position_ + front_, up_);
}

void Camera::UpdateProjMatrix() {
    if (width_ == 0.0f || height_ == 0.0f)
        throw runtime_error{"Screen size has not been set"};
    proj_ = perspective(radians(fov_), width_ / height_, near_, far_);
}

void Camera::ProcessMouseMove(double x, double y) {
    if (is_first_frame_) {
        last_x_ = x;
        last_y_ = y;
        is_first_frame_ = false;
    }
    float x_offset = (x - last_x_) * sensitivity_;
    float y_offset = (last_y_ - y) * sensitivity_;
    last_x_ = x;
    last_y_ = y;
    yaw_ = mod(yaw_ + x_offset, 360.0f);
    pitch_ = clamp(pitch_ + y_offset, -89.0f, 89.0f);
    
    UpdateFrontVector();
    UpdateRightVector();
    UpdateViewMatrix();
}

void Camera::ProcessMouseScroll(double y, double min_val, double max_val) {
    fov_ = clamp(fov_ + y, min_val, max_val);
    UpdateProjMatrix();
}

void Camera::set_screen_size(int width, int height) {
    width_ = width;
    height_ = height;
    last_x_ = width / 2.0f;
    last_y_ = height / 2.0f;
    UpdateProjMatrix();
}

void Camera::ProcessKeyboardInput(CameraMoveDirection direction,
                                  float distance) {
    switch (direction) {
        case CameraMoveDirection::kUp:
            position_ += front_ * distance;
            break;
        case CameraMoveDirection::kDown:
            position_ -= front_ * distance;
            break;
        case CameraMoveDirection::kLeft:
            position_ += right_ * distance;
            break;
        case CameraMoveDirection::kRight:
            position_ -= right_ * distance;
            break;
        default:
            throw runtime_error{"Invalid direction"};
    }
    UpdateViewMatrix();
}

} /* opengl */
