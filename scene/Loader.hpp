//
// Created by lc on 2022/4/1.
//

#ifndef CRENDERER_LOADER_HPP
#define CRENDERER_LOADER_HPP

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <core/Vec.hpp>
#include <object/mesh/Mesh.hpp>
#include <object/primitive/Triangle.hpp>
#include <unordered_map>
#include <string>

class Loader {
    inline static std::unordered_map<std::string, Loader> loaders = std::unordered_map<std::string, Loader>();

private:
    const aiScene* _scene;
    std::vector<Mesh<Triangle>> _meshes;

    Loader(const std::string& path) {
        Assimp::Importer importer;
        _scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
        _processNode(_scene->mRootNode);
    }

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

        _meshes.emplace_back(
            vertexes,
            indexes,
            normals,
            texCoords
        );

    }

public:
    const std::vector<Mesh<Triangle>>& meshes() const { return _meshes; }
    static const Loader load(const std::string& path) {
        if (loaders.count(path) == 0) {
            loaders.emplace(path, Loader(path));
        }
        return loaders.find(path)->second;
    }
    static void unload(const std::string& path) {
        if (loaders.count(path)) {
            loaders.erase(path);
        }
    }
};
#endif //CRENDERER_LOADER_HPP
