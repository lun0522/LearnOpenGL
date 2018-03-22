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

#include "shader.h"
#include "camera.h"
#include "render.h"

using std::string;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int NUM_CUBE = 5;
const int NUM_POINT_LIGHTS = 3;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastFrame = 0.0f;

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
    Shader lightShader = Shader(path + "shaders/shader_light.vs",
                                path + "shaders/shader_light.fs");
    Shader rotatorShader = Shader(path + "shaders/shader_rotator.vs",
                                  path + "shaders/shader_rotator.fs");
    
    // ------------------------------------
    // VAO, VBO and EBO
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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
    // which vertex attribute to configure (location)
    // size of vertex attribute
    // data type
    // whether e want the data to be normalized
    // space between consecutive vertex attribute sets (stride) (can set to be 0 if tightly packed)
    // offset of the position data begins in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0); // do this before EBO unbinds! otherwise the unbinding is also recorded
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    
    // ------------------------------------
    // texture
    
    // OpenGL expects 0.0 of y axis lays on the top, so flip the image
    stbi_set_flip_vertically_on_load(true);
    GLuint texture = loadTexture(path + "texture/matrix.jpg", GL_RGB);
    GLuint specularMap = loadTexture(path + "texture/container2_specular.png", GL_RGBA);
    
    // ------------------------------------
    // draw
    
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    
    glm::vec3 lightColor = glm::vec3(0.3f);
    glm::vec3 ambientColor = lightColor * 0.2f;
    glm::vec3 diffuseColor = lightColor * 0.8f;
    
    glm::vec3 cubePos[NUM_CUBE] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f,  1.8f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f)
    };
    
    glm::vec3 pointLightPos[NUM_POINT_LIGHTS] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -1.5f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -7.0f)
    };
    
    glm::vec3 pointLightColor[NUM_POINT_LIGHTS] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    
    rotatorShader.use();
    rotatorShader.setInt("material.emission", 0); // tell OpenGL which sampler corresponds to which texture
    rotatorShader.setInt("material.diffuse", 1);
    rotatorShader.setInt("material.specular", 1);
    rotatorShader.setFloat("material.shininess", 0.4f * 128.0f);
    rotatorShader.setVec3("material.specular", 0.508273f, 0.508273f, 0.508273f);
    
    // directional light
    rotatorShader.setVec3("dirLight.direction", -1.0f, -1.0f, -1.0f);
    rotatorShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
    rotatorShader.setVec3("dirLight.diffuse", glm::value_ptr(ambientColor));
    rotatorShader.setVec3("dirLight.specular", glm::value_ptr(lightColor));
    
    // point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
        string light = "pointLights[" + std::to_string(i) + "]";
        rotatorShader.setFloat(light + ".constant", 1.0f);
        rotatorShader.setFloat(light + ".linear", 0.09f);
        rotatorShader.setFloat(light + ".quadratic", 0.032f);
        glm::vec3 ambient = ambientColor + pointLightColor[i];
        glm::vec3 diffuse = diffuseColor + pointLightColor[i];
        glm::vec3 specular = lightColor + pointLightColor[i];
        rotatorShader.setVec3(light + ".ambient", glm::value_ptr(ambient));
        rotatorShader.setVec3(light + ".diffuse", glm::value_ptr(diffuse));
        rotatorShader.setVec3(light + ".specular", glm::value_ptr(specular));
    }
    
    // spot light
    rotatorShader.setVec3("spotLight.position", 0.0f, 0.0f, 0.0f);
    rotatorShader.setVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
    rotatorShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
    rotatorShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    rotatorShader.setFloat("spotLight.constant", 1.0f);
    rotatorShader.setFloat("spotLight.linear", 0.09f);
    rotatorShader.setFloat("spotLight.quadratic", 0.032f);
    rotatorShader.setVec3("spotLight.ambient", glm::value_ptr(ambientColor));
    rotatorShader.setVec3("spotLight.diffuse", glm::value_ptr(diffuseColor));
    rotatorShader.setVec3("spotLight.specular", glm::value_ptr(lightColor));
    
    while (!glfwWindowShouldClose(window)) { // until user hit close
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        processKeyboardInput();
        
        glBindVertexArray(rotatorVAO);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        
        lightShader.use();
        lightShader.setMat4("view", glm::value_ptr(view));
        lightShader.setMat4("projection", glm::value_ptr(projection));
        
        // only point lights
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLightPos[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setMat4("model", glm::value_ptr(model));
            lightShader.setVec3("lightColor", glm::value_ptr(pointLightColor[i]));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        rotatorShader.use();
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            glm::vec3 lightInView = glm::vec3(view * glm::vec4(pointLightPos[i], 1.0f));
            rotatorShader.setVec3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lightInView));
        }
        rotatorShader.setFloat("emitIntensity", glm::sin(lastFrame * 3) + 0.5);
        rotatorShader.setMat4("view", glm::value_ptr(view));
        rotatorShader.setMat4("projection", glm::value_ptr(projection));
        
        for (int i = 0; i < NUM_CUBE; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos[i]);
            model = glm::rotate(model, glm::radians(lastFrame * 20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view * model)));
            rotatorShader.setMat4("model", glm::value_ptr(model));
            rotatorShader.setMat3("normal", glm::value_ptr(normal));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(window); // use color buffer to draw
        glfwPollEvents(); // check events (keyboard, mouse, ...)
    }
    
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &rotatorVAO);
}
