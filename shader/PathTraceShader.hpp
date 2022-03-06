////
//// Created by lc on 2022/3/1.
////
//
//#ifndef CRENDERER_PATHTRACESHADER_HPP
//#define CRENDERER_PATHTRACESHADER_HPP
//
//#include "Shader.hpp"
//#include "../Scene.hpp"
//#include "../object/MeshTriangle.hpp"
//
//class PathTraceShader: public Shader {
//private:
//    Scene* scene_;
//public:
//    PathTraceShader(Scene* scene): scene_(scene) {}
//    virtual Vec3f shade(const Ray& camera2hitPoint, int depth, const Intersection* iterObject, const Intersection* iterVolume) override {
//        if (!iterObject->happened()) {
//            return Vec3f();
//        }
//
//        if (iterObject->intersectedObject()->material()->hasEmission()) {
//            return iterObject->intersectedObject()->material()->getEmission();
//        }
//
//        Vec3f intersectedObjectNormal = iterObject->intersectedObject()->computeNormal(iterObject->hitPoint()).normalized();
//
//        Object** light = new Object*();
//        Vec3f sampleLightPosition;
//        double sampleRatio;
//        scene_->sampleLight(light, sampleLightPosition, sampleRatio);
//
//        Vec3f lightNormal = (*light)->computeNormal(sampleLightPosition);
//        Vec3f light2hitPoint = (iterObject->hitPoint() - sampleLightPosition).normalized();
//
//        Ray ray2light(iterObject->hitPoint(), (sampleLightPosition - iterObject->hitPoint()).normalized());
//
//        Vec3f directColor(0, 0, 0);
//        Vec3f inDirectColor(0, 0, 0);
//
//        auto intersectionWithLight = scene_->intersect(ray2light);
//
//        if ((intersectionWithLight.hitPoint() - sampleLightPosition).norm() < 0.1) {
//            double r = (sampleLightPosition - iterObject->hitPoint()).norm();
//            double r2 = std::pow(r, 2);
//
//            directColor = (*light)->material()->getEmission(light2hitPoint, lightNormal)
//                            / r2
//                            * iterObject->intersectedObject()->material()->fr(-light2hitPoint, -camera2hitPoint.direction, intersectedObjectNormal)
//                            * std::fmax(cos(lightNormal, light2hitPoint), 0)
//                            * std::fmax(cos(light2hitPoint, -intersectedObjectNormal), 0)
//                            / sampleRatio;
//        }
//
//        delete light;
//
//        double russiaRatio = 0.8;
//
//        if (util::getRandom01() < russiaRatio) {
//            std::pair<Vec3f, double> sampleIndirectDirectionPair = iterObject->intersectedObject()->material()->sampleOutDirection(
//                -camera2hitPoint.direction,
//                intersectedObjectNormal
//            );
//            Vec3f& sampleIndirectDirection = sampleIndirectDirectionPair.first;
//            double sampleIndirectDirectionRatio = sampleIndirectDirectionPair.second;
//            double cosNormalOutRay = cos(intersectedObjectNormal, sampleIndirectDirection);
//
//            auto nextIntersection = scene_->intersect(Ray(iterObject->hitPoint(), sampleIndirectDirection));
//
//            if (nextIntersection.happened() && nextIntersection.t() >= 0 && !nextIntersection.intersectedObject()->material()->hasEmission()) {
//                Vec3f tracedColor = scene_->trace(Ray(iterObject->hitPoint(), sampleIndirectDirection), depth + 1);
//                inDirectColor += iterObject->intersectedObject()->material()->fr(sampleIndirectDirection, -camera2hitPoint.direction, intersectedObjectNormal)
//                                 * cosNormalOutRay
//                                 * tracedColor
//                                 / sampleIndirectDirectionRatio
//                                 / russiaRatio;
//            }
//        }
//
//        return directColor + inDirectColor;
//    }
//
//};
//
//#endif //CRENDERER_PATHTRACESHADER_HPP
