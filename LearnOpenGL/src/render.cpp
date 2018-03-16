//
//  render.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render.h"

using std::string;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastFrame = 0.0f;
float intensity = 1.0f;

typedef struct ScreenSize {
    int width;
    int height;
} ScreenSize;

ScreenSize getScreenSize(GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return { width, height };
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseMoveCallback(GLFWwindow *window, double xPos, double yPos) {
    camera.processMouseMove(xPos, yPos);
}

void mouseScrollCallback(GLFWwindow *window, double xPos, double yPos) {
    camera.processMouseScroll(yPos);
}

void Render::processKeyboardInput() {
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.processKeyboardInput(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.processKeyboardInput(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.processKeyboardInput(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.processKeyboardInput(RIGHT, deltaTime);
    
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        intensity += 0.02f;
        if (intensity > 1.0f) intensity = 1.0f;
    } else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        intensity -= 0.02f;
        if (intensity < 0.0f) intensity = 0.0f;
    }
}

GLuint loadTexture(string path, GLenum format) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, channel;
    stbi_uc *data = stbi_load(path.c_str(), &width, &height, &channel, 0);
    if (!data) throw "Cannot load texture from " + path;
    
    // texture target, minmap level, texture format, width, height, always 0, image format, dtype, data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // automatically generate all required minmaps
    
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture;
}

Render::Render() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // ------------------------------------
    // window
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) throw "Failed to create window";
    
    glfwMakeContextCurrent(window);
    // called when window is resized by the user
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // hide mouse and capture input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    
    // ------------------------------------
    // GLAD (function pointer loader)
    // do this after context is created, and before calling any OpenGL function!
    
    if (!gladLoadGL()) throw "Failed to init GLAD";
    
    // screen size is different from the input width and height on retina screen
    ScreenSize size = getScreenSize(window);
    glViewport(0, 0, size.width, size.height); // specify render area
    camera.setScreenSize(size.width, size.height);
}

void Render::renderLoop() {
    // ------------------------------------
    // shader program
    
    string path = "/Users/lun/Desktop/Code/LearnOpenGL/LearnOpenGL/src/";
    Shader lightShader = Shader(path + "shaders/shader_light.vs", path + "shaders/shader_light.fs");
    Shader rotatorShader = Shader(path + "shaders/shader_rotator.vs", path + "shaders/shader_rotator.fs");
    
    // ------------------------------------
    // VAO, VBO and EBO
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    // vertex array object, vertex buffer object, element buffer array
    // VAO stores attribute pointers (interpreted from VBO) and EBO (records any glBindBuffer call
    // with target GL_ELEMENT_ARRAY_BUFFER)
    GLuint rotatorVAO, lightVAO, VBO;
    
    glGenVertexArrays(1, &rotatorVAO);
    glBindVertexArray(rotatorVAO);
    
    glGenBuffers(1, &VBO); // request 1 buffer object, and store in VBO (can pass an array)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // GL_STATIC_DRAW: for data that does not change
    // GL_DYNAMIC_DRAW: for data that changes a lot
    // GL_STREAM_DRAW: for data that changes every time when it is drawn
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // tell OpenGL how to interpret the vertex data
    // which vertex attribute to configure (location = 0)
    // size of vertex attribute
    // data type
    // whether e want the data to be normalized
    // space between consecutive vertex attribute sets (stride) (can set to be 0 if tightly packed)
    // offset of the position data begins in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0); // do this before EBO unbinds! otherwise the unbinding is also recorded
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    
    // ------------------------------------
    // coordinates
    
    // object to world
    glm::mat4 light = glm::mat4(1.0f);
    light = glm::translate(light, glm::vec3(0.5f, 0.5f, 1.5f));
    light = glm::scale(light, glm::vec3(0.2f));
    glm::mat4 rotator = glm::mat4(1.0f);
    
    // ------------------------------------
    // texture
    
    // OpenGL expects 0.0 of y axis lays on the top, so flip the image
    stbi_set_flip_vertically_on_load(true);
    GLuint texture1 = loadTexture(path + "texture/awesomeface.png", GL_RGBA);
    GLuint texture2 = loadTexture(path + "texture/container.jpg", GL_RGB);
    
    // ------------------------------------
    // draw
    
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    
    glm::vec3 lightColor = glm::vec3(1.0f);
    glm::vec3 rotatorColor = glm::vec3(1.0f, 0.5f, 0.31f);
    
    lightShader.use();
    lightShader.setVector("lightColor", glm::value_ptr(lightColor));
    lightShader.setMatrix("model", glm::value_ptr(light));
    
    rotatorShader.use();
    rotatorShader.setInt("texture1", 0); // tell OpenGL which sampler corresponds to which texture
    rotatorShader.setInt("texture2", 1);
    rotatorShader.setVector("lightColor", glm::value_ptr(lightColor));
    rotatorShader.setVector("objectColor", glm::value_ptr(rotatorColor));
    
    while (!glfwWindowShouldClose(window)) { // until user hit close
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        processKeyboardInput();
        
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        
        lightShader.use();
        lightShader.setFloat("lightIntensity", intensity);
        lightShader.setMatrix("view", glm::value_ptr(view));
        lightShader.setMatrix("projection", glm::value_ptr(projection));
        glBindVertexArray(rotatorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        rotatorShader.use();
        rotator = glm::rotate(rotator, glm::radians(1.0f), glm::vec3(1.0f, 0.5f, 0.0f));
        rotatorShader.setMatrix("model", glm::value_ptr(rotator));
        rotatorShader.setMatrix("view", glm::value_ptr(view));
        rotatorShader.setMatrix("projection", glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glfwSwapBuffers(window); // use color buffer to draw
        glfwPollEvents(); // check events (keyboard, mouse, ...)
    }
    
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &rotatorVAO);
}
