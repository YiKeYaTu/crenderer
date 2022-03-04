//
// Created by lc on 2022/3/2.
//

#ifndef CRENDERER_LIGHTMATERIAL_HPP
#define CRENDERER_LIGHTMATERIAL_HPP

#include "Vector.hpp"
#include "utils/util.hpp"

class SpotlightMaterial {
public:
    const double theta = 45 / 180 * util::MY_PI;
    const Vec3f normal;

    const Vec3f& e(Vec3f& point2light) const {
        double c = cos(point2light, -normal);

        if (c > std::cos(theta)) {
            return Vec3f(0, 0, 0);
        }

        return e;l
    }
};

#endif //CRENDERER_LIGHTMATERIAL_HPP
