//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_SCENE_HPP
#define CRENDERER_SCENE_HPP

#include "Vector.hpp"
#include "object/MeshTriangle.hpp"
#include <cmath>

class Scene {
private:
    std::vector<MeshTriangle> objects_;
    const unsigned int width_;
    const unsigned int height_;
    std::vector<Vec3f> frameBuffer_;
    Vec3f cameraPos_;
    Vec3f bgColor_;
    std::shared_ptr<BVH> bvh_;
public:
    Scene(const unsigned int width, const unsigned int height, const Vec3f& cameraPos, const Vec3f& bgColor)
    : width_(width), height_(height), cameraPos_(cameraPos), bgColor_(bgColor) {
        frameBuffer_.resize(width * height);
    }
    const unsigned int width() const { return width_; }
    const unsigned int height() const { return height_; }
    void addObject(MeshTriangle& object) { objects_.push_back(object); }

    std::vector<Vec3f> render() {
        int m = 0;

        for (uint32_t j = 0; j < height_; ++j) {
            for (uint32_t i = 0; i < width_; ++i) {
                float x = (2 * (i + 0.5) / (float)width_ - 1);
                float y = (1 - 2 * (j + 0.5) / (float)height_);

                Vec3f dir = Vec3f(-x, y, 1).normalized();
                int maxK = 16;

                for (int k = 0; k < maxK; ++k) {
                    const Ray ray(cameraPos_, dir);
                    frameBuffer_[m] += trace(ray);
                }

                frameBuffer_[m] /= maxK;
                m ++;
            }
            util::UpdateProgress(j / (float)height_);
        }
        util::UpdateProgress(1.f);

        return frameBuffer_;
    }

    std::pair<Intersection, MeshTriangle> intersect(const Ray& ray) {
        MeshTriangle nearestObject;
        Intersection nearestIntersection;

        for (const auto& object : objects_) {
            Intersection itersec = object.intersect(ray);
            if (itersec.t() < nearestIntersection.t()) {
                nearestIntersection = itersec;
                nearestObject = object;
            }
        }
        return { nearestIntersection, nearestObject };
    }

    Vec3f trace(const Ray& ray) {
        Vec3f renderedColor;

        const auto& p = intersect(ray);

        MeshTriangle nearestObject = p.second;
        Intersection nearestIntersection = p.first;


        renderedColor += trace1object_(ray, nearestObject);

        return renderedColor;
    }

private:
    void sampleLight(Vec3f& radiance, Vec3f& samplePosition, double& sampleRatio) {
        static double area = 0;

        if (area == 0) {
            for (const auto& object : objects_) {
                if (object.material()->isLightSource()) {
                    const auto& bounds3 = object.bounds3();
                    double length = bounds3.max().x() - bounds3.min().x();
                    double width = bounds3.max().z() - bounds3.min().z();

                    area += width * length;
                }
            }
        }
        
        double target = util::getRandom01() * area;
        double currenArea = 0;

        for (const auto& object : objects_) {
            if (object.material()->isLightSource()) {
                const auto& bounds3 = object.bounds3();
                double length = bounds3.max().x() - bounds3.min().x();
                double width = bounds3.max().z() - bounds3.min().z();

                currenArea += width * length;

                if (currenArea >= target) {
                    double a = util::getRandom01();
                    double b = util::getRandom01();
                    double c = util::getRandom01();

                    radiance = object.material()->e();
                    samplePosition = Vec3f(
                        bounds3.min().x() + (bounds3.max().x() - bounds3.min().x()) * a,
                        bounds3.min().y() + (bounds3.max().y() - bounds3.min().y()) * b,
                        bounds3.min().z() + (bounds3.max().z() - bounds3.min().z()) * c);
                    sampleRatio = 1.0f / area;
                    break;
                }
            }
        }
    }
    Vec3f trace1object_(const Ray& ray, const MeshTriangle& object) {
        Intersection intersection = object.intersect(ray);

        if (!intersection.happened()) {
            return bgColor_;
        }

        //if (object.material()->isLightSource()) {
        //    return Vec3f(1, 1, 1);
        //}

        const Vec3f hitPoint = intersection.hitPoint();
        Vec3f renderedColor;

        for (const auto& o : objects_) {
            if (&o == &object || !o.material()->isLightSource()) {
                continue;
            }

            Vec3f lightPosition = o.centroid();
            Ray ray2light(hitPoint, (lightPosition - hitPoint).normalized());


            const auto& p = intersect(ray2light);

            MeshTriangle nearestObject = p.second;
            Intersection nearestIntersection = p.first;

            // Intersection intersectWithLight = o.intersect(ray2light);

            if ((nearestIntersection.hitPoint() - lightPosition).norm() < 1) {
                // renderedColor += phongFragmentShader(ray, intersection, o);
                
            }
            renderedColor += pathTraceFragmentShader(ray, intersection, o);
        }

        return renderedColor;
    }

    Vec3f pathTraceFragmentShader(const Ray& camera2hitPoint, const Intersection& intersection, const MeshTriangle& light) {
        Vec3f ks(intersection.intersectedObject()->material()->ks());
        Vec3f kd(intersection.intersectedObject()->material()->kd());

        //Vec3f directColor;
        //Vec3f inDirectColor;
        //// 方向量向外
        Vec3f intersectedObjectNormal = intersection.intersectedObject()->normal().normalized();

        Vec3f radiance, sampleLightPosition;
        double sampleRatio;

        sampleLight(radiance, sampleLightPosition, sampleRatio);

        Ray ray2light(intersection.hitPoint(), (sampleLightPosition - intersection.hitPoint()).normalized());
        Intersection intersectWithLight = light.intersect(ray2light);

        const auto& p = intersect(ray2light);

        MeshTriangle nearestObject = p.second;
        Intersection nearestIntersection = p.first;

        // Intersection intersectWithLight = o.intersect(ray2light);
        Vec3f directColor(0, 0, 0);
        Vec3f inDirectColor(0, 0, 0);

        if ((nearestIntersection.hitPoint() - sampleLightPosition).norm() < 0.1) {
            double r = (sampleLightPosition - intersection.hitPoint()).norm();
            double r2 = std::pow(r, 2);
            Vec3f light2hitPoint = (intersection.hitPoint() - sampleLightPosition).normalized();

            directColor = radiance / r2 *
                intersection.intersectedObject()->material()->fr(-light2hitPoint, -camera2hitPoint.direction, intersectedObjectNormal) *
                std::fmax(cos(light.triangles()[0]->normal(), light2hitPoint), 0) *
                std::fmax(cos(light2hitPoint, -intersectedObjectNormal), 0) /
                sampleRatio;
        }


        // todo: 在光源上 sample 一个点
        // todo: 在当前物体的包围半球上 sample 一个点
        //if (false) {
        if (get_random_float() < 0.8) {
            std::pair<Vec3f, double> sampleIndirectDirectionPair = intersection.intersectedObject()->sampleDirection();
            Vec3f& sampleIndirectDirection = sampleIndirectDirectionPair.first;
            double sampleIndirectDirectionRatio = sampleIndirectDirectionPair.second;
            double cosNormalOutRay = cos(intersectedObjectNormal, sampleIndirectDirection);

            Vec3f tracedColor = trace(Ray(intersection.hitPoint(), sampleIndirectDirection)) / sampleIndirectDirectionRatio / 0.8;

            inDirectColor += intersection.intersectedObject()->material()->fr(sampleIndirectDirection, -camera2hitPoint.direction, intersectedObjectNormal) *
                cosNormalOutRay *
                tracedColor;
        }

        return directColor + inDirectColor;
    }

    Vec3f phongFragmentShader(const Ray& camera2hitPoint, const Intersection& intersection, const MeshTriangle& light) {
        const Vec3f camera2hitPointVec(camera2hitPoint.direction);
        const Vec3f light2hitPointVec(intersection.hitPoint() - light.centroid());

        double p = 150;
        double r2 = (light.centroid() - intersection.hitPoint()).norm();

        Vec3f normal = -intersection.intersectedObject()->normal();
        Vec3f intensity = light.material()->e();

        Vec3f ks(intersection.intersectedObject()->material()->ks());
        Vec3f kd(intersection.intersectedObject()->material()->kd());

        Vec3f specular = ks * cos(light2hitPointVec, normal) * std::pow(cos(camera2hitPointVec + light2hitPointVec, normal), p) * intensity / r2;
        Vec3f diffuse = kd * cos(light2hitPointVec, normal) * intensity / r2;

        Vec3f color = specular + diffuse;
        return color;
    }
};

#endif //CRENDERER_SCENE_HPP