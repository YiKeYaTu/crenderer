//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_TRIANGLE_HPP
#define CRENDERER_TRIANGLE_HPP

#include "../Vector.hpp"

class Triangle {
public:
    Triangle(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2):
        v0(v0), v1(v1), v2{v2},
        e0(v1 - v0), e1(v2 - v1), e2(v0 - v2),
        n(cross(e0, e1)){ }
private:
    Vec3f v0, v1, v2;
    Vec3f e0, e1, e2;
    Vec3f n;
};

#endif //CRENDERER_TRIANGLE_HPP
