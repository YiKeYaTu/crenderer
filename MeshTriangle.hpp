//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_MESHTRIANGLE_HPP
#define CRENDERER_MESHTRIANGLE_HPP

#include <vector>
#include <fstream>
#include <string>
#include "Ray.hpp"
#include "Material.hpp"
#include "geometry/Triangle.hpp"
#include "acceleration/BVH.hpp"
#include "utils/OBJLoader.hpp"

class MeshTriangle {
public:
    static MeshTriangle loadObjectFromObjFile(const std::string &, const Material& material);
    MeshTriangle() = default;
    MeshTriangle(const std::vector<Vec3f> &vertexes, const std::vector<std::size_t> indexes, const Material& material)
    : MeshTriangle(vertexes, indexes) { material_ = material; }
    MeshTriangle(const std::vector<Vec3f> &vertexes, const std::vector<std::size_t> indexes) {
        for (int i = 0; i < static_cast<int>(indexes.size()) - 2; i += 3) {
            auto a = vertexes[indexes[i + 0]];
            auto b = vertexes[indexes[i + 1]];
            auto c = vertexes[indexes[i + 2]];
            triangles_.push_back(std::make_shared<Triangle>(Triangle(a, b, c)));
        }
        bvh_ = BVH::buildBVH(triangles_);
    }

    Intersection intersect(const Ray& ray) const { return intersect_(ray, bvh_); }
    std::vector<std::shared_ptr<Geometry>> triangles() const { return triangles_; }
    Material material() const { return material_; }
    Vec3f centroid() const {
        Vec3f centroid;
        for (const auto& item : triangles_) {
            centroid += item->centroid;
        }
        return centroid / triangles_.size();
    }

private:
    std::vector<std::shared_ptr<Geometry>> triangles_;
    std::shared_ptr<BVH> bvh_;
    Material material_;

    static Intersection intersect_(const Ray& ray, const std::shared_ptr<BVH> bvh) {
        if (!bvh) {
            return Intersection();
        }

        if (!bvh->left() && !bvh->right()) {
            return bvh->geometry()->intersect(ray);
        }

        Intersection intersection = bvh->bounds3().intersect(ray);

        if (intersection.happened()) {

            Intersection leftIntersection = intersect_(ray, bvh->left());
            Intersection rightIntersection = intersect_(ray, bvh->right());

            if (leftIntersection.happened() && rightIntersection.happened()) {
                return leftIntersection.t() < rightIntersection.t() ? leftIntersection : rightIntersection;
            }

            if (leftIntersection.happened()) {
                return leftIntersection;
            } else {
                return rightIntersection;
            }
        }

        return intersection;
    }
};

MeshTriangle MeshTriangle::loadObjectFromObjFile(const std::string &fileName, const Material& material) {
    OBJLoader loader(fileName);

    try {
        loader.loadOBJFile();
    } catch (std::exception* e) {
        std::cout << e->what();
    }

    return MeshTriangle(loader.vertexes(), loader.indexes(), material);
}

#endif //CRENDERER_MESHTRIANGLE_HPP
