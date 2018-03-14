//
//  camera.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraFront, glm::vec3 cameraUp,
               float cameraFov, float cameraYaw, float cameraPitch, float mouseSensitivity) {
    position = cameraPosition;
    front = cameraFront;
    up = cameraUp;
    right = glm::normalize(glm::cross(up, front));
    updateViewMatrix();
    fov = cameraFov;
    yaw = cameraYaw;
    pitch = cameraPitch;
    sensitivity = mouseSensitivity;
    firstFrame = true;
}

void Camera::setScreenSize(int screenWidth, int screenHeight) {
    width = screenWidth;
    height = screenHeight;
    lastX = width / 2.0f;
    lastY = height / 2.0f;
    updateProjectionMatrix();
}

void Camera::processMouseMove(double xPos, double yPos) {
    if (firstFrame) {
        lastX = xPos;
        lastY = yPos;
        firstFrame = false;
    }
    
    float xOffset = (xPos - lastX) * sensitivity;
    float yOffset = (yPos - lastY) * sensitivity;
    lastX = xPos;
    lastY = yPos;
    
    yaw += xOffset;
    if (yaw >= 360.0f) yaw -= 360.0f;
    
    pitch -= yOffset;
    if (pitch > 89.0f) pitch = 89.0f;
    else if (pitch < -89.0f) pitch = -89.0f;
    
    front = glm::vec3(cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
                      sin(glm::radians(pitch)),
                      cos(glm::radians(pitch)) * sin(glm::radians(yaw)));
    updateViewMatrix();
}

void Camera::processMouseScroll(double yOffset) {
    fov += yOffset;
    if (fov < 1.0f) fov = 1.0f;
    else if (fov > 45.0f) fov = 45.0f;
    updateProjectionMatrix();
}

void Camera::processKeyboardInput(CameraMoveDirection direction, float deltaTime) {
    float speed = deltaTime * 2.0f;
    switch (direction) {
        case UP:
            position += front * speed;
            break;
        case DOWN:
            position -= front * speed;
            break;
        case LEFT:
            position += right * speed;
            break;
        case RIGHT:
            position -= right * speed;
            break;
        default:
            throw "Invalid direction";
    }
    updateViewMatrix();
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, position + front, up);
}

glm::mat4& Camera::getViewMatrix() {
    return viewMatrix;
}

void Camera::updateProjectionMatrix() {
    if (width == 0.0f || height == 0.0f) throw "Screen size has not been set";
    projectionMatrix = glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
}

glm::mat4& Camera::getProjectionMatrix() {
    if (width == 0.0f || height == 0.0f) throw "Screen size has not been set";
    return projectionMatrix;
}
