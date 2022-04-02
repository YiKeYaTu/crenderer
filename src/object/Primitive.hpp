//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_PRIMITIVE_HPP
#define CRENDERER_PRIMITIVE_HPP

#include <object/Object.hpp>
#include <core/Vec.hpp>
#include <vector>

template <unsigned int NumVertexes>
class Primitive: public Object {
    static_assert(NumVertexes > 2);
protected:
    typedef unsigned int sizeType;
public:
    virtual Vec3f calcNormalByPoint(Vec3f& point) const = 0;
    virtual Vec3f getNormalByIndex(sizeType idx) const { assert(idx < NumVertexes); return _normals[idx]; }

    static const sizeType numVertexes = NumVertexes;
    Primitive(const Vec3f* vertexes, const sizeType* indexes, const Vec3f* normals = nullptr, const Vec2f* texCoords
    = nullptr) {
        for (int i = 0; i < NumVertexes; ++i) {
            _vertexes[i] = vertexes[indexes[i]];
            _edges[i] = (i == NumVertexes) ? vertexes[indexes[0]] - vertexes[indexes[i]] : vertexes[indexes[i + 1]] -
                    vertexes[indexes[i]];

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

#endif //CRENDERER_PRIMITIVE_HPP
