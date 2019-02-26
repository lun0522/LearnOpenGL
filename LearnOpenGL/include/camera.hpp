//
//  camera.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef camera_hpp
#define camera_hpp

#include <glm/glm.hpp>

namespace opengl {

using glm::vec3;
using glm::mat4;

enum class CameraMoveDirection {
    kUp, kDown, kLeft, kRight,
};

class Camera {
    vec3 position_, front_, up_, right_;
    mat4 view_, proj_;
    float fov_, near_, far_, yaw_, pitch_;
    float width_, height_, last_x_, last_y_, sensitivity_;
    bool is_first_frame_;
    void UpdateFrontVector();
    void UpdateRightVector();
    void UpdateViewMatrix();
    void UpdateProjMatrix();
    
public:
    Camera(const vec3& position = {0.0f, 0.0f, 0.0f},
           const vec3& front = {0.0f, 0.0f, -1.0f},
           const vec3& up = {0.0f, 1.0f, 0.0f},
           float fov = 45.0f,
           float near = 0.1f,
           float far = 100.0f,
           float yaw = -90.0f,
           float pitch = 0.0f,
           float sensitivity = 0.05f);
    void ProcessMouseMove(double x, double y);
    void ProcessMouseScroll(double y, double min_val, double max_val);
    void ProcessKeyboardInput(CameraMoveDirection direction, float distance);
    void set_screen_size(int width, int height);
    const vec3& position()      const { return position_; }
    const vec3& direction()     const { return front_; }
    const mat4& view_matrix()   const { return view_; }
    const mat4& proj_matrix()   const { return proj_; }
};

} /* namespace opengl */

#endif /* camera_hpp */
