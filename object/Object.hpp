//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_GEOMETRY_HPP
#define CRENDERER_GEOMETRY_HPP

#include <cmath>
#include "../utils/util.hpp"
#include "../Vector.hpp"
#include "../acceleration/Bounds3.hpp"
#include "../Ray.hpp"
#include "../material/CommonMaterial.hpp"
#include "ObjectTypes.hpp"

extern class Intersection;

class Object {
public:
    virtual Intersection intersect(const Ray& ray) const = 0;

    Object() {}

    int type() { return type_; }

    std::pair<Vec3f, double> sampleDirection() const {
        double ratio = 1.0f / 2.0f / util::MY_PI;
        // ensure that the cos of normal and sampled direction bigger than 0
        double a = 2 * util::MY_PI * util::getRandom01();
        double b = 2 * util::MY_PI * util::getRandom01();

        double x = 1 * std::cos(a);
        double y = 1 * std::sin(a);
        double z = sqrt(x * x + y * y) * tan(b);

        Vec3f direction(x, y, z);

        if (cos(direction, normal_) < 0) {
            return std::pair(-direction.normalized(), ratio);
        }

        return std::pair(direction.normalized(), ratio);
    }

    virtual const Vec3f computeNormal(const Vec3f& p) const = 0;

    const Bounds3& bounds3() const { return bounds3_; };
    const Vec3f& centroid() const { return centroid_; }
    const Vec3f normal() const { return normal_; }
    const Vec3f normal(const Vec3f& p) const { return computeNormal(p); }
    const CommonMaterial* material() const { return material_; }

protected:
    Bounds3 bounds3_;
    Vec3f centroid_;
    Vec3f normal_;
    int type_;
    mutable const CommonMaterial* material_;
};

#endif //CRENDERER_GEOMETRY_HPP
