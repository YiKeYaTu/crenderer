//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_MATERIAL_HPP
#define CRENDERER_MATERIAL_HPP

class Material {
public:
    Material() = default;
    Material(const Vec3f &kd, const Vec3f &ks) : kd_(kd), ks_(ks) {}
    Material(const Vec3f &kd, const Vec3f &ks, const Vec3f &e) : e_(e), kd_(kd), ks_(ks), isLightSource_(true) {}

    bool isLightSource() { return isLightSource_; }
    const Vec3f& e() const { return e_; }
    const Vec3f& kd() const { return kd_; }
    const Vec3f& ks() const { return ks_; }

private:
    bool isLightSource_ = false;
    Vec3f e_;
    Vec3f kd_;
    Vec3f ks_;
};

#endif //CRENDERER_MATERIAL_HPP
