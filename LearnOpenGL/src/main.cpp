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
#include "shader.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
        std::cerr << "Failed to create window" << std::endl;
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
        std::cerr << "Failed to init GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); // specify render area
    
    // ------------------------------------
    // shader program
    
    std::string path = "/Users/lun/Desktop/Code/LearnOpenGL/LearnOpenGL/src/shaders/";
    Shader *shader;
    try {
        Shader tmp(path + "shader.vs", path + "shader.fs");
        shader = &tmp;
    } catch (std::string err) {
        std::cerr << err;
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
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    GLuint indices[] = {
        0, 1, 2
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // vertex attributes are disabled by default
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0); // do this before EBO unbinds! otherwise the unbinding is also recorded
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO
    
    // ------------------------------------
    // draw
    
    while (!glfwWindowShouldClose(window)) { // until user hit close
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        (*shader).use();
        (*shader).setFloat("bias", 0.25f);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0); // how many indices to use, offset of array
        
        glfwSwapBuffers(window); // use color buffer to draw
        glfwPollEvents(); // check events (keyboard, mouse, ...)
    }
    
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
