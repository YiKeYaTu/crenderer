//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_SCENE_HPP
#define CRENDERER_SCENE_HPP

#include <object/Object.hpp>
#include <scene/Loader.hpp>

class Scene {
private:
    unsigned short _width = 600;
    unsigned short _height = 600;
    std::vector<Mesh<Triangle>> _meshes;

public:
    const unsigned short width() const { return _width; }
    const unsigned short height() const { return _height; }

    Scene() {}

    const std::vector<Mesh<Triangle>>& meshes() const { return _meshes; }

    void setSceneSize(unsigned short width, unsigned short height) {
        _width = width;
        _height = height;
    }

    void add(const std::vector<Mesh<Triangle>>& meshes) {
        _meshes.insert(_meshes.begin(), meshes.begin(), meshes.end());
    }

};

#endif //CRENDERER_SCENE_HPP
