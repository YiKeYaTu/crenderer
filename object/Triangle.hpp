//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_TRIANGLE_HPP
#define CRENDERER_TRIANGLE_HPP

#include "../Vector.hpp"
#include "Object.hpp"
#include "../acceleration/Bounds3.hpp"
#include "../material/CommonMaterial.hpp"

class Triangle: public Object {
    friend std::ostream &operator<<(std::ostream &os, const Triangle &t) {
        os << "v0: " << t.v0_ << std::endl;
        os << "v1: " << t.v1_ << std::endl;
        os << "v2: " << t.v2_ << std::endl;

        return os;
    }
public:
    explicit Triangle(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2): Triangle(v0, v1, v2, nullptr) {}
    explicit Triangle(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, const CommonMaterial* material):
        v0_(v0), v1_(v1), v2_{v2},
        e0_(v1 - v0), e1_(v2 - v1), e2_(v0 - v2) {
            material_ = material;
            centroid_ = (v0 + v1 + v2) / 3;
            bounds3_ = Bounds3::computeBounds3({ v0, v1, v2 });
            normal_ = crossProduct(e0_, e1_);
        };

    virtual const Vec3f computeNormal(const Vec3f& p) const override {
        return normal_;
    }

    virtual Intersection intersect(const Ray& ray) const override {
        Vec3f o = ray.origin;
        Vec3f d = ray.direction;
        Vec3f v13 = v0_ - v2_;
        Vec3f v23 = v1_ - v2_;

        double c11 = -d.x(), c12 = v13.x(), c13 = v23.x();
        double c21 = -d.y(), c22 = v13.y(), c23 = v23.y();
        double c31 = -d.z(), c32 = v13.z(), c33 = v23.z();

        double D = c11*c22*c33 + c12*c23*c31 + c13*c21*c32 - c13*c22*c31 - c11*c23*c32 - c12*c21*c33;

        double r1 = o.x() - v2_.x();
        double r2 = o.y() - v2_.y();
        double r3 = o.z() - v2_.z();

        double D1 = r1*c22*c33 + c12*c23*r3 + c13*r2*c32 - c13*c22*r3 - r1*c23*c32 - c12*r2*c33;
        double D2 = c11*r2*c33 + r1*c23*c31 + c13*c21*r3 - c13*r2*c31 - c11*c23*r3 - r1*c21*c33;
        double D3 = c11*c22*r3 + c12*r2*c31 + r1*c21*c32 - r1*c22*c31 - c11*r2*c32 - c12*c21*r3;

        double t = D1 / D;
        double a = D2 / D, b = D3 / D, c = 1.0f - a - b;

        double eps = 1e-15;

        if (t < 0 || a + eps < 0 || b + eps < 0 || c + eps < 0) {
            return Intersection();
        }

        Vec3f hitPoint(v0_*a + v1_*b + v2_*c);
        double distance = (ray.direction*t).norm();

        return Intersection(t, std::numeric_limits<double>::max(), a, b, c, distance, hitPoint + normal_.normalized() * 0.00001, this);
    };
private:
    Vec3f v0_, v1_, v2_;
    Vec3f e0_, e1_, e2_;
    Vec3f n_;
    void* belongsTo_;
};

#endif //CRENDERER_TRIANGLE_HPP
