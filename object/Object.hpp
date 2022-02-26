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

inline float get_random_float()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

    return dist(rng);
}

Vec3f toWorld(const Vec3f& a, const Vec3f& N) {
    Vec3f B, C;
    if (std::fabs(N.x()) > std::fabs(N.y())) {
        float invLen = 1.0f / std::sqrt(N.x() * N.x() + N.z() * N.z());
        C = Vec3f(N.z() * invLen, 0.0f, -N.x() * invLen);
    }
    else {
        float invLen = 1.0f / std::sqrt(N.y() * N.y() + N.z() * N.z());
        C = Vec3f(0.0f, N.z() * invLen, -N.y() * invLen);
    }
    B = cross(C, N);
    return B * a.x()  + C * a.y() + N * a.z();
}

extern class Intersection;

class Object {
public:
    virtual Intersection intersect(const Ray& ray) const = 0;

    Object() {}

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



        /*float x_1 = get_random_float(), x_2 = get_random_float();
        float z = std::fabs(1.0f - 2.0f * x_1);
        float r = std::sqrt(1.0f - z * z), phi = 2 * util::MY_PI * x_2;
        Vec3f localRay(r * std::cos(phi), r * std::sin(phi), z);
        return { toWorld(localRay, normal_), ratio };*/
    }

    const Bounds3& bounds3() const { return bounds3_; };
    const Vec3f& centroid() const { return centroid_; }
    const Vec3f& normal() const { return normal_; }
    const Material* material() const { return material_; }

protected:
    Bounds3 bounds3_;
    Vec3f centroid_;
    Vec3f normal_;
    mutable const Material* material_;
};

#endif //CRENDERER_GEOMETRY_HPP
