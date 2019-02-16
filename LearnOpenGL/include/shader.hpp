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
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace GL {
    using namespace glm;
    using namespace std;
    
    class Shader {
        GLuint programId;
    public:
        Shader(const string &vertPath,
               const string &fragPath,
               const string &geomPath = "");
        void use() const;
        GLuint getUniform(const string &name) const;
        void setInt(const string &name, int value) const;
        void setFloat(const string &name, float value) const;
        void setVec3(const string &name, GLfloat v0,
                     GLfloat v1, GLfloat v2) const;
        void setVec3(const string &name, const vec3 &value) const;
        void setMat3(const string &name, const mat3 &value) const;
        void setMat4(const string &name, const mat4 &value) const;
        void setBlock(const string &name, GLuint bindingPoint) const;
    };
}

#endif /* shader_hpp */
