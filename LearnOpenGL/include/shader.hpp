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

class Shader {
  public:
    Shader(const std::string& vert_path,
           const std::string& frag_path,
           const std::string& geom_path = "");
    void Use() const;
    GLuint get_uniform(const std::string& name) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_mat3(const std::string& name, const glm::mat3& value) const;
    void set_mat4(const std::string& name, const glm::mat4& value) const;
    void set_block(const std::string& name, GLuint binding_point) const;
    
  private:
    GLuint program_id_;
};

} /* namespace opengl */

#endif /* shader_hpp */
