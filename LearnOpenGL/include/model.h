//
//  model.h
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/23/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef model_h
#define model_h

#include <vector>
#include <string>
#include <unordered_map>
#include <assimp/scene.h>

#include "mesh.h"

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
    void draw(const Shader& shader) const;
    static GLuint textureFromFile(const std::string& path);
};

#endif /* model_h */
