//
// Created by lc on 2022/3/1.
//

#ifndef CRENDERER_VOLUMELIGHTRENDERER_HPP
#define CRENDERER_VOLUMELIGHTRENDERER_HPP

#include "Shader.hpp"
#include "../utils/util.hpp"
#include "../Scene.hpp"
#include "../object/MeshTriangle.hpp"
#include "PathTraceShader.hpp"

class VolumeLightShader: public Shader {
private:
    Scene *scene_;

    double sigT = 0.02;
    double sigS = 0.01;

    bool enableSampleLight = true;

    struct SampledLight {
        Object **light;
        Intersection intersectionWithLight;
        double sampleRatio;

        Vec3f sampledLightPosition;
        Vec3f sampledLightNormal;
        Vec3f sampledEmission;

        Ray ray2light;

        bool canReachLight;
    };

    struct ComputedTransmittance {
        Intersection iterObject;
        Intersection iterVolume;

        double tEnterVolume;
        double tLeaveVolume;
        double tInVolume;
        double transmittance;
    };

    ComputedTransmittance computeTransmittance_(const Ray& ray2somewhere) {
        double transmittance = 1.0, tEnterVolume, tLeaveVolume, tInVolume;

        Intersection iterObject = scene_->intersect(ray2somewhere);
        Intersection iterVolume = scene_->intersectVolume(ray2somewhere);

        if (iterVolume.happened()) {
            tEnterVolume = fmin( iterVolume.t(), iterObject.t() );
            tLeaveVolume = fmin( iterObject.t(), iterVolume.tMax() );
            tInVolume = tLeaveVolume - fmax( tEnterVolume, 0 );
            transmittance = exp( -sigT * tInVolume );
        } else {
            tEnterVolume = tLeaveVolume = tInVolume = 0;
        }

        return ComputedTransmittance{
            iterObject, iterVolume,
            tEnterVolume, tLeaveVolume, tInVolume, transmittance
        };
    }

    SampledLight sampleLight_(const Vec3f& point) {
        Object **light = new Object *();
        Vec3f sampledLightPosition;
        double sampleRatio;
        scene_->sampleLight(light, sampledLightPosition, sampleRatio);

        double r2 = pow( ( point - sampledLightPosition ).norm(), 2 );
        Vec3f light2point = (point - sampledLightPosition).normalized();

        Vec3f sampledLightNormal = (*light)->computeNormal(sampledLightPosition);
        // projection light area to the half sphere
        Vec3f sampledEmission = (*light)->material()->getEmission(point, sampledLightPosition, sampledLightNormal, *light)
                * (1.0 / r2)
                * std::fmax(cos(sampledLightNormal, light2point), 0);

        Ray ray2light(point, (sampledLightPosition - point).normalized());
        auto intersectionWithLight = scene_->intersect(ray2light);
        bool canReachLight = (intersectionWithLight.hitPoint() - sampledLightPosition).norm() < 0.1;

        return SampledLight{
            light, intersectionWithLight, sampleRatio,
            sampledLightPosition, sampledLightNormal, sampledEmission,
            ray2light,
            canReachLight
        };
    }
public:
    // assume ray must intersect with x
    // assume sigmaAbsorb and sigmaScattering are constant
    VolumeLightShader(Scene *scene) : scene_(scene) { }

    std::pair<double, double> sampleScattering(double sigma, double min = 0) {
        double randomCDF = util::getRandom01();
        double t = -log(1 - randomCDF) / sigma + min;
        return { t, sigma * std::exp(-sigma * t) };
    }

    // phase function
    // isotropic-scattering
    const double p(const Vec3f &wi, const Vec3f &wo) {
        return 1.0f / (4 * util::MY_PI);
    }

    // phase function
    // HenyeyGreenstein
    const double HenyeyGreenstein(const Vec3f &wi, const Vec3f &wo, double g = 0.45) {
        double g2 = std::pow(g, 2);
        double c = cos(wi, wo);
        return (1.f - g2)
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

    Vec3f shade(const Ray &ray, int depth) {
        depth ++;

        double russiaRatio = 1.0;
        if (depth > 5) { russiaRatio = 0.5; }

        Vec3f surfaceColor, particleColor;

        ComputedTransmittance computedTransmittance = computeTransmittance_(ray);

        auto samplePair = sampleScattering(sigT, fmax( computedTransmittance.tEnterVolume, 0 ));
        double sampledScatteringT = samplePair.first;
        double tPDF = samplePair.second;

        if (false) {
//        if (computedTransmittance.iterVolume.happened()) {
            Vec3f particlePosition = ray.origin + ray.direction * sampledScatteringT;
            // if ray doesn't intersect any object,
            // or the sampled time lesser than intersected time
            if (!computedTransmittance.iterObject.happened() || sampledScatteringT < computedTransmittance.iterObject.t()) {
                particleColor += shadeParticle_(ray, depth, russiaRatio, samplePair, computedTransmittance);
            }
        }

        if (computedTransmittance.iterObject.happened() && sampledScatteringT >= computedTransmittance.iterObject.t()) {
            surfaceColor += shadeSurface_(ray, depth, russiaRatio, computedTransmittance);
        }

        return surfaceColor + particleColor;
    }

private:
    Vec3f shadeParticle_(const Ray& ray, int depth, double russiaRatio, std::pair<double, double>& pair, ComputedTransmittance& computedTransmittance) {
        double sampledScatteringT = pair.first;
        double tPDF = pair.second;

        Vec3f particlePosition = ray.origin + ray.direction * sampledScatteringT;
        Vec3f particleColor;

        if (enableSampleLight) {
            SampledLight sampledLightObj = sampleLight_(particlePosition);
            if (sampledLightObj.canReachLight) {
                ComputedTransmittance computedTransmittance = computeTransmittance_(sampledLightObj.ray2light);
                particleColor += sampledLightObj.sampledEmission
                                 * computedTransmittance.transmittance
                                 * (1.0 / 1.0)
                                 * Schlick(sampledLightObj.ray2light.direction, -ray.direction, 0.5)
                                 * (sigS / sigT)
                                 * (1.0 / sampledLightObj.sampleRatio);
            }
        }
//        if (false) {
        if (util::getRandom01() < russiaRatio) {
            std::pair<Vec3f, double> sampleIndirectDirectionPair = sampleInScatteringDirection();
            Ray particle2random(particlePosition, sampleIndirectDirectionPair.first);

            Ray nextRay(particlePosition, sampleIndirectDirectionPair.first);
            auto nextIter = scene_->intersect(nextRay);

            if (!nextIter.happened() || !nextIter.intersectedObject()->material()->hasEmission()) {
                Vec3f tracedColor = scene_->trace(particle2random, depth);
                Vec3f indirectParticleColor = tracedColor
                        * Schlick(sampleIndirectDirectionPair.first, -ray.direction, 0.5)
                        * (sigS / sigT)
                        * (1.0f / sampleIndirectDirectionPair.second)
                        * (1.0f / russiaRatio);

                particleColor += indirectParticleColor;
            }
        }

        particleColor = particleColor
                        * sigT
                        * std::exp( -sigT * ( computedTransmittance.tEnterVolume >= 0 ? (sampledScatteringT - computedTransmittance.tEnterVolume) : sampledScatteringT ) )
                        * (1.0f / tPDF);
        return particleColor;
    }

    Vec3f shadeSurface_(const Ray& ray, int depth, double russiaRatio, ComputedTransmittance& computed2eyeTransmittance) {
        auto& iterObject = computed2eyeTransmittance.iterObject;
        double fromVolumeEntryToSurfaceT = computed2eyeTransmittance.iterObject.t() - fmax(computed2eyeTransmittance.tEnterVolume, 0);

        if (iterObject.intersectedObject()->material()->hasEmission()) {
            return iterObject.intersectedObject()->material()->getEmission()
                            * computed2eyeTransmittance.transmittance
                            * ( 1.0 / ( exp(-sigT * fromVolumeEntryToSurfaceT ) ) );
        }

        Vec3f surfaceColor;

        if (enableSampleLight) {
            SampledLight sampledLightObj = sampleLight_(iterObject.hitPoint());

            if (sampledLightObj.canReachLight) {
                ComputedTransmittance computed2lightTransmittance = computeTransmittance_(sampledLightObj.ray2light);
                Vec3f intersectedObjectNormal = iterObject.intersectedObject()->computeNormal(iterObject.hitPoint());

                surfaceColor += sampledLightObj.sampledEmission
                                * iterObject.intersectedObject()->material()->fr(sampledLightObj.ray2light.direction, -ray.direction, intersectedObjectNormal)
                                * std::fmax(cos(sampledLightObj.ray2light.direction, intersectedObjectNormal), 0)
                                * computed2lightTransmittance.transmittance
                                * (1.0 / sampledLightObj.sampleRatio)
                                * computed2eyeTransmittance.transmittance
                                * 1.0 / ( exp( -sigT * fromVolumeEntryToSurfaceT ) );
            }
        }

//        if (false) {
        if (util::getRandom01() < russiaRatio) {
            Vec3f intersectedObjectNormal = iterObject.intersectedObject()
                    ->computeNormal(iterObject.hitPoint()).normalized();

            std::pair<Vec3f, double> sampleIndirectDirectionPair = iterObject.intersectedObject()->material()->sampleOutDirection(
                    -ray.direction,
                    intersectedObjectNormal
            );
            Vec3f& sampleIndirectDirection = sampleIndirectDirectionPair.first;
            double sampleIndirectDirectionRatio = sampleIndirectDirectionPair.second;
            double cosNormalOutRay = cos(intersectedObjectNormal, sampleIndirectDirection);

            Ray nextRay(iterObject.hitPoint(), sampleIndirectDirection);
            auto nextIter = scene_->intersect(nextRay);

            if (!nextIter.happened() || !nextIter.intersectedObject()->material()->hasEmission()) {
                Vec3f tracedColor = scene_->trace(Ray(iterObject.hitPoint(), sampleIndirectDirection), depth);
                surfaceColor += iterObject.intersectedObject()->material()->fr(sampleIndirectDirection, -ray.direction, intersectedObjectNormal)
                                * cosNormalOutRay
                                * tracedColor
                                * (1.0f / sampleIndirectDirectionRatio)
                                * computed2eyeTransmittance.transmittance
                                * (1.0f / exp( -sigT * fromVolumeEntryToSurfaceT ))
                                * (1.0f / russiaRatio);
            }
        }

        return surfaceColor;
    }
};

#endif //CRENDERER_VOLUMELIGHTRENDERER_HPP
