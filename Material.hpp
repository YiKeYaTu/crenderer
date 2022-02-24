//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_MATERIAL_HPP
#define CRENDERER_MATERIAL_HPP

class Material {
public:
    Material() = default;
    Material(double intensity): intensity_(intensity), isLightSource_(true) {}

    bool isLightSource() { return isLightSource_; }
    double intensity() { return intensity_; }

private:
    bool isLightSource_ = false;
    double intensity_;
};

#endif //CRENDERER_MATERIAL_HPP
