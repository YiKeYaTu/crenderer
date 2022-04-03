//
// Created by lc on 2022/4/3.
//

#ifndef CRENDERER_PRIMITIVE_HPP
#define CRENDERER_PRIMITIVE_HPP

#include <object/Object.hpp>

class Primitive: Object {
public:
    Primitive(): Object(ObjectCategory::PRIMITIVE) {}
    virtual Vec3f calcNormalByPoint(Vec3f& point) const = 0;
};

#endif //CRENDERER_PRIMITIVE_HPP
