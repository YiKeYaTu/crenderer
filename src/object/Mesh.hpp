//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_MESH_HPP
#define CRENDERER_MESH_HPP

#include <object/Object.hpp>
#include <core/Vec.hpp>
#include <core/Vertex.hpp>
#include <vector>

template <typename T>
class Mesh: public Object {
    static_assert(std::is_base_of<Object, T>::value);
public:
    typedef unsigned int sizeType;
    Mesh(
        const std::vector<Vec3f>& vertexes,
        const std::vector<sizeType>& indexes,
        const std::vector<Vec3f>& normals,
        const std::vector<Vec2f>& texCoords
    ) : _numVertexesPerPrimitive
    (T::numVertexes) {
        assert(vertexes.size() > 0 && indexes.size() > 0 && vertexes.size() == normals.size());
        _vertexes.reserve(vertexes.size());
        _indexes.reserve(indexes.size());

        for (int i = 0; i < vertexes.size(); i += 1) {
            _vertexes.template emplace_back(&vertexes[i], &normals[i], texCoords.size() > 0 ? &texCoords[i] : nullptr);
            _indexes.template emplace_back(indexes[i]);

//            if (i % _numVertexesPerPrimitive == 0) {
//                _primitives.emplace_back(
//                    &vertexes[0],
//                    &indexes[0] + i,
//                    &normals[0],
//                    texCoords.size() > 0 ? &texCoords[0] : nullptr
//                );
//            }
        }
    }

    const std::vector<Vertex>& vertexes() const { return _vertexes; }
    const std::vector<sizeType> &indexes() const { return _indexes; }

private:
    std::vector<Vertex> _vertexes;
    std::vector<sizeType> _indexes;
    std::vector<T> _primitives;
    sizeType _numVertexesPerPrimitive;
};

#endif //CRENDERER_MESH_HPP
