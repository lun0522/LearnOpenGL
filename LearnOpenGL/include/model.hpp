//
//  model.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/23/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef model_hpp
#define model_hpp

#include <vector>
#include <string>
#include <unordered_map>
#include <assimp/scene.h>

#include "mesh.hpp"

class Model {
    const bool gamma;
    std::string directory;
    std::vector<Mesh> meshes;
    std::unordered_map<std::string, Texture> loadedTexture;
    void processNode(const aiNode *node, const aiScene *scene);
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(const aiMaterial *material,
                                              const aiTextureType aiType,
                                              const TextureType type);
public:
    Model(const std::string& objPath,
          const std::string& texPath = "",
          const bool gammaCorrection = false);
    void draw(const Shader& shader, const GLuint texOffset = 0) const;
    void drawInstanced(const Shader& shader, const GLuint amount, const GLuint texOffset = 0) const;
    template<typename Func>
    void appendData(Func& func) const {
        for (int i = 0; i < meshes.size(); ++i)
        meshes[i].appendData(func);
    }
};

#endif /* model_h */
