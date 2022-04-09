//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_SCENE_HPP
#define CRENDERER_SCENE_HPP

#include <object/Object.hpp>
#include <unordered_map>
#include <string>
#include <material/Material.hpp>

class Scene {
private:
    unsigned short _width = 1000;
    unsigned short _height = 800;
    std::unordered_map<std::string, Material> _materials;

public:
    const unsigned short width() const { return _width; }
    const unsigned short height() const { return _height; }

    const std::unordered_map<std::string, Material>& materials() const { return _materials; }
    const Material& material(std::string& materialName) const { return _materials.find(materialName)->second; }

    void setSceneSize(unsigned short width, unsigned short height) {
        _width = width;
        _height = height;
    }

    void add(const std::string& materialName, const Material& material) {
        if (_materials.count(materialName)) {
            throw std::runtime_error(std::string("Find duplicated material: ") + materialName);
        }
        _materials.insert({ materialName, material });
    }
};

#endif //CRENDERER_SCENE_HPP
