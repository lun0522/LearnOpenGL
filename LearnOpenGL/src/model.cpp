//
//  model.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/23/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "loader.hpp"
#include "model.hpp"

namespace opengl {

using glm::vec2;
using glm::vec3;
using std::runtime_error;

namespace {

vector<Texture> LoadMaterialTextures(const string& directory,
                                     const aiMaterial* material,
                                     aiTextureType ai_type,
                                     TextureType type) {
    vector<Texture> textures(material->GetTextureCount(ai_type));
    for (int i = 0; i < textures.size(); ++i) {
        aiString path;
        material->GetTexture(ai_type, i, &path);
        GLuint texture = Loader::loadTexture(
            directory + "/" + path.C_Str(), type == TextureType::kDiffuse);
        textures[i] = {texture, type};
    }
    return textures;
}

Mesh ProcessMesh(const string& directory,
                 const aiMesh* mesh,
                 const aiScene* scene) {
    // load vertices (position, normal, texCoord)
    vector<Vertex> vertices(mesh->mNumVertices);
    aiVector3D* ai_tex_coords = mesh->mTextureCoords[0];
    for (int i = 0; i < vertices.size(); ++i) {
        vec3 position{mesh->mVertices[i].x,
                      mesh->mVertices[i].y,
                      mesh->mVertices[i].z};
        vec3 normal{mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z};
        vec2 tex_coord{ai_tex_coords ? ai_tex_coords[i].x : 0.0,
                       ai_tex_coords ? ai_tex_coords[i].y : 0.0};
        vertices[i] = {position, normal, tex_coord};
    }
    
    // load indices
    vector<GLuint> indices{};
    for (int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        indices.insert(indices.end(), face.mIndices,
                       face.mIndices + face.mNumIndices);
    }
    
    // load textures
    vector<Texture> textures{};
    if (scene->HasMaterials()) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diff_maps = LoadMaterialTextures(
            directory, material, aiTextureType_DIFFUSE,
            TextureType::kDiffuse);
        vector<Texture> spec_maps = LoadMaterialTextures(
            directory, material, aiTextureType_SPECULAR,
            TextureType::kSpecular);
        vector<Texture> refl_maps = LoadMaterialTextures(
            directory, material, aiTextureType_AMBIENT,
            TextureType::kReflection);
        
        textures.insert(textures.end(), diff_maps.begin(), diff_maps.end());
        textures.insert(textures.end(), spec_maps.begin(), spec_maps.end());
        textures.insert(textures.end(), refl_maps.begin(), refl_maps.end());
    }
    
    return {vertices, indices, textures};
}

void ProcessNode(vector<Mesh>* meshes,
                 const string& directory,
                 const aiNode* node,
                 const aiScene* scene) {
    for (int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->emplace_back(ProcessMesh(directory, mesh, scene));
    }
    for (int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(meshes, directory, node->mChildren[i], scene);
    }
}

} /* namespace */

Model::Model(const string& obj_path, const string& tex_path) {
    Assimp::Importer importer;
    // other useful options:
    // aiProcess_GenNormals: create normal for vertices
    // aiProcess_SplitLargeMeshes: split mesh when the number of triangles
    //     that can be rendered at a time is limited
    // aiProcess_OptimizeMeshes: do the reverse of splitting, merge meshes to
    //     reduce drawing calls
    const aiScene* scene = importer.ReadFile(
        obj_path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if (!scene || !scene->mRootNode
        || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        throw runtime_error{
            string{"Failed to import scene: "} + importer.GetErrorString()};
    
    ProcessNode(&meshes_, tex_path, scene->mRootNode, scene);
}

} /* namespace opengl */
