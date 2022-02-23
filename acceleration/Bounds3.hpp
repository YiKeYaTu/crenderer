//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_BOUNDS3_HPP
#define CRENDERER_BOUNDS3_HPP

class Bounds3 {
public:
    Vec3f min;
    Vec3f max;
    friend std::ostream &operator<<(std::ostream &os, const Bounds3& bounds3) {
        os << bounds3.min << std::endl;
        os << bounds3.max << std::endl;
        return os;
    }
    Bounds3() = default;
    static Bounds3 computeBounds3(const std::vector<Vec3f>& vectors) {
        Vec3f minBounds3 = Vec3f::min((const std::vector<Vec<double>> &) vectors);
        Vec3f maxBounds3 = Vec3f::max((const std::vector<Vec<double>> &) vectors);
        return Bounds3(minBounds3, maxBounds3);
    }
    static Bounds3 computeBounds3(const std::vector<Bounds3>& bounds3s) {
        Vec3f minBounds3 = Vec3f::maxVec3f();
        Vec3f maxBounds3 = Vec3f::minVec3f();
        for (const auto& bounds3 : bounds3s) {
            minBounds3 = Vec3f::min({ minBounds3, bounds3.min });
            maxBounds3 = Vec3f::max({ maxBounds3, bounds3.max });
        }
        return Bounds3(minBounds3, maxBounds3);
    }

    Intersection intersect() {
        return Intersection();
    }
private:
    Bounds3(const Vec3f& minBounds3, const Vec3f& maxBounds3)
            : min(minBounds3), max(maxBounds3) {}
};

#endif //CRENDERER_BOUNDS3_HPP
