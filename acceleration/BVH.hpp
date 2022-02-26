//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_BVH_HPP
#define CRENDERER_BVH_HPP

#include "../object/Triangle.hpp"
#include "./Bounds3.hpp"
#include <algorithm>

class BVH {
private:
    std::shared_ptr<BVH> left_;
    std::shared_ptr<BVH> right_;
    std::shared_ptr<Object> geometry_;
    Bounds3 bounds3_;

    static std::shared_ptr<BVH> buildBVH_(std::vector<std::shared_ptr<Object>>& objects, int axis, int lo, int hi) {
        if (lo >= hi) {
            return nullptr;
        }

        if (lo == hi - 1) {
            return std::make_shared<BVH>(nullptr, nullptr, objects[lo], objects[lo]->bounds3());
        }

        std::shared_ptr<BVH> bvh = std::make_shared<BVH>();
        std::vector<Bounds3> bounds3s;

        std::transform(objects.begin() + lo, objects.begin() + hi, std::inserter(bounds3s, bounds3s.begin()), [](std::shared_ptr<Object>& object) {
            return object->bounds3();
        });
        std::sort(objects.begin() + lo, objects.begin() + hi, [axis](std::shared_ptr<Object>& object1, std::shared_ptr<Object>& object2) {
            return object1->centroid()[axis] < object2->centroid()[axis];
        });

        int mid = (lo + hi) / 2;

        bvh->left_ = buildBVH_(objects, (axis + 1) % 3, lo, mid);
        bvh->right_ = buildBVH_(objects, (axis + 1) % 3, mid, hi);
        bvh->bounds3_ = Bounds3::computeBounds3(bounds3s);

        return bvh;
    }

public:
    BVH() { }
    BVH(const std::shared_ptr<BVH> &left, const std::shared_ptr<BVH> &right, const std::shared_ptr<Object> &geometry,
        const Bounds3 &bounds3) : left_(left), right_(right), geometry_(geometry), bounds3_(bounds3) {}

    static std::shared_ptr<BVH> buildBVH(std::vector<std::shared_ptr<Object>>& objects) {
        return buildBVH_(objects, 0, 0, objects.size());
    }

    std::shared_ptr<Object> geometry() { return geometry_; }
    Bounds3 bounds3() { return bounds3_; }
    std::shared_ptr<BVH> left() { return left_; }
    std::shared_ptr<BVH> right() { return right_; }

    static Intersection intersect(const Ray& ray, const std::shared_ptr<BVH> bvh) {
        if (!bvh) {
            return Intersection();
        }

        if (!bvh->left() && !bvh->right()) {
            return bvh->geometry()->intersect(ray);
        }

        Intersection intersection = bvh->bounds3().intersect(ray);

        if (intersection.happened()) {

            Intersection leftIntersection = intersect(ray, bvh->left());
            Intersection rightIntersection = intersect(ray, bvh->right());

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

#endif //CRENDERER_BVH_HPP
