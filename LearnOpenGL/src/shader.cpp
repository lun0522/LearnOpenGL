//
//  shader.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/9/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include "shader.h"

using std::string;

string readCode(string path) {
    using std::stringstream;
    using std::ifstream;
    
    stringstream stream;
    ifstream file(path);
    file.exceptions(ifstream::failbit | ifstream::badbit);
    if (!file.is_open()) throw "Cannot open file: " + path;
    
    try {
        stream << file.rdbuf();
        return stream.str();
    } catch (ifstream::failure e) {
        throw "Failed in reading file: " + e.code().message();
    }
}

GLuint createShader(GLenum type, string source) {
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
        throw string(infoLog);
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
        throw string(infoLog);
    }
}

Shader::Shader(string &vertexPath, string &fragmentPath) {
    GLuint vertex = createShader(GL_VERTEX_SHADER, readCode(vertexPath).c_str());
    GLuint fragment = createShader(GL_FRAGMENT_SHADER, readCode(fragmentPath).c_str());
    try {
        validateShader(vertex);
        validateShader(fragment);
    } catch (string log) {
        throw "Failed in creating shader: " + log;
    }
    
    programId = createProgram(vertex, fragment);
    try {
        validateLink(programId);
    } catch (string log) {
        throw "Failed in linking: " + log;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(programId);
}

GLuint Shader::getUniform(const string &name) const {
    GLint location = glad_glGetUniformLocation(programId, name.c_str());
    if (location != -1) return location;
    else throw string("Cannot find uniform " + name);
}

void Shader::setBool(const string &name, bool value) const {
    glUniform1i(getUniform(name), (int)value);
}

void Shader::setInt(const string &name, int value) const {
    glUniform1i(getUniform(name), value);
}

void Shader::setFloat(const string &name, float value) const {
    glUniform1f(getUniform(name), value);
}
