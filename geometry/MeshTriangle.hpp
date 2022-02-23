//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_MESHTRIANGLE_HPP
#define CRENDERER_MESHTRIANGLE_HPP

#include <vector>
#include <fstream>
#include <string>
#include "Triangle.hpp"
#include "../acceleration/BVH.hpp"
#include "../utils/OBJLoader.hpp"
#include "../Ray.hpp"

class MeshTriangle {
public:
    static MeshTriangle loadFromObjFile(const std::string &);

    MeshTriangle(const std::vector<Vec3f> &vertexes, const std::vector<std::size_t> indexes) {
        for (int i = 0; i < static_cast<int>(indexes.size()) - 2; i += 3) {
            auto a = vertexes[indexes[i + 0]];
            auto b = vertexes[indexes[i + 1]];
            auto c = vertexes[indexes[i + 2]];
            triangles.push_back(std::make_shared<Triangle>(Triangle(a, b, c)));
        }
        bvh = BVH::buildBVH(triangles);
    }

    Intersection intersect(Ray& ray, std::shared_ptr<BVH> bvh) {
        Intersection intersection = bvh->bounds3().intersect();

        if (!bvh) {
            return intersection;
        }

        if (intersection.happened()) {
            Intersection leftIntersection = intersect(ray, bvh->left());
            Intersection rightIntersection = intersect(ray, bvh->right());

            if (leftIntersection.happened() && rightIntersection.happened()) {
                return leftIntersection.t() < rightIntersection.happened() ? leftIntersection : rightIntersection;
            }

            return rightIntersection;
        }

        return intersection;
    }
private:
    std::vector<std::shared_ptr<Geometry>> triangles;
    std::shared_ptr<BVH> bvh;
};

MeshTriangle MeshTriangle::loadFromObjFile(const std::string &fileName) {
    OBJLoader loader(fileName);

    try {
        loader.loadOBJFile();
    } catch (std::exception* e) {
        std::cout << e->what();
    }

    return MeshTriangle(loader.vertexes(), loader.indexes());
}

#endif //CRENDERER_MESHTRIANGLE_HPP
