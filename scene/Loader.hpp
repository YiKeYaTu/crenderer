//
// Created by lc on 2022/4/1.
//

#ifndef CRENDERER_LOADER_HPP
#define CRENDERER_LOADER_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "object/mesh/Mesh.hpp"
#include <vector>
#include <core/Vec.hpp>
#include <object/primitive/Triangle.hpp>

class Loader {
private:
    Assimp::Importer _importer;
    aiScene* _scene;
    std::vector<Mesh<Triangle>> _meshes;

    void _processNode(aiNode *node) {
        if (!node) {
            return;
        }
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = _scene->mMeshes[node->mMeshes[i]];
            _processMesh(mesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            _processNode(node->mChildren[i]);
        }
    }

    void _processMesh(aiMesh *mesh) {
        std::vector<Vec3f> vertexes;
        std::vector<unsigned int> indexes;
        std::vector<Vec3f> normals;
        std::vector<Vec2f> texCoords;

        vertexes.reserve(mesh->mNumVertices);
        indexes.reserve(mesh->mNumVertices);
        normals.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            aiVector3t<float>& vertex = mesh->mVertices[i];
            aiVector3t<float>& normal = mesh->mNormals[i];
            vertexes.emplace_back(vertex.x, vertex.y, vertex.z);
            normals.emplace_back(normal.x, normal.y, normal.z);

            if (mesh->mTextureCoords[0]) {
                texCoords.emplace_back(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indexes.push_back(face.mIndices[j]);
            }
        }

//        if (mesh->mMaterialIndex >= 0) {
//            aiMaterial *material = _scene->mMaterials[mesh->mMaterialIndex];
//            for(unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++) {
//                aiString str;
//                material->GetTexture(aiTextureType_DIFFUSE, i, &str);
//
//            }
//        }

        _meshes.emplace_back(
            vertexes,
            indexes,
            normals,
            texCoords
        );

    }

public:
    Loader(const std::string& path)
        : _scene(const_cast<aiScene*>(_importer.ReadFile(
            path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals
        ))) {
        _processNode(_scene->mRootNode);
    }

    const std::vector<Mesh<Triangle>>& meshes() const { return _meshes; }
};

#endif //CRENDERER_LOADER_HPP
