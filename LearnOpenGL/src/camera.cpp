//
//  camera.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

Camera::Camera(int screenWidth, int screenHeight) {
    firstFrame = true;
    fov = 45.0f;
    width = screenWidth;
    height = screenHeight;
    yaw = -90.0f;
    pitch = 0.0f;
    lastX = width / 2.0f;
    lastY = height / 2.0f;
    mouseSensitivity = 0.05f;
    cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera::processMouseMove(double xPos, double yPos) {
    if (firstFrame) {
        lastX = xPos;
        lastY = yPos;
        firstFrame = false;
    }
    
    float xOffset = (xPos - lastX) * mouseSensitivity;
    float yOffset = (yPos - lastY) * mouseSensitivity;
    lastX = xPos;
    lastY = yPos;
    
    yaw += xOffset;
    pitch -= yOffset;
    if (pitch > 89.0f) pitch = 89.0f;
    else if (pitch < -89.0f) pitch = -89.0f;
    
    cameraFront = glm::vec3(cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
                            sin(glm::radians(pitch)),
                            cos(glm::radians(pitch)) * sin(glm::radians(yaw)));
    updateViewMatrix();
}

void Camera::processMouseScroll(double yOffset) {
    fov -= yOffset;
    if (fov < 1.0f) fov = 1.0f;
    else if (fov > 45.0f) fov = 45.0f;
    updateProjectionMatrix();
}

void Camera::processKeyboardInput(CameraMoveDirection direction, float deltaTime) {
    float cameraSpeed = deltaTime * 2.0f;
    switch (direction) {
        case up:
            cameraPosition += cameraFront * cameraSpeed;
            break;
        case down:
            cameraPosition -= cameraFront * cameraSpeed;
            break;
        case left:
            cameraPosition += cameraRight * cameraSpeed;
            break;
        case right:
            cameraPosition -= cameraRight * cameraSpeed;
            break;
        default:
            throw "Invalid direction";
    }
    updateViewMatrix();
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

glm::mat4 Camera::getViewMatrix() {
    return viewMatrix;
}

void Camera::updateProjectionMatrix() {
    projectionMatrix = glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}
