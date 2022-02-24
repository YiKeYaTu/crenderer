//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_BOUNDS3_HPP
#define CRENDERER_BOUNDS3_HPP

#include "../Intersection.hpp"
#include "../Ray.hpp"

class Bounds3 {
public:
    Bounds3() = default;
    Bounds3(const Vec3f& minBounds3, const Vec3f& maxBounds3)
            : min_(minBounds3), max_(maxBounds3) {}

    Vec3f min() const { return min_; }
    Vec3f max() const { return max_; }

    friend std::ostream &operator<<(std::ostream &os, const Bounds3& bounds3) {
        os << bounds3.min_ << std::endl;
        os << bounds3.max_ << std::endl;
        return os;
    }
    static Bounds3 computeBounds3(const std::vector<Vec3f>& vectors) {
        Vec3f minBounds3 = Vec3f::min((const std::vector<Vec<double>> &) vectors);
        Vec3f maxBounds3 = Vec3f::max((const std::vector<Vec<double>> &) vectors);
        return Bounds3(minBounds3, maxBounds3);
    }
    static Bounds3 computeBounds3(const std::vector<Bounds3>& bounds3s) {
        Vec3f minBounds3 = Vec3f::maxVec3f();
        Vec3f maxBounds3 = Vec3f::minVec3f();
        for (const auto& bounds3 : bounds3s) {
            minBounds3 = Vec3f::min({ minBounds3, bounds3.min() });
            maxBounds3 = Vec3f::max({ maxBounds3, bounds3.max() });
        }
        return Bounds3(minBounds3, maxBounds3);
    }
    // compute whether ray intersect with the bounding box
    Intersection intersect(const Ray& ray) {
        double tMin = std::numeric_limits<double>::lowest();
        double tMax = std::numeric_limits<double>::max();

        double t1, t2;

        for (int i = 0; i < 3; ++i) {
            t1 = (min_[i] - ray.origin[i]) * ray.recDirection[i];
            t2 = (max_[i] -  ray.origin[i]) * ray.recDirection[i];

            if (t1 < 0 && t2 < 0) { return Intersection(); }

            tMin = std::fmax(std::fmin(t1, t2), tMin);
            tMax = std::fmin(std::fmax(t1, t2), tMax);
        }

        if (tMin <= tMax) {
            return Intersection(true);
        }

        return Intersection();
    }
private:

    Vec3f min_;
    Vec3f max_;
};

#endif //CRENDERER_BOUNDS3_HPP
