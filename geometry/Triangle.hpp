//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_TRIANGLE_HPP
#define CRENDERER_TRIANGLE_HPP

#include "../core/Vector.hpp"
#include "Geometry.hpp"
#include "../acceleration/Bounds3.hpp"

class Triangle: public Geometry {
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t) {
        os << "v0: " << t.v0_ << std::endl;
        os << "v1: " << t.v1_ << std::endl;
        os << "v2: " << t.v2_ << std::endl;

        return os;
    }
public:
    explicit Triangle(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2):
        v0_(v0), v1_(v1), v2_{v2},
        e0_(v1 - v0), e1_(v2 - v1), e2_(v0 - v2),
        n_(cross(e0_, e1_)),
        Geometry(
                (v0 + v1 + v2) / 3,
                Bounds3::computeBounds3({ v0, v1, v2 })
                ) { };

    virtual Intersection intersect() override {
        return Intersection();
    };
private:
    Vec3f v0_, v1_, v2_;
    Vec3f e0_, e1_, e2_;
    Vec3f n_;
};

#endif //CRENDERER_TRIANGLE_HPP
