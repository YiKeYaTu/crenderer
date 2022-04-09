//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_MATERIAL_HPP
#define CRENDERER_MATERIAL_HPP

#include <core/Vec.hpp>
#include <core/Mat.hpp>
#include <object/mesh/Mesh.hpp>
#include <object/primitive/Primitive.hpp>
#include <scene/Camera.hpp>
#include <scene/Scene.hpp>

#include <loader/MeshLoader.hpp>
#include <loader/TextureLoader.hpp>

class Material {
public:
    Material(const MeshLoader& meshLoader, const std::vector<const TextureLoader*>& textureLoaders = std::vector<const TextureLoader*>(), const Mat4f& transformation = Mat4f::Identity())
        : _meshLoader(&meshLoader), _textureLoaders(textureLoaders), _transformation(transformation) {
        assert(textureLoaders.size() <= 1 || textureLoaders.size() == meshLoader.meshes().size());
    }

    virtual ~Material() {}

    const Mat4f &getTransformation() const {
        return _transformation;
    }

protected:
    const MeshLoader* _meshLoader;
    std::vector<const TextureLoader*> _textureLoaders;
    Mat4f _transformation;
};

#endif //CRENDERER_MATERIAL_HPP
