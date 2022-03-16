//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_MESHTRIANGLE_HPP
#define CRENDERER_MESHTRIANGLE_HPP

#include <vector>
#include <fstream>
#include <string>
#include "../Ray.hpp"
#include "../Material.hpp"
#include "Triangle.hpp"
#include "../acceleration/BVH.hpp"
#include "../utils/OBJLoader.hpp"
#include "../material/CommonMaterial.hpp"

class MeshTriangle : public Object {
public:
    static MeshTriangle loadObjectFromObjFile(const std::string &, const CommonMaterial& material);
    MeshTriangle() = default;
    MeshTriangle(const std::vector<Vec3f> &vertexes, const std::vector<std::size_t> indexes, const CommonMaterial* material) {
        material_ = material;
        for (int i = 0; i < static_cast<int>(indexes.size()) - 2; i += 3) {
            auto a = vertexes[indexes[i + 0]];
            auto b = vertexes[indexes[i + 1]];
            auto c = vertexes[indexes[i + 2]];
            triangles_.push_back(new Triangle(a, b, c, material_));
        }
        bvh_ = BVH::buildBVH(triangles_);
        for (Object* object : triangles_) {
            centroid_ += object->centroid();
        }
        centroid_ /= triangles_.size();
        bounds3_ = Bounds3::computeBounds3(vertexes);
        type_ = MESH_TRIANGLE;
    }
    MeshTriangle(const std::vector<Vec3f> &vertexes, const std::vector<std::size_t> indexes)
    : MeshTriangle(vertexes, indexes, nullptr) {}

    virtual Intersection intersect(const Ray& ray) const override { return BVH::intersect(ray, bvh_); }
    const std::vector<Object*>& triangles() const { return triangles_; }

    virtual const Vec3f computeNormal(const Vec3f& p) const override {
        return triangles_[0]->computeNormal(p);
    }

    std::shared_ptr<BVH> bvh() { return bvh_; }

private:
    std::vector<Object*> triangles_;
    std::shared_ptr<BVH> bvh_;
};

MeshTriangle MeshTriangle::loadObjectFromObjFile(const std::string &fileName, const CommonMaterial& material) {
    OBJLoader loader(fileName);

    try {
        loader.loadOBJFile();
    } catch (std::exception* e) {
        std::cout << e->what();
    }

    return MeshTriangle(loader.vertexes(), loader.indexes(), &material);
}

#endif //CRENDERER_MESHTRIANGLE_HPP
