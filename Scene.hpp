//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_SCENE_HPP
#define CRENDERER_SCENE_HPP

#include "Vector.hpp"
#include "object/Object.hpp"
#include <cmath>

class Scene {
private:
    std::vector<std::shared_ptr<Object>> objects_;
    const unsigned int width_;
    const unsigned int height_;
    const double fov_;
    std::vector<Vec3f> frameBuffer_;
    Vec3f cameraPos_;
    Vec3f bgColor_;
    std::shared_ptr<BVH> bvh_;
public:
    Scene(const unsigned int width, const unsigned int height, const Vec3f& cameraPos, const Vec3f& bgColor, const double fov = 45)
    : width_(width), height_(height), cameraPos_(cameraPos), bgColor_(bgColor), fov_(fov) {
        frameBuffer_.resize(width * height);
    }
    const unsigned int width() const { return width_; }
    const unsigned int height() const { return height_; }
    void addObject(MeshTriangle& object) { objects_.push_back(std::make_shared<MeshTriangle>(object)); }

    std::vector<Vec3f> render(std::size_t spp = 32) {
        assert(spp > 0);

        int m = 0;
        double aspectRatio = static_cast<double>(width_) / height_;
        double scale = std::tan((fov_ / 2) / 180 * util::MY_PI);

        if (bvh_ == nullptr) {
            bvh_ = BVH::buildBVH(objects_);
        }

        for (uint32_t j = 0; j < height_; ++j) {
            for (uint32_t i = 0; i < width_; ++i) {
                float x = (2 * (i + 0.5) / (float)width_ - 1) * aspectRatio * scale;
                float y = (1 - 2 * (j + 0.5) / (float)height_) * scale;

                Vec3f dir = Vec3f(-x, y, 1).normalized();

                for (int k = 0; k < spp; ++k) {
                    const Ray ray(cameraPos_, dir);
                    frameBuffer_[m] += trace(ray);
                }

                frameBuffer_[m] /= spp;
                m ++;
            }
            util::UpdateProgress(j / (float)height_);
        }
        util::UpdateProgress(1.f);

        return frameBuffer_;
    }

    Intersection intersect(const Ray& ray) {
        return BVH::intersect(ray, bvh_);
    }

    Vec3f trace(const Ray& ray, int depth = 0) {
        Intersection intersection = intersect(ray);

        if (!intersection.happened()) {
            return bgColor_;
        }

        return pathTraceFragmentShader(ray, intersection, depth);
    }

private:
    void sampleLight(Object** light, Vec3f& samplePosition, double& sampleRatio) {
        static double area = 0;

        if (area == 0) {
            for (const auto& object : objects_) {
                if (object->material()->isLightSource()) {
                    const auto& bounds3 = object->bounds3();
                    double length = bounds3.max().x() - bounds3.min().x();
                    double width = bounds3.max().z() - bounds3.min().z();

                    area += width * length;
                }
            }
        }
        
        double target = util::getRandom01() * area;
        double currenArea = 0;

        for (const auto& object : objects_) {
            if (object->material()->isLightSource()) {
                const auto& bounds3 = object->bounds3();
                double length = bounds3.max().x() - bounds3.min().x();
                double width = bounds3.max().z() - bounds3.min().z();

                currenArea += width * length;

                if (currenArea >= target) {
                    double a = util::getRandom01();
                    double b = util::getRandom01();
                    double c = util::getRandom01();

                    *light = object.get();
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
 

    Vec3f pathTraceFragmentShader(const Ray& camera2hitPoint, const Intersection& intersection, int depth) {
        Vec3f intersectedObjectNormal = intersection.intersectedObject()->normal().normalized();

        Object** light = new Object*();

        Vec3f sampleLightPosition;
        double sampleRatio;

        if (intersection.intersectedObject()->material()->isLightSource()) {
            if (depth == 0) {
                return intersection.intersectedObject()->material()->e();
            }
            return Vec3f();
        }

        sampleLight(light, sampleLightPosition, sampleRatio);

        Ray ray2light(intersection.hitPoint(), (sampleLightPosition - intersection.hitPoint()).normalized());

        Vec3f directColor(0, 0, 0);
        Vec3f inDirectColor(0, 0, 0);

        if ((intersect(ray2light).hitPoint() - sampleLightPosition).norm() < 0.1) {
            double r = (sampleLightPosition - intersection.hitPoint()).norm();
            double r2 = std::pow(r, 2);
            Vec3f light2hitPoint = (intersection.hitPoint() - sampleLightPosition).normalized();

            directColor = (*light)->material()->e() / r2 *
                intersection.intersectedObject()->material()->fr(-light2hitPoint, -camera2hitPoint.direction, intersectedObjectNormal) *
                std::fmax(cos(dynamic_cast<MeshTriangle&>(**light).triangles()[0]->normal(), light2hitPoint), 0) *
                std::fmax(cos(light2hitPoint, -intersectedObjectNormal), 0) /
                sampleRatio;
        }

        delete light;

        //if (false) {
        if (get_random_float() < 0.8) {
            std::pair<Vec3f, double> sampleIndirectDirectionPair = intersection.intersectedObject()->sampleDirection();
            Vec3f& sampleIndirectDirection = sampleIndirectDirectionPair.first;
            double sampleIndirectDirectionRatio = sampleIndirectDirectionPair.second;
            double cosNormalOutRay = cos(intersectedObjectNormal, sampleIndirectDirection);

            Vec3f tracedColor = trace(Ray(intersection.hitPoint(), sampleIndirectDirection), depth + 1) / sampleIndirectDirectionRatio / 0.8;

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
