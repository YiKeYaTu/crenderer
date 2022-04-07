//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_MATERIAL_HPP
#define CRENDERER_MATERIAL_HPP

#include <core/Vec.hpp>
#include <core/Mat.hpp>
#include <object/mesh/Mesh.hpp>
#include <object/primitive/Primitive.hpp>
#include <scene/Loader.hpp>
#include <scene/Camera.hpp>
#include <scene/Scene.hpp>

class Material {
public:
    Material(const Loader& loader, const Mat4f& transformation = Mat4f::Identity())
        : _loader(loader), _transformation(transformation) {}

    virtual ~Material() {}

    const Mat4f &getTransformation() const {
        return _transformation;
    }

protected:
    const Loader& _loader;
    const Mat4f _transformation;
};

#endif //CRENDERER_MATERIAL_HPP
