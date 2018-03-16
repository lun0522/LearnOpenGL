//
//  camera.h
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef camera_h
#define camera_h

#include <glm/glm.hpp>

enum CameraMoveDirection { UP, DOWN, LEFT, RIGHT };

class Camera {
    glm::vec3 position, front, up, right;
    glm::mat4 viewMatrix, projectionMatrix;
    float fov, yaw, pitch, width, height, lastX, lastY, sensitivity;
    bool firstFrame;
    void updateRight();
    void updateViewMatrix();
    void updateProjectionMatrix();
public:
    Camera(glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f),
           float cameraFov = 45.0f,
           float cameraYaw = -90.0f,
           float cameraPitch = 0.0f,
           float mouseSensitivity = 0.05f);
    void setScreenSize(int screenWidth, int screenHeight);
    void processMouseMove(double xPos, double yPos);
    void processMouseScroll(double yOffset);
    void processKeyboardInput(CameraMoveDirection direction, float deltaTime);
    glm::mat4& getViewMatrix();
    glm::mat4& getProjectionMatrix();
};

#endif /* camera_h */
