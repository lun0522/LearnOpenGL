//
//  shader.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/9/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef shader_hpp
#define shader_hpp

#include <string>
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader {
    GLuint programId;
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    GLuint getUniform(const std::string &name) const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, GLfloat v0, GLfloat v1, GLfloat v2) const;
    void setVec3(const std::string &name, glm::vec3& value) const;
    void setMat3(const std::string &name, glm::mat3& value) const;
    void setMat4(const std::string &name, glm::mat4& value) const;
};

#endif /* Shader_h */