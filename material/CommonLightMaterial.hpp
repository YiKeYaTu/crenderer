//
// Created by lc on 2022/3/2.
//

#ifndef CRENDERER_COMMONLIGHTMATERIAL_HPP
#define CRENDERER_COMMONLIGHTMATERIAL_HPP

#include "../Vector.hpp"
#include "CommonMaterial.hpp"

class CommonLightMaterial: public CommonMaterial {
public:
    CommonLightMaterial(const Vec3f &kd, const Vec3f &ks, const Vec3f &e): CommonMaterial(kd, ks, e) {}

    virtual const Vec3f getEmission(const Vec3f& point2lightVec, const Vec3f& normal) const {
        if (cos(point2lightVec, normal) <= 0) {
            return Vec3f();
        }
        return emission_;
    }

    virtual bool hasEmission() const override {
        return true;
    }
};

#endif //CRENDERER_COMMONLIGHTMATERIAL_HPP
