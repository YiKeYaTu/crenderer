//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_SCENE_HPP
#define CRENDERER_SCENE_HPP

#include "Vector.hpp"
#include "MeshTriangle.hpp"

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
        }

        return frameBuffer_;
    }
    Vec3f trace(const Ray& ray) {
        Vec3f renderedColor;
        for (const auto& object : objects_) {
            renderedColor += trace1object_(ray, object);
        }
        return renderedColor;
    }

private:
    Vec3f trace1object_(const Ray& ray, const MeshTriangle& object) {
        Intersection intersection = object.intersect(ray);

        if (!intersection.happened()) {
            return Vec3f();
        }

        Geometry* intersectedObject = intersection.intersectedObject();
        const Vec3f hitPoint = intersection.hitPoint();
        const double eps = 0.001;
        Vec3f renderedColor;

        for (const auto& object : objects_) {
            if (object.material().isLightSource()) {
                Vec3f lightPosition = object.centroid();
                Ray ray2light(hitPoint, (lightPosition - hitPoint).normalized());
                double ideaDistance2light = (hitPoint - lightPosition).norm();

                Intersection intersectWithLight = object.intersect(ray2light);

                if (intersectWithLight.happened() && std::abs(intersectWithLight.distance() - ideaDistance2light) <= eps) {
                    renderedColor += object.material().intensity();
                }
            }
        }

        return renderedColor;
    }
};

#endif //CRENDERER_SCENE_HPP
