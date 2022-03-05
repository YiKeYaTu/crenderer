//
// Created by lc on 2022/3/4.
//

#ifndef CRENDERER_PROPERTY_HPP
#define CRENDERER_PROPERTY_HPP

#include "../../Vector.hpp"

class Property {
public:
    virtual Vec3f BRDF(const Vec3f &kd, const Vec3f &ks, const Vec3f &e, const Vec3f& wi, const Vec3f& wo, const Vec3f& n) = 0;
    virtual std::pair<Vec3f, double> sample(const Vec3f& wi, const Vec3f& n) = 0;
};

#endif //CRENDERER_PROPERTY_HPP
