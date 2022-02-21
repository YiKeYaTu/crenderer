//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_MESHTRIANGLE_HPP
#define CRENDERER_MESHTRIANGLE_HPP

#include <vector>
#include <fstream>
#include <string>
#include "../utils/OBJLoader.hpp"

class MeshTriangle {
public:
    static MeshTriangle loadFromObjFile(const std::string &);

    MeshTriangle(const std::vector<Vec3f> &vertexes, const std::vector<std::size_t> indexes) {
        for (int i = 0; i < vertexes.size() - 2; i += 3) {
            triangles.emplace_back(vertexes[indexes[i + 0]], vertexes[indexes[i + 1]], vertexes[indexes[i + 2]]);
        }
    }
private:
    std::vector<Triangle> triangles;
};

MeshTriangle MeshTriangle::loadFromObjFile(const std::string &fileName) {
    OBJLoader loader(fileName);
    return MeshTriangle(loader.vertexes(), loader.indexes());
}

#endif //CRENDERER_MESHTRIANGLE_HPP
