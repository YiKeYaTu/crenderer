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

    virtual const Vec3f getEmission(const Vec3f& pointPos, const Vec3f& lightPos, const Vec3f& normal, const Object* light) const override {
        double a = normal.x(), b = normal.y(), c = normal.z();
        double x = pointPos.x(), y = pointPos.y(), z = pointPos.z();
        double x0 = light->centroid().x(), y0 = light->centroid().y(), z0 = light->centroid().z();

        double t = ( ( a*x0 + b*y0 + c*z0 ) - ( a*x + b*y + c*z )  )
                / ( normal.norm() * normal.norm() );

        double x1 = x + a*t, y1 = y + b*t , z1 = z + c*t;

        auto bounds3 = light->bounds3();

        // The projected point is on the surface
        if (x1 >= bounds3.min().x() &&
            y1 >= bounds3.min().y() &&
            z1 >= bounds3.min().z() &&
            x1 <= bounds3.max().x() &&
            y1 <= bounds3.max().y() &&
            z1 <= bounds3.max().z()
        ) {
            return emission_;
        }

        x1 = util::clamp(bounds3.min().x(), bounds3.max().x(), x1);
        y1 = util::clamp(bounds3.min().y(), bounds3.max().y(), y1);
        z1 = util::clamp(bounds3.min().z(), bounds3.max().z(), z1);

        Vec3f nearestPointOnSurface{ x1, y1, z1 };

        if (cos(pointPos - nearestPointOnSurface, normal) >= cos(theta)) {
            return emission_;
        }

        return emission_ * pow( fmax( cos(pointPos - nearestPointOnSurface, normal), 0 ), 100 );
    }

};

double SpotlightMaterial::defaultTheta =  20.0f / 180 * util::MY_PI;

#endif //CRENDERER_LIGHTMATERIAL_HPP
