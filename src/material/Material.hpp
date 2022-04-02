//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_MATERIAL_HPP
#define CRENDERER_MATERIAL_HPP

#include <core/Vec.hpp>

class Material {
public:
    float kd() const { return _kd; }
    float ks() const { return _ks; }
    float ka() const { return _ka; }
private:
    float _kd, _ks, _ka;
};

#endif //CRENDERER_MATERIAL_HPP
