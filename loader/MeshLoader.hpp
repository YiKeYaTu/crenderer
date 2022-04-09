//
// Created by lc on 2022/4/7.
//

#ifndef CRD_MESHMeshLoader_HPP
#define CRD_MESHMeshLoader_HPP


#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <core/Vec.hpp>
#include <object/mesh/Mesh.hpp>
#include <object/primitive/Triangle.hpp>
#include <unordered_map>
#include <string>

class MeshLoader {
    inline static std::unordered_map<std::string, MeshLoader> _loaders = std::unordered_map<std::string, MeshLoader>();

private:
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

        _meshes.emplace_back(
            vertexes,
            indexes,
            normals,
            texCoords
        );
    }

public:
    MeshLoader(const std::string& path) {
        Assimp::Importer importer;
        _scene = const_cast<aiScene*>(importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals));
        _processNode(_scene->mRootNode);
    }
    MeshLoader(const std::vector<Mesh<Triangle>>& meshes): _meshes(meshes) {}
    MeshLoader(const MeshLoader& rhs) = delete;
    MeshLoader& operator=(const MeshLoader& rhs) = delete;

    const std::vector<Mesh<Triangle>>& meshes() const { return _meshes; }
    static const MeshLoader& load(const std::string& path) {
        if (_loaders.count(path) == 0) {
            _loaders.emplace(path, path);
        }
        return _loaders.find(path)->second;
    }
    static void unload(const std::string& path) {
        if (_loaders.count(path)) {
            _loaders.erase(path);
        }
    }
};

#endif //CRD_MESHMeshLoader_HPP
