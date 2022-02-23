//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_BVH_HPP
#define CRENDERER_BVH_HPP

#include "../geometry/Triangle.hpp"
#include "./Bounds3.hpp"

class BVH {
private:
    std::shared_ptr<BVH> left_;
    std::shared_ptr<BVH> right_;
    std::shared_ptr<Geometry> geometry_;
    Bounds3 bounds3_;

    static std::shared_ptr<BVH> buildBVH_(std::vector<std::shared_ptr<Geometry>>& geometries, int axis, int lo, int hi) {
        if (lo >= hi) {
            return nullptr;
        }

        if (lo == hi - 1) {
            return std::make_shared<BVH>(nullptr, nullptr, geometries[lo], geometries[lo]->bounds3);
        }

        std::shared_ptr<BVH> bvh = std::make_shared<BVH>();
        std::vector<Bounds3> bounds3s;

        std::transform(geometries.begin() + lo, geometries.begin() + hi, std::inserter(bounds3s, bounds3s.begin()), [](std::shared_ptr<Geometry>& g) {
            return g->bounds3;
        });
        std::sort(geometries.begin() + lo, geometries.begin() + hi, [axis](std::shared_ptr<Geometry>& g1, std::shared_ptr<Geometry>& g2) {
            return g1->centroid[axis] < g2->centroid[axis];
        });

        int mid = (lo + hi) / 2;

        bvh->left_ = buildBVH_(geometries, (axis + 1) % 3, lo, mid);
        bvh->right_ = buildBVH_(geometries, (axis + 1) % 3, mid, hi);
        bvh->bounds3_ = Bounds3::computeBounds3(bounds3s);

        return bvh;
    }

public:
    BVH() { }
    BVH(const std::shared_ptr<BVH> &left, const std::shared_ptr<BVH> &right, const std::shared_ptr<Geometry> &geometry,
        const Bounds3 &bounds3) : left_(left), right_(right), geometry_(geometry), bounds3_(bounds3) {}

    static std::shared_ptr<BVH> buildBVH(std::vector<std::shared_ptr<Geometry>>& geometries) {
        return buildBVH_(geometries, 0, 0, geometries.size());
    }

    Bounds3 bounds3() { return bounds3_; }
    std::shared_ptr<BVH> left() { return left_; }
    std::shared_ptr<BVH> right() { return right_; }
};

#endif //CRENDERER_BVH_HPP
