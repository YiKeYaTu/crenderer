//
// Created by lc on 2022/3/4.
//

#ifndef CRENDERER_DIFFUSEPROPERTY_HPP
#define CRENDERER_DIFFUSEPROPERTY_HPP

#include "./Property.hpp"
#include "../../utils/util.hpp"

class DiffuseProperty : public Property {
    virtual Vec3f BRDF(const Vec3f &kd, const Vec3f &ks, const Vec3f &e, const Vec3f& wi, const Vec3f& wo, const Vec3f& normal) override {
        double cosWiNormal = wi.dot(normal) / wi.norm() / normal.norm();
        if (cosWiNormal <= 0) {
            return Vec3f();
        }
        return kd / util::MY_PI;
    }

    virtual std::pair<Vec3f, double> sample(const Vec3f& wi, const Vec3f& normal) {
        double ratio = 1.0f / 2.0f / util::MY_PI;
        // ensure that the cos of normal and sampled direction bigger than 0
        double a = 2 * util::MY_PI * util::getRandom01();
        double b = 2 * util::MY_PI * util::getRandom01();

        double x = 1 * std::cos(a);
        double y = 1 * std::sin(a);
        double z = sqrt(x * x + y * y) * tan(b);

        Vec3f direction(x, y, z);

        if (cos(direction, normal) < 0) {
            return std::pair(-direction.normalized(), ratio);
        }

        return std::pair(direction.normalized(), ratio);
    }
};

#endif //CRENDERER_DIFFUSEPROPERTY_HPP
