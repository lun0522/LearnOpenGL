//
//  model.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/23/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

#include "model.h"

using std::string;
using std::vector;

void Model::processNode(const aiNode *node, const aiScene *scene) {
    for (int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
    // load vertices (position, normal, texCoord)
    vector<Vertex> vertices(mesh->mNumVertices);
    aiVector3D *aiTexCoords = mesh->mTextureCoords[0];
    for (int i = 0; i < vertices.size(); ++i) {
        glm::vec3 position(mesh->mVertices[i].x,
                           mesh->mVertices[i].y,
                           mesh->mVertices[i].z);
        glm::vec3 normal(mesh->mNormals[i].x,
                         mesh->mNormals[i].y,
                         mesh->mNormals[i].z);
        glm::vec2 texCoord(aiTexCoords ? aiTexCoords[i].x : 0.0,
                           aiTexCoords ? aiTexCoords[i].y : 0.0);
        vertices[i] = { position, normal, texCoord };
    }
    
    // load indices
    vector<GLuint> indices;
    for (int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    // load textures
    vector<Texture> textures;
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    
    return Mesh(vertices, indices, textures);
}

GLuint textureFromFile(const string& path) {
    int width, height, channel;
    stbi_uc *data = stbi_load(path.c_str(), &width, &height, &channel, 0);
    if (!data) throw "Failed to load texture from " + path;
    
    GLenum format;
    switch (channel) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            throw "Unknown texture format (channel=" + std::to_string(channel) + ")";
    }
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // texture target, minmap level, texture format, width, height, always 0, image format, dtype, data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // automatically generate all required minmaps
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture;
}

vector<Texture> Model::loadMaterialTextures(const aiMaterial *material,
                                            const aiTextureType aiType,
                                            const TextureType type) {
    vector<Texture> textures;
    for (int i = 0; i < material->GetTextureCount(aiType); ++i) {
        aiString path;
        material->GetTexture(aiType, i, &path);
        string filename(path.C_Str());
        
        auto loaded = loadedTexture.find(filename);
        if (loaded == loadedTexture.end()) {
            Texture texture = { textureFromFile(directory + '/' + filename), type, filename };
            textures.push_back(texture);
            loadedTexture.insert({ filename, texture });
        } else {
            textures.push_back(loaded->second);
        }
    }
    return textures;
}

Model::Model(const string& path, const bool gammaCorrection): gamma(gammaCorrection) {
    Assimp::Importer importer;
    // other useful options:
    // aiProcess_GenNormals: create normal for vertices
    // aiProcess_SplitLargeMeshes: split mesh when the number of triangles that can be rendered at a time is limited
    // aiProcess_OptimizeMeshes: do the reverse of splitting, merge meshes to reduce drawing calls
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw "Failed to import scene: " + string(importer.GetErrorString());
    
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::draw(const Shader& shader) const {
    for (int i = 0; i < meshes.size(); ++i)
        meshes[i].draw(shader);
}
