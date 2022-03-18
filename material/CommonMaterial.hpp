//
// Created by lc on 2022/3/2.
//

#ifndef CRENDERER_COMMONMATERIAL_HPP
#define CRENDERER_COMMONMATERIAL_HPP

#include "../Vector.hpp"
#include "../utils/util.hpp"
#include "./property/Property.hpp"
#include "./property/DiffuseProperty.hpp"
#include "./property/SpecularProperty.hpp"
#include "./MaterialType.hpp"

class CommonMaterial {
public:
    CommonMaterial(const Vec3f &kd, const Vec3f &ks) : CommonMaterial(kd, ks, Vec3f(), DIFFUSE) {}
    CommonMaterial(const Vec3f &kd, const Vec3f &ks, const Vec3f &e) : CommonMaterial(kd, ks, e, DIFFUSE) {}
    CommonMaterial(const Vec3f &kd, const Vec3f &ks, const Vec3f &e, const PropertyType& propertyType) : emission_(e), kd_(kd), ks_(ks) {
        switch (propertyType) {
            case DIFFUSE:
                property_ = std::make_shared<DiffuseProperty>(DiffuseProperty());
                break;
            case SPECULAR:
                property_ = std::make_shared<SpecularProperty>(SpecularProperty());
                break;
            default:
                throw std::runtime_error("unacquainted property type: " + propertyType);
        }
        materialType_ = COMMON;
        propertyType_ = propertyType;
    }

    virtual bool hasEmission() const { return false; }
    virtual const Vec3f getEmission() const { return emission_; }
    virtual const Vec3f getEmission(const Vec3f& pointPos, const Vec3f& lightPos, const Vec3f& normal, const Object* light) const { return emission_; }
    virtual Vec3f fr(const Vec3f& wi, const Vec3f& wo, const Vec3f& normal) const {
        return property_->BRDF(kd_, ks_, emission_, wi, wo, normal);
    }
    virtual std::pair<Vec3f, double> sampleOutDirection(const Vec3f& wi, const Vec3f& normal) const {
        return property_->sample(wi, normal);
    }

    const Vec3f& kd() const { return kd_; }
    const Vec3f& ks() const { return ks_; }
    const Vec3f emission() const { return emission_; }
    const PropertyType propertyType() const { return propertyType_; }
    const MaterialType materialType() const { return materialType_; }

protected:
    Vec3f emission_{0, 0, 0};
    Vec3f kd_;
    Vec3f ks_;
    std::shared_ptr<Property> property_;
    PropertyType propertyType_;
    MaterialType materialType_;
};

#endif //CRENDERER_COMMONMATERIAL_HPP
