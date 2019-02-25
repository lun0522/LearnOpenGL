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

namespace opengl {

using glm::mat3;
using glm::mat4;
using glm::vec3;
using std::string;

class Shader {
    GLuint program_id_;
public:
    Shader(const string& vert_path,
           const string& frag_path,
           const string& geom_path = "");
    void Use() const;
    GLuint get_uniform(const string& name) const;
    void set_int(const string& name, int value) const;
    void set_float(const string& name, float value) const;
    void set_vec3(const string& name, const vec3& value) const;
    void set_mat3(const string& name, const mat3& value) const;
    void set_mat4(const string& name, const mat4& value) const;
    void set_block(const string& name, GLuint bindingPoint) const;
};

} /* namespace opengl */

#endif /* shader_hpp */
