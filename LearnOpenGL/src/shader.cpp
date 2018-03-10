//
//  shader.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/9/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include "shader.h"

std::string readCode(std::string path) {
    std::string code;
    std::stringstream stream;
    std::ifstream file(path);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        stream << file.rdbuf();
        code = stream.str();
        file.close();
    } catch (std::ifstream::failure e) {
        throw "Failed in reading file: " + e.code().message();
    }
    
    return code;
}

GLuint createShader(GLenum type, std::string source) {
    // .c_str() returns a r-value, should make it l-value and let it persist
    const char *srcString = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &srcString, NULL); // can pass an array of strings
    glCompileShader(shader);
    return shader;
}

void validateShader(GLuint shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw std::string(infoLog);
    }
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    return shaderProgram;
}

void validateLink(GLuint program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        throw std::string(infoLog);
    }
}

Shader::Shader(std::string vertexPath, std::string fragmentPath) {
    const char *vertexCode, *fragmentCode;
    try {
        vertexCode = readCode(vertexPath).c_str();
        fragmentCode = readCode(fragmentPath).c_str();
    } catch (std::string err) {
        throw err;
    }
    
    GLuint vertex = createShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragment = createShader(GL_FRAGMENT_SHADER, fragmentCode);
    try {
        validateShader(vertex);
        validateShader(fragment);
    } catch (std::string log) {
        throw "Failed in creating shader: " + log;
    }
    
    programId = createProgram(vertex, fragment);
    try {
        validateLink(programId);
    } catch (std::string log) {
        throw "Failed in linking: " + log;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(programId);
}

void Shader::setBool(const std::string &name, bool value) const {
    GLint location = glad_glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform1i(location, (int)value);
    else throw std::string("Cannot find uniform " + name);
}

void Shader::setInt(const std::string &name, int value) const {
    GLint location = glad_glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform1i(location, value);
    else throw std::string("Cannot find uniform " + name);
}

void Shader::setFloat(const std::string &name, float value) const {
    GLint location = glad_glGetUniformLocation(programId, name.c_str());
    if (location != -1) glUniform1f(location, value);
    else throw std::string("Cannot find uniform " + name);
}
