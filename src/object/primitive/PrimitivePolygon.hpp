//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_PRIMITIVEPOLYGON_HPP
#define CRENDERER_PRIMITIVEPOLYGON_HPP

#include "object/primitive/Primitive.hpp"
#include "core/Vec.hpp"
#include <vector>

class _PrimitivePolygon: public Primitive {
public:
    using Primitive::Primitive;
};

template <unsigned int NumVertexes>
class PrimitivePolygon: public _PrimitivePolygon {
    static_assert(NumVertexes > 2);
protected:
    typedef unsigned int sizeType;
public:
    virtual Vec3f getNormalByVertex(sizeType vertexIdx) const { assert(vertexIdx < NumVertexes); return _normals[vertexIdx]; }
    static const sizeType numVertexes = NumVertexes;
    PrimitivePolygon(const Vec3f* vertexes, const sizeType* indexes, const Vec3f* normals = nullptr, const Vec2f* texCoords
    = nullptr) {
        for (int i = 0; i < NumVertexes; ++i) {
            _vertexes[i] = vertexes[indexes[i]];
            _edges[i] = (i == NumVertexes - 1)
                ? vertexes[indexes[0]] - vertexes[indexes[i]]
                : vertexes[indexes[i + 1]] - vertexes[indexes[i]];

            if (texCoords) {
                _texCoords[i] = texCoords[indexes[i]];
            }
            if (normals) {
                _normals[i] = normals[indexes[i]];
            }
        }

        if (!normals) {
            for (int i = 0; i < NumVertexes; ++i) {
                _normals[i] = crossProduct(_edges[0], _edges[1]);
            }
        }
    }

protected:
    std::vector<Vec3f> _vertexes = std::vector<Vec3f>(NumVertexes);
    std::vector<Vec3f> _normals = std::vector<Vec3f>(NumVertexes);
    std::vector<Vec2f> _texCoords = std::vector<Vec2f>(NumVertexes);
    std::vector<Vec3f> _edges = std::vector<Vec3f>(NumVertexes);
};

#endif //CRENDERER_PRIMITIVEPOLYGON_HPP
