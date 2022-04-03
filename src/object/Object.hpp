//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_OBJECT_HPP
#define CRENDERER_OBJECT_HPP

#include <core/Vec.hpp>

class Object {
public:
    static enum ObjectCategory {
        PRIMITIVE,
        MESH
    };

    Object() = delete;
    Object(const ObjectCategory& objectCategory): _objectCategory(objectCategory) {}
    const ObjectCategory& objectCategory() const { return _objectCategory; }

protected:
    ObjectCategory _objectCategory;
};

#endif //CRENDERER_OBJECT_HPP
