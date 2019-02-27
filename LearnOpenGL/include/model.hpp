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

using std::string;
using std::vector;

class Model {
    vector<Mesh> meshes_;
    
public:
    Model(const string& obj_path, const string& tex_path = "");
    
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
        
    void AppendData(const function<void ()>& func) const {
        for (const auto& mesh : meshes_)
            mesh.AppendData(func);
    }
};

} /* namespace opengl */

#endif /* model_hpp */
