//
//  shader.h
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/9/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef Shader_h
#define Shader_h

#include "glad.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    GLuint programId;
    Shader(std::string &vertexPath, std::string &fragmentPath);
    void use();
    GLuint getUniform(const std::string &name) const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};

#endif /* Shader_h */
