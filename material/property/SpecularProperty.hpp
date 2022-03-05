//
// Created by lc on 2022/3/4.
//

#ifndef CRENDERER_SPECULARPROPERTY_HPP
#define CRENDERER_SPECULARPROPERTY_HPP

#include "./Property.hpp"
#include "../../utils/util.hpp"

class SpecularProperty : public Property {
public:
    double sigma = 10 / 180 * util::MY_PI;

    virtual Vec3f BRDF(const Vec3f &kd, const Vec3f &ks, const Vec3f &e, const Vec3f& wi, const Vec3f& wo, const Vec3f& normal) override {
        if (cos(wi, normal) < 0 || cos(wo, normal) < 0) { return Vec3f(); }
        double c = cos(wi + wo, normal);
        return kd * pow(c, 150);
    }

    virtual std::pair<Vec3f, double> sample(const Vec3f& wi, const Vec3f& normal) {
        double pdf = 1.0;
        Vec3f wo(normal * 2 * (wi.dot(normal))  - wi);
        return std::pair(wo.normalized(), pdf);
    }
};

#endif //CRENDERER_SPECULARPROPERTY_HPP
