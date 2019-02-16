//
//  shader.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/9/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

namespace GL {
    static unordered_map<string, string> loadedCode{};
    const string &readCode(const string &path) {
        auto loaded = loadedCode.find(path);
        if (loaded == loadedCode.end()) {
            ifstream file(path);
            file.exceptions(ifstream::failbit | ifstream::badbit);
            if (!file.is_open())
                throw runtime_error{"Failed to open file: " + path};
            
            try {
                ostringstream stream;
                stream << file.rdbuf();
                string code = stream.str();
                loaded = loadedCode.insert({ path, code }).first;
            } catch (ifstream::failure e) {
                throw runtime_error{"Failed to read file: " + e.code().message()};
            }
        }
        return loaded->second;
    }
    
    GLuint createShader(GLenum type, const char *code) {
        // .c_str() returns a r-value, should make it l-value and let it persist
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &code, nullptr); // can pass an array of strings
        glCompileShader(shader);
        return shader;
    }
    
    void validateShader(GLuint shader) {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            throw runtime_error{infoLog};
        }
    }
    
    GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, GLint geometryShader) {
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        if (geometryShader != GL_INVALID_INDEX)
            glAttachShader(shaderProgram, geometryShader);
        glLinkProgram(shaderProgram);
        return shaderProgram;
    }
    
    void validateLink(GLuint program) {
        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            throw runtime_error{infoLog};
        }
    }
    
    Shader::Shader(const string &vertPath,
                   const string &fragPath,
                   const string &geomPath) {
        GLuint vertShader = createShader(GL_VERTEX_SHADER, readCode(vertPath).c_str());
        GLuint fragShader = createShader(GL_FRAGMENT_SHADER, readCode(fragPath).c_str());
        GLint geomShader = geomPath.empty() ? GL_INVALID_INDEX : createShader(GL_GEOMETRY_SHADER, readCode(geomPath).c_str());
        try {
            validateShader(vertShader);
            validateShader(fragShader);
            if (geomShader != GL_INVALID_INDEX)
                validateShader(geomShader);
        } catch (const string &e) {
            throw runtime_error{"Failed to create shader: " + e};
        }
        
        programId = createProgram(vertShader, fragShader, geomShader);
        try {
            validateLink(programId);
        } catch (const string &e) {
            throw runtime_error{"Failed to link: " + e};
        }
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        if (!geomPath.empty())
            glDeleteShader(geomShader);
    }
    
    void Shader::use() const {
        glUseProgram(programId);
    }
    
    GLuint Shader::getUniform(const string &name) const {
        GLint location = glGetUniformLocation(programId, name.c_str());
        if (location == GL_INVALID_INDEX)
            throw runtime_error{"Cannot find uniform: " + name};
        return location;
    }
    
    void Shader::setInt(const string &name, int value) const {
        glUniform1i(getUniform(name), value);
    }
    
    void Shader::setFloat(const string &name, float value) const {
        glUniform1f(getUniform(name), value);
    }
    
    void Shader::setVec3(const string &name, GLfloat v0,
                         GLfloat v1, GLfloat v2) const {
        glUniform3f(getUniform(name), v0, v1, v2);
    }
    
    void Shader::setVec3(const string &name, const vec3 &value) const {
        glUniform3fv(getUniform(name), 1, value_ptr(value));
    }
    
    void Shader::setMat3(const string &name, const mat3 &value) const {
        // how many matrices to send, transpose or not (GLM is already in coloumn order, so no)
        glUniformMatrix3fv(getUniform(name), 1, GL_FALSE, value_ptr(value));
    }
    
    void Shader::setMat4(const string &name, const mat4 &value) const {
        glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, value_ptr(value));
    }
    
    void Shader::setBlock(const string &name, GLuint bindingPoint) const {
        GLint blockIndex = glGetUniformBlockIndex(programId, name.c_str());
        if (blockIndex == GL_INVALID_INDEX)
            throw runtime_error{"Cannot find block: " + name};
        glUniformBlockBinding(programId, blockIndex, bindingPoint);
    }
}
