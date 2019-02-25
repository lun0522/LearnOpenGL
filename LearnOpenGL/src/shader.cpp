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

#define GL_NO_SHADER 0

namespace opengl {

using namespace glm;
using namespace std;

namespace {

const string& ReadCode(const string& path) {
    static unordered_map<string, string> kLoadedCode{};
    auto loaded = kLoadedCode.find(path);
    if (loaded == kLoadedCode.end()) {
        ifstream file{path};
        file.exceptions(ifstream::failbit | ifstream::badbit);
        if (!file.is_open())
            throw runtime_error{"Failed to open file: " + path};
        
        try {
            ostringstream stream;
            stream << file.rdbuf();
            string code = stream.str();
            loaded = kLoadedCode.insert({path, code}).first;
        } catch (const ifstream::failure& e) {
            throw runtime_error{"Failed to read file: " + e.code().message()};
        }
    }
    return loaded->second;
}

GLuint CreateShader(GLenum type, const char* code) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr); // can pass an array of strings
    glCompileShader(shader);
    
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        throw runtime_error{string{"Failed to create shader: "} + info_log};
    }
    
    return shader;
}

GLuint CreateProgram(GLuint vert_shader,
                     GLuint frag_shader,
                     GLuint geom_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    if (geom_shader != GL_NO_SHADER)
        glAttachShader(program, geom_shader);
    glLinkProgram(program);
    
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        throw runtime_error{string{"Failed to link program: "} + info_log};
    }
    
    return program;
}

} /* namespace */

Shader::Shader(const string& vert_path,
               const string& frag_path,
               const string& geom_path) {
    GLuint vert_shader = CreateShader(
        GL_VERTEX_SHADER, ReadCode(vert_path).c_str());
    GLuint frag_shader = CreateShader(
        GL_FRAGMENT_SHADER, ReadCode(frag_path).c_str());
    GLuint geom_shader = geom_path.empty() ? GL_NO_SHADER : CreateShader(
        GL_GEOMETRY_SHADER, ReadCode(geom_path).c_str());
    program_id_ = CreateProgram(vert_shader, frag_shader, geom_shader);
    
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    if (geom_shader != GL_NO_SHADER)
        glDeleteShader(geom_shader);
}

void Shader::Use() const {
    glUseProgram(program_id_);
}

GLuint Shader::get_uniform(const string& name) const {
    GLint location = glGetUniformLocation(program_id_, name.c_str());
    if (location == GL_INVALID_INDEX)
        throw runtime_error{"Cannot find uniform: " + name};
    return location;
}

void Shader::set_int(const string& name, int value) const {
    glUniform1i(get_uniform(name), value);
}

void Shader::set_float(const string& name, float value) const {
    glUniform1f(get_uniform(name), value);
}

void Shader::set_vec3(const string& name, const vec3& value) const {
    glUniform3fv(get_uniform(name), 1, value_ptr(value));
}

void Shader::set_mat3(const string& name, const mat3& value) const {
    // how many matrices to send, transpose or not
    // (GLM is already in coloumn order, so no)
    glUniformMatrix3fv(get_uniform(name), 1, GL_FALSE, value_ptr(value));
}

void Shader::set_mat4(const string& name, const mat4& value) const {
    glUniformMatrix4fv(get_uniform(name), 1, GL_FALSE, value_ptr(value));
}

void Shader::set_block(const string& name, GLuint binding_point) const {
    GLint block_index = glGetUniformBlockIndex(program_id_, name.c_str());
    if (block_index == GL_INVALID_INDEX)
        throw runtime_error{"Cannot find block: " + name};
    glUniformBlockBinding(program_id_, block_index, binding_point);
}

} /* namespace opengl */
