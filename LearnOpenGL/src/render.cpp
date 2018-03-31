//
//  render.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "loader.hpp"
#include "model.hpp"
#include "render.hpp"

using std::string;

typedef struct ScreenSize {
    int width;
    int height;
} ScreenSize;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int NUM_POINT_LIGHTS = 3;

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastFrame = 0.0f;
ScreenSize originalSize{0, 0};
ScreenSize currentSize{0, 0};

void updateScreenSize(GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    currentSize.width = width;
    currentSize.height = height;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    currentSize.width = width;
    currentSize.height = height;
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
    updateScreenSize(window);
    originalSize = currentSize;
    glViewport(0, 0, currentSize.width, currentSize.height); // specify render area
    camera.setScreenSize(currentSize.width, currentSize.height);
}

void Render::renderLoop() {
    // ------------------------------------
    // shader program
    
    string path = "/Users/lun/Desktop/Code/LearnOpenGL/LearnOpenGL/src/";
    Shader lampShader(path + "shaders/shader_lamp.vs",
                      path + "shaders/shader_lamp.fs");
    Shader glassShader(path + "shaders/shader_glass.vs",
                       path + "shaders/shader_glass.fs");
    Shader objectShader(path + "shaders/shader_object.vs",
                        path + "shaders/shader_object.fs");
    Shader skyboxShader(path + "shaders/shader_skybox.vs",
                        path + "shaders/shader_skybox.fs");
    Shader screenShader(path + "shaders/shader_screen.vs",
                        path + "shaders/shader_screen.fs");
    
    
    // ------------------------------------
    // models
    
    Model lamp(path + "texture/cube.obj");
    Model glass(path + "texture/glass.obj");
    Model skybox(path + "texture/skybox.obj");
    Model screen(path + "texture/screen.obj");
    Model object(path + "texture/nanosuit/nanosuit.obj", path + "texture/nanosuit");
    
    std::vector<string> boxfaces {
        "right.tga",
        "left.tga",
        "top.tga",
        "bottom.tga",
        "back.tga",
        "front.tga",
    };
    GLuint skyboxTex = Loader::loadCubemap(path + "texture/tidepool", boxfaces);
    GLuint glassTex = Loader::loadTexture(path + "texture/glass.png");
    
    
    // ------------------------------------
    // extra framebuffer
    
    // framebuffer holds color, depth (optinal) and stencil (option) buffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // framebuffer should have at least one color attachment (can have multiple)
    GLuint texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentSize.width, currentSize.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // data is set to NULL, because later contents will be rendered to it
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // no need for mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    
    // render buffer is preferred when we don't need to sample data (depth/stencil)
    // otherwise we use texture (color)
    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    // enable to store both depth and stencil buffer (also only allocate memory, without content)
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, currentSize.width, currentSize.height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw "Frame buffer incomplete";
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // ------------------------------------
    // parameters
    
    glm::mat4 objectModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
    objectModel = glm::scale(objectModel, glm::vec3(1.0f) * 0.5f);
    glm::mat4 glassModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 6.0f));
    
    glm::vec3 lightColor(0.6f);
    glm::vec3 ambientColor = lightColor * 0.2f;
    glm::vec3 diffuseColor = lightColor * 0.8f;
    
    glm::vec3 lampPos[NUM_POINT_LIGHTS] = {
        glm::vec3( 0.0f, -3.0f,  4.0f),
        glm::vec3(-4.0f, -1.0f, -3.0f),
        glm::vec3( 4.0f,  2.0f, -2.0f)
    };
    
    glm::vec3 lampColor[NUM_POINT_LIGHTS] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    
    objectShader.use();
    objectShader.setFloat("material.shininess", 0.2f);
    
    // directional light
    glm::vec3 dirLight(0.0f, -1.0f, 1.0f);
    objectShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
    objectShader.setVec3("dirLight.diffuse", ambientColor);
    objectShader.setVec3("dirLight.specular", lightColor);
    
    // point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
        string light = "pointLights[" + std::to_string(i) + "]";
        objectShader.setFloat(light + ".constant", 1.0f);
        objectShader.setFloat(light + ".linear", 0.09f);
        objectShader.setFloat(light + ".quadratic", 0.032f);
        glm::vec3 ambient = ambientColor * lampColor[i];
        glm::vec3 diffuse = diffuseColor * lampColor[i];
        glm::vec3 specular = lightColor * lampColor[i];
        objectShader.setVec3(light + ".ambient", ambient);
        objectShader.setVec3(light + ".diffuse", diffuse);
        objectShader.setVec3(light + ".specular", specular);
    }
    
    // spot light
    objectShader.setVec3("spotLight.position", 0.0f, 0.0f, 0.0f);
    objectShader.setVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
    objectShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
    objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    objectShader.setFloat("spotLight.constant", 1.0f);
    objectShader.setFloat("spotLight.linear", 0.09f);
    objectShader.setFloat("spotLight.quadratic", 0.032f);
    objectShader.setVec3("spotLight.ambient", ambientColor);
    objectShader.setVec3("spotLight.diffuse", diffuseColor);
    objectShader.setVec3("spotLight.specular", lightColor);
    
    glassShader.use();
    glassShader.setMat4("model", glassModel);
    
    
    // ------------------------------------
    // draw
    
    while (!glfwWindowShouldClose(window)) { // until user hit close
        // render to texture of customized framebuffer first
        // later use this texture for default framebuffer
        // always render in orignal size, let customized framebuffer deal with resizing
        processKeyboardInput();
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, originalSize.width, originalSize.height);
        
        // for closed shapes, omit clockwise triangles
        // glass should be double-sided (see vertices of glass)
        // otherwise we have to disable face culling when drawing it
        glEnable(GL_CULL_FACE);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        // actions to take when:
        // stencil test fail
        // stencil test pass && depth test fail
        // stencil test pass && depth test pass
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        
        glEnable(GL_BLEND);
        // src * alpha + dst * (1.0 - alpha)
        // dst refers to value that already exists in color buffer
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        
        
        // ------------------------------------
        // render lamps with outlines
        
        // enable any of ragments of lights (lamps) to update stencil buffer with 1
        // so that later we know where we should not draw outlines
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // let stencil test always pass
        
        lampShader.use();
        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);
        
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            glm::mat4 lampModel = glm::translate(glm::mat4(1.0f), lampPos[i]);
            lampModel = glm::scale(lampModel, glm::vec3(0.8f));
            lampShader.setMat4("model", lampModel);
            lampShader.setVec3("lightColor", lampColor[i]);
            lamp.draw(lampShader);
        }
        
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // disable updating stencil buffer (imagine when lamps overlap)
        
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            glm::mat4 lampModel = glm::translate(glm::mat4(1.0f), lampPos[i]);
            lampModel = glm::scale(lampModel, glm::vec3(0.85f));
            lampShader.setMat4("model", lampModel);
            lampShader.setVec3("lightColor", 1.0f, 1.0f, 0.0f);
            lamp.draw(lampShader);
        }
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        
        
        // ------------------------------------
        // render object
        
        objectShader.use();
        objectModel = glm::rotate(objectModel, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view * objectModel)));
        objectShader.setMat3("normal", normal);
        objectShader.setMat4("model", objectModel);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        objectShader.setInt("material.envMap", 0);
        
        // lights direction in camera space
        glm::vec3 dirLightDir = glm::vec3(view * glm::vec4(dirLight, 0.0f));
        objectShader.setVec3("dirLight.direction", dirLightDir);
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            glm::vec3 pointLightDir = glm::vec3(view * glm::vec4(lampPos[i], 1.0f));
            objectShader.setVec3("pointLights[" + std::to_string(i) + "].position",
                                 pointLightDir);
        }
        
        object.draw(objectShader, 1);
        
        
        // ------------------------------------
        // render skybox as background
        
        // render this after all oblique objects are rendered
        // a trick: set depth to be 1.0 by setting gl_Position.zw to 1.0
        //          and depth function to GL_LEQUAL, so that skybox lays
        //          right on maximum depth
        glDepthFunc(GL_LEQUAL);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        skyboxShader.use();
        skyboxShader.setInt("skybox", 0);
        // ignore translation, so that camera never moves relative to skybox
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
        skyboxShader.setMat4("view", skyboxView);
        skyboxShader.setMat4("projection", projection);
        skybox.draw(skyboxShader);
        glDepthFunc(GL_LESS);
        
        
        // ------------------------------------
        // render semi-transparent glass
        
        // render this at last because of alpha blending
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glassTex);
        glassShader.use();
        glassShader.setMat4("view", view);
        glassShader.setMat4("projection", projection);
        glassShader.setInt("texture1", 0);
        glass.draw(glassShader);
        
        
        // ------------------------------------
        // switch back to default framebuffer
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        screenShader.use();
        screenShader.setInt("texture1", 0);
        
        glViewport(0, 0, currentSize.width, currentSize.height);
        screen.draw(screenShader); // draw screen in original size
        
        glViewport(0, 0, currentSize.width / 4, currentSize.height / 4);
        screen.draw(screenShader); // draw screen in small size
        
        glfwSwapBuffers(window); // use color buffer to draw
        glfwPollEvents(); // check events (keyboard, mouse, ...)
    }
}
