//
// Created by lc on 2022/3/3.
//

#ifndef CRENDERER_SPHERE_HPP
#define CRENDERER_SPHERE_HPP

#include "./Object.hpp"

class Sphere: public Object {
public:
    Sphere(const Vec3f& c, const double r, const CommonMaterial* material): center(c), radius(r) {
        centroid_ = center;
        material_ = material;
        bounds3_ = Bounds3(Vec3f(c.x() - r, c.y() - r, c.z() - r), Vec3f(c.x() + r, c.y() + r, c.z() + r));
        type_ = SPHERE;
    }

    virtual Intersection intersect(const Ray& ray) const {
        const Vec3f& d = ray.direction;
        const Vec3f& p = ray.origin;

        double a = d.norm() * d.norm();
        double b = 2 * d.dot(p - center);
        double c = (p - center).dot(p - center) - radius*radius;

        double delta = b*b - 4*a*c;

        double tMin, tMax;

        if (delta < 0) {
            return Intersection();
        } else if (delta == 0) {
            tMin = tMax = -b / (2*a);
        } else {
            tMin = ( -b - sqrt(delta) ) / (2*a);
            tMax = ( -b + sqrt(delta) ) / (2*a);
        }

        if (tMin <= 0 && tMax <= 0) {
            return Intersection();
        }

        Vec3f hitPoint = ray.origin + ray.direction * tMin;
        if (tMin < 0) {
            Vec3f hitPoint = ray.origin + ray.direction * tMax;
        }

        return Intersection(tMin, tMax, -1, -1, -1, (ray.direction * tMin).norm(), hitPoint, this);
    }

    virtual const Vec3f computeNormal(const Vec3f& p) const override {
        return (p - centroid_).normalized();
    }

    const Vec3f center;
    const double radius;
};

#endif //CRENDERER_SPHERE_HPP
