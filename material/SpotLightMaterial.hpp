//
// Created by lc on 2022/3/2.
//

#ifndef CRENDERER_LIGHTMATERIAL_HPP
#define CRENDERER_LIGHTMATERIAL_HPP

#include "../Vector.hpp"
#include "../utils/util.hpp"
#include "CommonLightMaterial.hpp"

class SpotlightMaterial: public CommonLightMaterial {
public:
    static double defaultTheta;

    SpotlightMaterial() = default;
    SpotlightMaterial(const Vec3f &kd, const Vec3f &ks, const Vec3f &e, double theta = defaultTheta): CommonLightMaterial(kd, ks, e), theta(theta) {}

    const double theta;
    const Vec3f zeroEmission;

    virtual const Vec3f getEmission(const Vec3f& light2point, const Vec3f& normal) const override {
        double c = cos(light2point, normal);

        if (c < std::cos(theta)) {
            if (c < 0) {
                return zeroEmission;
            } else {
                return emission_ * std::pow(c / std::cos(theta), 40);
            }
        }

        return emission_;
    }

};

double SpotlightMaterial::defaultTheta =  20.0f / 180 * util::MY_PI;

#endif //CRENDERER_LIGHTMATERIAL_HPP
