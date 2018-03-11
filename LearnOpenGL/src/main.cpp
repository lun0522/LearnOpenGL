//
//  main.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/7/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"

using std::string;
using std::endl;
using std::cerr;

float ratio = 0.7f;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (ratio > 0.1) ratio -= 0.01;
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (ratio < 0.9) ratio += 0.01;
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

int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // ------------------------------------
    // window
    
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        cerr << "Failed to create window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // called when window is resized by the user
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // ------------------------------------
    // GLAD (function pointer loader)
    // do this after context is created, and before calling any OpenGL function!
    
    if (!gladLoadGL()) {
        cerr << "Failed to init GLAD" << endl;
        glfwTerminate();
        return -1;
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); // specify render area
    
    // ------------------------------------
    // shader program
    
    string path = "/Users/lun/Desktop/Code/LearnOpenGL/LearnOpenGL/src/";
    string vPath = path + "shaders/shader.vs", fPath = path + "shaders/shader.fs";
    Shader *shader;
    try {
        Shader tmp(vPath, fPath);
        shader = &tmp;
    } catch (string err) {
        cerr << err;
        glfwTerminate();
        return -1;
    }
    
    // ------------------------------------
    // VAO, VBO and EBO
    
    // vertex array object, vertex buffer object, element buffer array
    // VAO stores attribute pointers (interpreted from VBO) and EBO (records any glBindBuffer call
    // with target GL_ELEMENT_ARRAY_BUFFER)
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // top left
    };
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
    };
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0); // do this before EBO unbinds! otherwise the unbinding is also recorded
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO
    
    // ------------------------------------
    // texture
    
    // OpenGL expects 0.0 of y axis lays on the top, so flip the image
    stbi_set_flip_vertically_on_load(true);
    GLuint texture1, texture2;
    try {
        texture1 = loadTexture(path + "texture/awesomeface.png", GL_RGBA);
        texture2 = loadTexture(path + "texture/container.jpg", GL_RGB);
    } catch (string err) {
        cerr << err;
        glfwTerminate();
        return -1;
    }
    
    // ------------------------------------
    // draw
    
    // tell OpenGL which sampler corresponds to which texture
    // only have to do once
    (*shader).use();
    (*shader).setInt("texture1", 0);
    (*shader).setInt("texture2", 1);
    
    while (!glfwWindowShouldClose(window)) { // until user hit close
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // use program, set uniforms, and draw
        (*shader).use();
        (*shader).setFloat("ratio", ratio);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // how many indices to use, offset of array
        
        glfwSwapBuffers(window); // use color buffer to draw
        glfwPollEvents(); // check events (keyboard, mouse, ...)
    }
    
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
