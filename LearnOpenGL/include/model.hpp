//
//  model.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/23/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef model_hpp
#define model_hpp

#include <functional>
#include <vector>
#include <string>
#include <assimp/scene.h>

#include "mesh.hpp"

namespace opengl {

class Model {
  public:
    Model(const std::string& obj_path, const std::string& tex_path = "");
    
    void Draw(const Shader& shader,
              GLuint tex_offset = 0,
              bool load_texture = true) const {
        shader.Use();
        for (const auto& mesh : meshes_)
            mesh.Draw(shader, tex_offset, load_texture);
    }
    
    void DrawInstanced(const Shader& shader,
                       GLuint amount,
                       GLuint tex_offset = 0,
                       bool load_texture = true) const {
        shader.Use();
        for (const auto& mesh : meshes_)
            mesh.DrawInstanced(shader, amount, tex_offset, load_texture);
    }
        
    void AppendData(const std::function<void ()>& func) const {
        for (const auto& mesh : meshes_)
            mesh.AppendData(func);
    }
    
  private:
    std::vector<Mesh> meshes_;
};

} /* namespace opengl */

#endif /* model_hpp */
