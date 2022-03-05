//
// Created by lc on 2022/3/1.
//

#ifndef CRENDERER_VOLUMELIGHTRENDERER_HPP
#define CRENDERER_VOLUMELIGHTRENDERER_HPP

#include "Shader.hpp"
#include "../utils/util.hpp"
#include "../Scene.hpp"
#include "../object/MeshTriangle.hpp"
#include "../sample.hpp"
#include "PathTraceShader.hpp"


// sigma 怎么设置？
// 给定一个分布的概率密度函数，怎么去 sample 一个符合概率密度函数的点？

// sample 一个距离（沿着光线方向）
// P(s) = k * e ^ (-k * s)
// P(d) = e ^ (-k * s)

class VolumeLightShader: public Shader {
private:
    Scene *scene_;
    std::shared_ptr<PathTraceShader> pathTraceShader_;
    double sigT = 0.02;
    double sigS = 0.01;

    bool enableSampleLight = true;
public:
    // assume ray must intersect with x
    // assume sigmaAbsorb and sigmaScattering are constant
    VolumeLightShader(Scene *scene) : scene_(scene) {
//        pathTraceShader_ = std::make_shared<PathTraceShader>(PathTraceShader(scene));
    }

    std::pair<double, double> sampleScattering(double sigma, double min = 0) {
        double randomCDF = util::getRandom01();
        double t = -log(1 - randomCDF) / sigma + min;
        return { t, sigma * std::exp(-sigma * t) };
    }

    // phase function
    // isotropic-scattering
    const double p(const Vec3f &wi, const Vec3f &wo) {
        return 1 / (4 * util::MY_PI);
    }

    // phase function
    // HenyeyGreenstein
    const double HenyeyGreenstein(const Vec3f &wi, const Vec3f &wo, double g = 0.45) {
        double g2 = std::pow(g, 2);
        double c = cos(wi, wo);
        return (1 - g2)
               / (4 * util::MY_PI * std::pow(1 + g2 - 2 * g * c, 1.5));
    }

    // phase function
    // Schlick
    const double Schlick(const Vec3f &wi, const Vec3f &wo, double k = 0.45) {
        double k2 = std::pow(k, 2);
        double c = cos(wi, wo);
        return (1 - k2) / (4 * util::MY_PI * std::pow(1 + k * c, 2));
    }

    const std::pair<Vec3f, double> sampleInScatteringDirection() {
        double a = 2 * util::MY_PI * util::getRandom01();
        double b = 2 * util::MY_PI * util::getRandom01();

        double x = 1 * std::cos(a);
        double y = 1 * std::sin(a);
        double z = sqrt(x * x + y * y) * tan(b);

        Vec3f direction(x, y, z);
        return {direction.normalized(), 1.0f / (4.0f * util::MY_PI) };
    }

    Vec3f shade(const Ray &ray, int depth, const Intersection* iterObject, const Intersection* iterVolume) {
        assert(iterObject != nullptr);
        depth ++;

        double russiaRatio = 1.0;

        if (depth > 5) {
            russiaRatio = 0.5;
        }

        Vec3f surfaceColor, particleColor;
        double transmittance = 1.0, tEnterVolume = std::numeric_limits<double>::max(), tOutVolume = tEnterVolume;

        if (iterVolume && iterVolume->happened()) {
            tOutVolume = fmin(iterObject->t(), iterVolume->tMax());
            tEnterVolume = iterVolume->t();
            transmittance = exp( -sigT * fmax(tOutVolume - tEnterVolume, 0) );
        }

        auto pair = sampleScattering(
                sigT,
                tEnterVolume
        );
        double sampledScatteringT = pair.first;
        double tPDF = pair.second;

        if (iterVolume && iterVolume->happened()) {
            Vec3f particlePosition = ray.origin + ray.direction * sampledScatteringT;
            if (!iterObject->happened() || sampledScatteringT < iterObject->t()) {
                if (enableSampleLight) {
                    Object **particleLight = new Object* ();
                    Vec3f sampleParticleLightPosition;
                    double sampleParticleRatio;
                    scene_->sampleLight(particleLight, sampleParticleLightPosition, sampleParticleRatio);

                    Vec3f particleLightNormal = (*particleLight)->computeNormal(sampleParticleLightPosition);
                    Vec3f light2particle = (particlePosition - sampleParticleLightPosition).normalized();
                    Ray ray2light(sampleParticleLightPosition, (sampleParticleLightPosition - particlePosition).normalized());

                    auto intersectionWithLight = scene_->intersect(ray2light);

                    if ((intersectionWithLight.hitPoint() - sampleParticleLightPosition).norm() < 0.1) {
                        double r = (particlePosition - sampleParticleLightPosition).norm();
                        double r2 = std::pow(r, 2);
                        double particleLightTransmittance = 1.0;

                        auto intersectionWithLightVolume = scene_->intersectVolume(ray2light);
                        double tEnterParticleLightVolume = std::numeric_limits<double>::max(), tOutParticleLightVolume = tEnterParticleLightVolume;

                        if (intersectionWithLightVolume.happened()) {
                            tOutParticleLightVolume = fmin(intersectionWithLight.t(), intersectionWithLightVolume.tMax());
                            tEnterParticleLightVolume = intersectionWithLightVolume.t();
                            particleLightTransmittance = exp( -sigT * fmax(tOutParticleLightVolume - tEnterParticleLightVolume, 0) );
                        }

                        particleColor += (*particleLight)->material()->getEmission(light2particle, particleLightNormal)
                            * (1 / r2)
                            * std::fmax(cos(particleLightNormal, light2particle), 0)
                            * particleLightTransmittance
                            * (1.0 / 1.0)
                            * Schlick(ray2light.direction, -ray.direction, 0.5)
                            * (sigS / sigT)
                            * (1 / sampleParticleRatio);
                    }
                }
//                if (false) {
                if (util::getRandom01() < russiaRatio) {
                    std::pair<Vec3f, double> sampleIndirectDirectionPair = sampleInScatteringDirection();
                    Ray particle2random(particlePosition, sampleIndirectDirectionPair.first);

                    Ray nextRay(particlePosition, sampleIndirectDirectionPair.first);
                    auto nextIter = scene_->intersect(nextRay);

                    if (!nextIter.happened() || !nextIter.intersectedObject()->material()->hasEmission()) {
                        Vec3f tracedColor = scene_->trace(particle2random, depth);
                        Vec3f indirectParticleColor = (tracedColor
                                * Schlick(sampleIndirectDirectionPair.first, -ray.direction, 0.5)
                                * (sigS / sigT)
                                * (1 / sampleIndirectDirectionPair.second)) * (1 / russiaRatio);

                        particleColor += indirectParticleColor;
                    }
                }

                particleColor = particleColor
                        * sigT
                        * std::exp( -sigT * (sampledScatteringT - tEnterVolume) )
                        * (1 / tPDF);
            }
        }

        if (iterObject->happened() && sampledScatteringT >= iterObject->t()) {
            if (iterObject->intersectedObject()->material()->hasEmission()) {
                surfaceColor += iterObject->intersectedObject()->material()->getEmission()
                    * transmittance
                    / exp(-sigT* fmax( iterObject->t() - tEnterVolume, 0 ) );
            } else {
                if (enableSampleLight) {
                    Object **light = new Object *();
                    Vec3f sampleLightPosition;
                    double sampleRatio;
                    scene_->sampleLight(light, sampleLightPosition, sampleRatio);

                    Vec3f lightNormal = (*light)->computeNormal(sampleLightPosition);
                    Vec3f light2hitPoint = (iterObject->hitPoint() - sampleLightPosition).normalized();
                    Ray ray2light(iterObject->hitPoint(), (sampleLightPosition - iterObject->hitPoint()).normalized());

                    auto intersectionWithLight = scene_->intersect(ray2light);

                    if ((intersectionWithLight.hitPoint() - sampleLightPosition).norm() < 0.1) {
                        Vec3f intersectedObjectNormal = iterObject->intersectedObject()->computeNormal(iterObject->hitPoint()).normalized();
                        double r = (sampleLightPosition - iterObject->hitPoint()).norm();
                        double r2 = std::pow(r, 2);
                        double lightTransmittance = 1.0;

                        auto intersectionWithLightVolume = scene_->intersectVolume(ray2light);
                        double tEnterLightVolume = std::numeric_limits<double>::max(), tOutLightVolume = tEnterLightVolume;

                        if (intersectionWithLightVolume.happened()) {
                            tOutLightVolume = fmin(intersectionWithLight.t(), intersectionWithLightVolume.tMax());
                            tEnterLightVolume = intersectionWithLightVolume.t();
                            lightTransmittance = exp( -sigT * fmax(tOutLightVolume - tEnterLightVolume, 0) );
                        }

                        surfaceColor += (*light)->material()->getEmission(light2hitPoint, lightNormal)
                                        / r2
                                        * iterObject->intersectedObject()->material()->fr(-light2hitPoint, -ray.direction, intersectedObjectNormal)
                                        * std::fmax(cos(lightNormal, light2hitPoint), 0)
                                        * std::fmax(cos(light2hitPoint, -intersectedObjectNormal), 0)
                                        * lightTransmittance
                                        / sampleRatio
                                        * transmittance
                                        / exp( fmax( -sigT*iterObject->t() - tEnterVolume, 0 ) );
//                                        * exp( fmax( -sigT*iterObject->t() - tEnterVolume, 0 ) );
                    }
                }
                if (util::getRandom01() < russiaRatio) {
                    Vec3f intersectedObjectNormal = iterObject->intersectedObject()
                            ->computeNormal(iterObject->hitPoint()).normalized();

                    std::pair<Vec3f, double> sampleIndirectDirectionPair = iterObject->intersectedObject()->material()->sampleOutDirection(
                            -ray.direction,
                            intersectedObjectNormal
                    );
                    Vec3f& sampleIndirectDirection = sampleIndirectDirectionPair.first;
                    double sampleIndirectDirectionRatio = sampleIndirectDirectionPair.second;
                    double cosNormalOutRay = cos(intersectedObjectNormal, sampleIndirectDirection);

                    Ray nextRay(iterObject->hitPoint(), sampleIndirectDirection);
                    auto nextIter = scene_->intersect(nextRay);

                    if (!nextIter.happened() || !nextIter.intersectedObject()->material()->hasEmission()) {
                        Vec3f tracedColor = scene_->trace(Ray(iterObject->hitPoint(), sampleIndirectDirection), depth + 1);
                        surfaceColor += iterObject->intersectedObject()->material()->fr(sampleIndirectDirection, -ray.direction, intersectedObjectNormal)
                                        * cosNormalOutRay
                                        * tracedColor
                                        / sampleIndirectDirectionRatio
                                        * transmittance
                                        / exp( fmax( -sigT*iterObject->t() - tEnterVolume, 0 ) )
                                        / russiaRatio;
                    }
                }
            }
        }

        return surfaceColor + particleColor;
    }
};

#endif //CRENDERER_VOLUMELIGHTRENDERER_HPP
