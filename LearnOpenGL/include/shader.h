//
//  shader.h
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/9/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef shader_h
#define shader_h

#include <string>
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>

class Shader {
public:
    Shader(std::string vertexPath, std::string fragmentPath);
    GLuint programId;
    void use();
    GLuint getUniform(const std::string &name) const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVector(const std::string &name, GLfloat *value) const;
    void setMatrix(const std::string &name, GLfloat *value) const;
};

#endif /* Shader_h */
