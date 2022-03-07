//
// Created by lc on 2022/2/23.
//

#ifndef CRENDERER_SCENE_HPP
#define CRENDERER_SCENE_HPP

#include <cmath>
#include "Vector.hpp"
#include "object/Object.hpp"
#include "shader/Shader.hpp"
#include "utils/util.hpp"

class Scene {
private:
    bool enableMultipleThread = true;
    std::vector<Object*> objects_;
    std::vector<Object*> lights_;
    std::vector<Object*> volumes_;

    const unsigned int width_;
    const unsigned int height_;
    const double fov_;

    std::vector<Vec3f> frameBuffer_;
    Vec3f cameraPos_;
    Vec3f bgColor_;

    std::shared_ptr<BVH> bvh_;
    std::shared_ptr<BVH> volumeBvh_;

    Shader* shader_;
    std::mutex m_;

public:
    Scene(const unsigned int width, const unsigned int height, const Vec3f& cameraPos, const Vec3f& bgColor, const double fov = 45)
    : width_(width), height_(height), cameraPos_(cameraPos), bgColor_(bgColor), fov_(fov) {
        frameBuffer_.resize(width * height);
    }
    const unsigned int width() const { return width_; }
    const unsigned int height() const { return height_; }
    void addObject(Object* object) { objects_.push_back(object); }
    void addVolume(Object* object) { volumes_.push_back(object); }
    void setShader(Shader* shader) { shader_ = shader; }

    std::vector<Vec3f> render(std::size_t spp = 32, std::size_t numThread = 1) {
        assert(spp > 0);

        int m = 0;
        double aspectRatio = static_cast<double>(width_) / height_;
        double scale = std::tan((fov_ / 2) / 180 * util::MY_PI);

        if (bvh_ == nullptr) {
            bvh_ = BVH::buildBVH(objects_);
            volumeBvh_ = BVH::buildBVH(volumes_);
        }

        util::ThreadPool threadPool_ = util::ThreadPool(16, 16);
        std::mutex mutexFrameBuffer;

        for (uint32_t j = 0; j < height_; ++j) {
            for (uint32_t i = 0; i < width_; ++i) {
                float x = (2 * (i + 0.5) / (float)width_ - 1) * aspectRatio * scale;
                float y = (1 - 2 * (j + 0.5) / (float)height_) * scale;

                Vec3f dir = Vec3f(-x, y, 1).normalized();

                for (int k = 0; k < spp; ++k) {
                    if (!enableMultipleThread) {
                        const Ray ray(cameraPos_, dir);
                        const Vec3f color = trace(ray) / spp;
                        frameBuffer_[m] += color;
                        if (i == width_ - 1) {
                            util::UpdateProgress((j * width_ + i) / (float)height_ / width_);
                        }
                    } else {
                        threadPool_.tryAppendTask([&, m, dir, i, j]{
                            const Ray ray(cameraPos_, dir);
                            const Vec3f color = trace(ray) / spp;
                            std::unique_lock<std::mutex> lockGuard(mutexFrameBuffer);
                            frameBuffer_[m] += color;
                            if (i == width_ - 1) {
                                util::UpdateProgress((j * width_ + i) / (float)height_ / width_);
                            }
                            lockGuard.unlock();
                        });
                    }
                }
                m ++;
            }
        }

        threadPool_.stop();
        threadPool_.allJoin();

        return frameBuffer_;
    }

    Intersection intersect(const Ray& ray) {
        return BVH::intersect(ray, bvh_);
    }

    Intersection intersectVolume(const Ray& ray) {
        return BVH::intersect(ray, volumeBvh_);
    }

    Vec3f trace(const Ray& ray, int depth = 0) {
        return shader_->shade(ray, depth);
    }

    void sampleLight(Object** light, Vec3f& samplePosition, double& sampleRatio) {
        std::lock_guard lockGuard(m_);
        static double area = 0;

        if (area == 0) {
            for (const auto object : objects_) {
                if (object->material()->hasEmission()) {
                    lights_.push_back(object);
                    const auto& bounds3 = object->bounds3();
                    double length = bounds3.max().x() - bounds3.min().x();
                    double width = bounds3.max().z() - bounds3.min().z();

                    area += width * length;
                }
            }
        }

        double target = util::getRandom01() * area;
        double currenArea = 0;

        for (const auto candiLight : lights_) {
            const auto& bounds3 = candiLight->bounds3();
            double length = bounds3.max().x() - bounds3.min().x();
            double width = bounds3.max().z() - bounds3.min().z();

            currenArea += width * length;

            if (currenArea >= target) {
                double a = util::getRandom01();
                double b = util::getRandom01();
                double c = util::getRandom01();

                *light = candiLight;
                samplePosition = Vec3f(
                        bounds3.min().x() + (bounds3.max().x() - bounds3.min().x()) * a,
                        bounds3.min().y() + (bounds3.max().y() - bounds3.min().y()) * b,
                        bounds3.min().z() + (bounds3.max().z() - bounds3.min().z()) * c);
                sampleRatio = 1.0f / area;
                break;
            }
        }
    }
};

#endif //CRENDERER_SCENE_HPP
