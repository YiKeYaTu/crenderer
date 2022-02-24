//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_SCENE_HPP
#define CRENDERER_SCENE_HPP

#include "Vector.hpp"
#include "MeshTriangle.hpp"
#include <cmath>

class Scene {
private:
    std::vector<MeshTriangle> objects_;
    const unsigned int width_;
    const unsigned int height_;
    std::vector<Vec3f> frameBuffer_;
public:
    Scene(const unsigned int width, const unsigned int height) : width_(width), height_(height) {
        frameBuffer_.resize(width * height);
    }
    const unsigned int width() const { return width_; }
    const unsigned int height() const { return height_; }
    void addObject(MeshTriangle& object) { objects_.push_back(object); }

    std::vector<Vec3f> render() {
        Vec3f cameraPos(278, 273, -800);
        double aspectRatio = width_ / height_;

        int m = 0;

        for (uint32_t j = 0; j < height_; ++j) {
            for (uint32_t i = 0; i < width_; ++i) {
                float x = (2 * (i + 0.5) / (float)width_ - 1);
                float y = (1 - 2 * (j + 0.5) / (float)height_);

                Vec3f dir = Vec3f(-x, y, 1).normalized();

                for (int k = 0; k < 16; ++k) {
                    const Ray ray(cameraPos, dir);
                    frameBuffer_[m] += trace(ray);
                }
                m ++;
            }
            util::UpdateProgress(j / (float)height_);
        }
        util::UpdateProgress(1.f);

        return frameBuffer_;
    }
    Vec3f trace(const Ray& ray) {
        Vec3f renderedColor;

        MeshTriangle nearestObject;
        Intersection nearestIntersection;

        for (const auto& object : objects_) {
            Intersection itersec = object.intersect(ray);
            if (itersec.t() < nearestIntersection.t()) {
                nearestIntersection = itersec;
                nearestObject = object;
            }
        }

        renderedColor += trace1object_(ray, nearestObject);

        return renderedColor;
    }

private:
    Vec3f trace1object_(const Ray& ray, const MeshTriangle& object) {
        Intersection intersection = object.intersect(ray);

        if (!intersection.happened()) {
            return Vec3f();
        }

        if (object.material().isLightSource()) {
            return Vec3f(1, 1, 1);
        }

        const Vec3f hitPoint = intersection.hitPoint();
        const double eps = 0.001;
        Vec3f renderedColor;

        for (const auto& o : objects_) {
            if (&o == &object || !o.material().isLightSource()) {
                continue;
            }

            Vec3f lightPosition = o.centroid();
            Ray ray2light(hitPoint, (lightPosition - hitPoint).normalized());
            double ideaDistance2light = (hitPoint - lightPosition).norm();
            Intersection intersectWithLight = o.intersect(ray2light);

            if (intersectWithLight.happened() && std::abs(intersectWithLight.distance() - ideaDistance2light) <= eps) {
                renderedColor += phongFragmentShader(ray, object, intersection, o);
            }
        }

        return renderedColor;
    }

    Vec3f phongFragmentShader(const Ray& camera2hitPoint, const MeshTriangle& object, const Intersection& intersection, const MeshTriangle& light) {
        const Vec3f camera2hitPointVec(camera2hitPoint.direction);
        const Vec3f light2hitPointVec(intersection.hitPoint() - light.centroid());

        double p = 150;
        double r2 = (light.centroid() - intersection.hitPoint()).norm();

        Triangle* intersectedTriangle = dynamic_cast<Triangle*>(intersection.intersectedObject());
        Vec3f normal = -intersectedTriangle->normal();
        Vec3f intensity = light.material().e();

        Vec3f ks(object.material().ks());
        Vec3f kd(object.material().kd());

        Vec3f specular = ks * cos(light2hitPointVec, normal) * std::pow(cos(camera2hitPointVec + light2hitPointVec, normal), p) * intensity / r2;
        Vec3f diffuse = kd * cos(light2hitPointVec, normal) * intensity / r2;

        Vec3f color = specular + diffuse;
        return color;
    }
};

#endif //CRENDERER_SCENE_HPP
