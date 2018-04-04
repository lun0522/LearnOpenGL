//
//  render.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "loader.hpp"
#include "model.hpp"
#include "shadow.hpp"
#include "render.hpp"

using std::string;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

typedef struct ScreenSize {
    int width;
    int height;
} ScreenSize;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int NUM_POINT_LIGHTS = 3;
const int NUM_ASTEROID = 750;

Camera camera(vec3(0.0f, 0.0f, 10.0f));
float lastFrame = 0.0f;
float explosion = 0.0f;
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
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        explosion = std::max(0.0f, explosion - 0.1f);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        explosion = std::min(9.9f, explosion + 0.1f);
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
                        path + "shaders/shader_object.fs",
                        path + "shaders/shader_object.gs");
    Shader skyboxShader(path + "shaders/shader_skybox.vs",
                        path + "shaders/shader_skybox.fs");
    Shader screenShader(path + "shaders/shader_screen.vs",
                        path + "shaders/shader_screen.fs");
    Shader planetShader(path + "shaders/shader_planet.vs",
                        path + "shaders/shader_planet.fs");
    Shader asteroidShader(path + "shaders/shader_asteroid.vs",
                          path + "shaders/shader_planet.fs");
    Shader dirShadowShader(path + "shaders/shader_shadow.vs",
                           path + "shaders/shader_shadow.fs");
    
    
    // ------------------------------------
    // models
    
    Model lamp(path + "texture/cube.obj");
    Model glass(path + "texture/glass.obj");
    Model skybox(path + "texture/skybox.obj");
    Model screen(path + "texture/screen.obj");
    Model object(path + "texture/nanosuit/nanosuit.obj", path + "texture/nanosuit");
    Model planet(path + "texture/planet/planet.obj", path + "texture/planet");
    Model asteroid(path + "texture/rock/rock.obj", path + "texture/rock");
    
    std::vector<string> boxfaces {
        "right.tga",
        "left.tga",
        "top.tga",
        "bottom.tga",
        "back.tga",
        "front.tga",
    };
    GLuint skyboxTex = Loader::loadCubemap(path + "texture/tidepool", boxfaces);
    GLuint glassTex = Loader::loadTexture(path + "texture/glass.png", true);
    GLuint floorTex = Loader::loadTexture(path + "texture/floor.jpg", true);
    GLuint blackTex = Loader::loadTexture(path + "texture/black.jpg", true);
    
    vec3 dirLight(1.0f, -1.0f, 1.0f);
    Shadow dirLightShadow(currentSize.width, currentSize.height,
                          vec3(0.0f) - dirLight * 20.0f, dirLight);
    
    
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
    
    GLuint uboMatrices; // used to store view and projection matrices
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_DYNAMIC_DRAW); // no data yet
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices); // or use glBindBufferRange for flexibility
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    for (Shader& shader : std::vector<Shader> {
        lampShader,
        glassShader,
        objectShader,
        skyboxShader,
        planetShader,
        asteroidShader,
    }) {
        shader.use();
        shader.setBlock("Matrices", 0);
    }
    
    mat4 objectModel = glm::translate(mat4(1.0f), vec3(0.0f, -5.0f, 0.0f));
    objectModel = glm::scale(objectModel, vec3(1.0f) * 0.5f);
    
    mat4 floorModel = glm::translate(mat4(1.0f), vec3(0.0f, -5.0f, 0.0f));
    floorModel = glm::rotate(floorModel, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    floorModel = glm::scale(floorModel, vec3(1.0f) * 5.0f);
    
    mat4 glassModel = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 6.0f));
    glassShader.use();
    glassShader.setMat4("model", glassModel);
    
    vec3 planetCenter(0.0f, 5.5f, 0.0f);
    mat4 planetModel = glm::translate(glm::mat4(1.0f), planetCenter);
    
    std::vector<mat4> asteroidModels(NUM_ASTEROID);
    srand(glfwGetTime()); // random seed
    float radius = 5.0f, offset = 1.0f, displacement[3];
    for (int i = 0; i < NUM_ASTEROID; ++i) {
        for (int j = 0; j < 3; ++j)
            displacement[j] = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        
        mat4 model = glm::translate(mat4(1.0f), planetCenter);
        
        float theta = (float)i / NUM_ASTEROID * 360.0f;
        float x = sin(theta) * radius + displacement[0];
        float y = displacement[1] * 0.4f;
        float z = cos(theta) * radius + displacement[2];
        model = glm::translate(model, vec3(x, y, z)); // -offset ~ offset
        
        float angle = rand() % 360; // 0 ~ 360
        model = glm::rotate(model, glm::radians(angle), vec3(0.4f, 0.6f, 0.8f));
        
        float scale = (rand() % 20) / 100.f + 0.05; // 0.05 ~ 0.25
        model = glm::scale(model, vec3(scale * 0.25f));
        
        asteroidModels[i] = model;
    }
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_ASTEROID * sizeof(mat4), asteroidModels.data(), GL_STATIC_DRAW);
    
    auto func = []() {
        for (int attrib = 3; attrib <= 6; ++attrib) {
            // maximum amount of data allowed as a vertex attribute is equal to a vec4
            // so state that there are 4 vec4s as a workaround
            glVertexAttribPointer(attrib, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4),
                                  (void *)(sizeof(vec4) * (attrib - 3)));
            glEnableVertexAttribArray(attrib);
            // second parameter means to read next chunk of data after how many instances
            // if stated as 1, model data gets updated only after one entire instance is drawn,
            // rather than after each vertex
            glVertexAttribDivisor(attrib, 1);
        }
    };
    asteroid.appendData(func);
    
    vec3 lightColor(0.3f);
    vec3 ambientColor = lightColor * 0.1f;
    vec3 diffuseColor = lightColor * 0.6f;
    
    vec3 lampPos[NUM_POINT_LIGHTS] = {
        vec3( 0.0f, -3.0f,  4.0f),
        vec3(-4.0f, -1.0f, -3.0f),
        vec3( 4.0f,  2.0f, -2.0f)
    };
    
    vec3 lampColor[NUM_POINT_LIGHTS] = {
        vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 0.0f, 1.0f)
    };
    
    objectShader.use();
    objectShader.setFloat("material.shininess", 0.2f);
    
    // directional light
    objectShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
    objectShader.setVec3("dirLight.diffuse", diffuseColor);
    objectShader.setVec3("dirLight.specular", ambientColor);
    objectShader.setMat4("lightSpace", dirLightShadow.getLightSpaceMatrix());
    
    // point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
        string light = "pointLights[" + std::to_string(i) + "]";
        objectShader.setFloat(light + ".constant", 1.0f);
        objectShader.setFloat(light + ".linear", 0.1f);
        objectShader.setFloat(light + ".quadratic", 0.002f);
        vec3 ambient = ambientColor * lampColor[i] * 0.3f;
        vec3 diffuse = diffuseColor * lampColor[i] * 0.3f;
        vec3 specular = lightColor * lampColor[i] * 0.3f;
        objectShader.setVec3(light + ".ambient", ambient);
        objectShader.setVec3(light + ".diffuse", diffuse);
        objectShader.setVec3(light + ".specular", specular);
    }
    
    // spot light
    objectShader.setVec3("spotLight.position", 0.0f, 0.0f, 0.0f);
    objectShader.setVec3("spotLight.direction", 0.0f, 0.0f, -1.0f);
    objectShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(7.5f)));
    objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));
    objectShader.setFloat("spotLight.constant", 1.0f);
    objectShader.setFloat("spotLight.linear", 0.1f);
    objectShader.setFloat("spotLight.quadratic", 0.002f);
    objectShader.setVec3("spotLight.ambient", ambientColor);
    objectShader.setVec3("spotLight.diffuse", diffuseColor);
    objectShader.setVec3("spotLight.specular", lightColor);
    
    
    // ------------------------------------
    // draw
    
    int frameCount = 0;
    double lastTime = glfwGetTime();
    
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
        
        // set once, use anywhere
        mat4 view = camera.getViewMatrix();
        mat4 projection = camera.getProjectionMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        
        // ------------------------------------
        // render lamps with outlines
        
        // enable any of ragments of lights (lamps) to update stencil buffer with 1
        // so that later we know where we should not draw outlines
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // let stencil test always pass
        
        lampShader.use();
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            mat4 lampModel = glm::translate(mat4(1.0f), lampPos[i]);
            lampModel = glm::scale(lampModel, vec3(0.8f));
            lampShader.setMat4("model", lampModel);
            lampShader.setVec3("lightColor", lampColor[i]);
            lamp.draw(lampShader);
        }
        
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // disable updating stencil buffer (imagine when lamps overlap)
        
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            mat4 lampModel = glm::translate(mat4(1.0f), lampPos[i]);
            lampModel = glm::scale(lampModel, vec3(0.85f));
            lampShader.setMat4("model", lampModel);
            lampShader.setVec3("lightColor", 1.0f, 1.0f, 0.0f);
            lamp.draw(lampShader);
        }
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        
        
        // ------------------------------------
        // render object
        
        std::vector<Model> models {
            object,
            glass,
        };
        std::vector<mat4> modelMatrices {
            objectModel,
            floorModel,
        };
        dirLightShadow.calculate(dirShadowShader, models, modelMatrices,
                                 originalSize.width, originalSize.height, framebuffer);
        
        glDisable(GL_CULL_FACE); // for explosion effect
        
        objectShader.use();
        dirLightShadow.bindShadowMap(GL_TEXTURE0);
        objectShader.setInt("depthMap", 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        objectShader.setInt("material.envMap", 1);
        
        objectShader.setFloat("explosion", explosion);
        mat3 normal = glm::transpose(glm::inverse(mat3(view * objectModel)));
        objectShader.setMat3("normal", normal);
        mat3 invView = glm::inverse(glm::mat3(view));
        objectShader.setMat3("invView", invView);
        
        // lights direction in camera space
        vec3 dirLightDir = vec3(view * vec4(dirLight, 0.0f));
        objectShader.setVec3("dirLight.direction", dirLightDir);
        for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
            vec3 pointLightDir = vec3(view * vec4(lampPos[i], 1.0f));
            objectShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightDir);
        }
        
        objectShader.setMat4("model", objectModel);
        object.draw(objectShader, 2);
        
        glEnable(GL_CULL_FACE);
        
        // note! even if we don't need cudemap when render floor, material.cubemap
        // still must have a value. if we bind floorTex to GL_TEXTURE1, material.cubemap
        // will become unset, and floor will not get rendered!
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, floorTex);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, blackTex);
        objectShader.setInt("material.diffuse0", 2);
        objectShader.setInt("material.specular0", 3);
        objectShader.setInt("material.reflection0", 3);
        
        normal = glm::transpose(glm::inverse(mat3(view * floorModel)));
        objectShader.setMat3("normal", normal);
        objectShader.setMat4("model", floorModel);
        glass.draw(objectShader);
        
        
        // ------------------------------------
        // render planet and asteroids
        
        planetShader.use();
        planetModel = glm::rotate(planetModel, 0.01f, vec3(0.0f, 1.0f, 0.0f));
        planetShader.setMat4("model", glm::scale(planetModel, vec3(0.5f)));
        planet.draw(planetShader);
        asteroid.drawInstanced(asteroidShader, NUM_ASTEROID);
        
        
        // ------------------------------------
        // render skybox as background
        
        // render this after all oblique objects are rendered
        // a trick: set depth to be 1.0 by setting gl_Position.zw to 1.0
        //          and depth function to GL_LEQUAL, so that skybox lays
        //          right on maximum depth (can either set gl_FragDepth
        //          to 1.0 in fragment shader, however, in that case OpenGL
        //          cannot do early depth testing any more)
        glDepthFunc(GL_LEQUAL);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        skyboxShader.use();
        skyboxShader.setInt("skybox", 0);
        skybox.draw(skyboxShader);
        glDepthFunc(GL_LESS);
        
        
        // ------------------------------------
        // render semi-transparent glass
        
        // render this at last because of alpha blending
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glassTex);
        glassShader.use();
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
        
        ++frameCount;
        double currentTime = glfwGetTime();
        if (currentTime - lastTime > 1.0) {
            std::cout <<  "FPS: " << std::to_string(frameCount) << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
    }
}
