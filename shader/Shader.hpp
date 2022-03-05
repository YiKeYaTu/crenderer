//
// Created by lc on 2022/3/1.
//

#ifndef CRENDERER_SHADER_HPP
#define CRENDERER_SHADER_HPP

#include "../Vector.hpp"
#include "../Ray.hpp"
#include "../Intersection.hpp"

class Shader {
public:
    virtual Vec3f shade(const Ray& camera2hitPoint, int depth, const Intersection* iterObject, const Intersection* iterVolume) = 0;
};

#endif //CRENDERER_SHADER_HPP
