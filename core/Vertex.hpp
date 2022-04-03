//
// Created by lc on 2022/4/2.
//

#ifndef CRENDERER_VERTEX_HPP
#define CRENDERER_VERTEX_HPP

#include <type_traits>
#include <core/Vec.hpp>

struct Vertex {
    Vec3f vertex;
    Vec3f normal;
    Vec2f texCoord;
    Vertex(const Vertex& v)
        : vertex(v.vertex), normal(v.normal), texCoord(v.texCoord) {}
    Vertex(const Vec3f* vertex, const Vec3f* normal, const Vec2f* texCoord)
        : vertex(*vertex), normal(*normal), texCoord(texCoord ? *texCoord: Vec2f()) {}
};

#endif //CRENDERER_VERTEX_HPP
