//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_TRIANGLE_HPP
#define CRENDERER_TRIANGLE_HPP

#include <object/Primitive.hpp>
#include <core/Vec.hpp>
#include <core/Vertex.hpp>

class Triangle: public Primitive<3> {
public:
    using Primitive<3>::Primitive;

    Vec3f& v0() { return _vertexes[0]; }
    Vec3f& v1() { return _vertexes[1]; }
    Vec3f& v2() { return _vertexes[2]; }

    Vec3f& e0() { return _edges[0]; }
    Vec3f& e1() { return _edges[1]; }
    Vec3f& e2() { return _edges[2]; }

    virtual Vec3f calcNormalByPoint(Vec3f& point) const override {}
};

#endif //CRENDERER_TRIANGLE_HPP
