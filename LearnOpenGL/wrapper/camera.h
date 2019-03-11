//
//  camera.h
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef WRAPPER_OPENGL_CAMERA_H
#define WRAPPER_OPENGL_CAMERA_H

#include <glm/glm.hpp>

namespace wrapper {
namespace opengl {

enum class CameraMoveDirection {
  kUp, kDown, kLeft, kRight,
};

class Camera {
 public:
  Camera(const glm::vec3& position = {0.0f, 0.0f, 0.0f},
         const glm::vec3& front = {0.0f, 0.0f, -1.0f},
         const glm::vec3& up = {0.0f, 1.0f, 0.0f},
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
  const glm::vec3& position()     const { return position_; }
  const glm::vec3& direction()    const { return front_; }
  const glm::mat4& view_matrix()  const { return view_; }
  const glm::mat4& proj_matrix()  const { return proj_; }

 private:
  glm::vec3 position_, front_, up_, right_;
  glm::mat4 view_, proj_;
  float fov_, near_, far_, yaw_, pitch_;
  float width_, height_, last_x_, last_y_, sensitivity_;
  bool is_first_frame_;

  void UpdateFrontVector();
  void UpdateRightVector();
  void UpdateViewMatrix();
  void UpdateProjMatrix();
};

} /* namespace opengl */
} /* namespace wrapper */

#endif /* WRAPPER_OPENGL_CAMERA_H */
