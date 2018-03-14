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

enum CameraMoveDirection { up, down, left, right };

class Camera {
    bool firstFrame;
    float fov;
    float width, height;
    float yaw, pitch;
    float lastX, lastY;
    float mouseSensitivity;
    glm::vec3 cameraPosition;
    glm::vec3 cameraFront;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    void updateViewMatrix();
    void updateProjectionMatrix();
public:
    Camera(int screenWidth, int screenHeight);
    void processMouseMove(double xPos, double yPos);
    void processMouseScroll(double yOffset);
    void processKeyboardInput(CameraMoveDirection direction, float deltaTime);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
};

#endif /* camera_h */
