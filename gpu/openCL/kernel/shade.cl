#ifndef OPENCL_SHADE_CL
#define OPENCL_SHADE_CL

#include "include/C_Bounds3.h"
#include "include/C_BVH.h"
#include "include/C_Intersection.h"
#include "include/object/C_Object.h"
#include "include/C_Ray.h"
#include "include/material/C_Material.h"
#include "include/material/C_Property.h"
#include "include/sample/C_SampledDir.h"

#include "kernel/random.cl"
#include "kernel/intersect.cl"
#include "kernel/material/material.cl"
#include "kernel/material/property.cl"
#include "kernel/object/object.cl"

#define RAY_SCALE 100.0f
// Global settings for rendering volume scattering
#define sigT 0.02f
#define sigS 0.01f
#define enableSampleLight true

C_Ray initCray(float3* origin, float3* direction) {
    C_Ray cRay = {
        .scale = 1.0f,
        .origin = *origin,
        .direction = *direction * RAY_SCALE,
        .recDirection = 1.0f / (*direction * RAY_SCALE),
    };
    return cRay;
}

typedef struct {
    global C_Object *light;
    C_Intersection intersectionWithLight;
    float sampleRatio;

    float3 sampledLightPosition;
    float3 sampledLightNormal;
    float3 sampledEmission;

    C_Ray ray2light;
    bool canReachLight;
} SampledLight;

SampledLight initSampledLight(
    global C_Object *light,
    C_Intersection* intersectionWithLight,
    float sampleRatio,

    float3* sampledLightPosition,
    float3* sampledLightNormal,
    float3* sampledEmission,

    C_Ray* ray2light,
    bool canReachLight
) {
    SampledLight sampledLight = {
        .light = light,
        .intersectionWithLight = *intersectionWithLight,
        .sampleRatio = sampleRatio,
        .sampledLightPosition = *sampledLightPosition,
        .sampledLightNormal = *sampledLightNormal,
        .sampledEmission = *sampledEmission,
        .ray2light = *ray2light,
        .canReachLight = canReachLight
    };
    return sampledLight;
}

typedef struct {
    C_Intersection iterObject;
    C_Intersection iterVolume;
    float tEnterVolume;
    float tLeaveVolume;
    float tInVolume;
    float transmittance;
} ComputedTransmittance;

ComputedTransmittance computeTransmittance(
    C_Ray* ray2somewhere,
    global C_BVH* bvhNodes,
    global C_BVH* volumeBvhNodes,
    global C_Object* objects,
    global C_Object* volumeObjects
) {
    float transmittance = 1.0, tEnterVolume, tLeaveVolume, tInVolume;

    C_Intersection iterObject = intersectBVH(ray2somewhere, bvhNodes, objects);
    C_Intersection iterVolume = intersectBVH(ray2somewhere, volumeBvhNodes, volumeObjects);

    if (iterVolume.happened) {
        tEnterVolume = min( iterVolume.tMin, iterObject.tMin );
        tLeaveVolume = min( iterObject.tMin, iterVolume.tMax );
        tInVolume = tLeaveVolume - max( tEnterVolume, 0.f );
        transmittance = exp( -sigT * tInVolume );
    } else {
        tEnterVolume = tLeaveVolume = tInVolume = 0;
    }
    
    ComputedTransmittance computedTransmittance = {
        .iterObject = iterObject,
        .iterVolume = iterVolume,
        .tEnterVolume = tEnterVolume,
        .tLeaveVolume = tLeaveVolume,
        .tInVolume = tInVolume,
        .transmittance = transmittance
    };

    return computedTransmittance;
}

global C_Object* sampleLight_(mwc64x_state_t* rng, float3* samplePosition, float* sampleRatio, global C_Object* lightObjects, size_t numLightObjects) {
    float area = 0;

    for (int i = 0; i < numLightObjects; i ++) {
        C_Bounds3 bounds3 = lightObjects[i].bounds3;
        float length = bounds3.max.x - bounds3.min.x;
        float width = bounds3.max.z - bounds3.min.z;
        area += width * length;
    }

    float target = rand(rng) * area;
    float currenArea = 0;

    for (int i = 0; i < numLightObjects; i ++) {
        global C_Object* candiLight = lightObjects + i;
        C_Bounds3 bounds3 = candiLight->bounds3;
        
        float length = bounds3.max.x - bounds3.min.x;
        float width = bounds3.max.z - bounds3.min.z;

        currenArea += width * length;

        if (currenArea >= target) {
            float a = rand(rng);
            float b = rand(rng);
            float c = rand(rng);

            *samplePosition = (float3) (
                    bounds3.min.x + (bounds3.max.x - bounds3.min.x) * a,
                    bounds3.min.y + (bounds3.max.y - bounds3.min.y) * b,
                    bounds3.min.z + (bounds3.max.z - bounds3.min.z) * c
            );
            *sampleRatio = 1.0f / area;
            return candiLight;
        }
    }
}

SampledLight sampleLight(
    float3* point,
                         
    global C_BVH* bvhNodes,
                         
    global C_Object* objects,
    global C_Object* lightObjects,
                         
    size_t numLightObjects,

    mwc64x_state_t* rng
) {
    float3 sampledLightPosition;
    float sampleRatio;
    global C_Object* light = sampleLight_(rng, &sampledLightPosition, &sampleRatio, lightObjects, numLightObjects);

    float r2 = pow( length( *point - sampledLightPosition ), 2 );
    float3 light2point = normalize(*point - sampledLightPosition);

    float3 sampledLightNormal = computeNormal(light, &sampledLightPosition);
    float3 sampledEmission = getEmission(&light->material, point, &sampledLightPosition, &sampledLightNormal, light)
        * (1.0f / r2)
        * max(dot(sampledLightNormal, light2point) / length(sampledLightNormal) / length(light2point), 0.f);
    
    float3 rayDir = normalize(sampledLightPosition - *point);
    C_Ray ray2light = initCray(point, &rayDir);
    
    C_Intersection intersectionWithLight = intersectBVH(&ray2light, bvhNodes, objects);
    bool canReachLight = length(intersectionWithLight.hitPoint - sampledLightPosition) < 0.1;

    return initSampledLight(
        light, &intersectionWithLight, sampleRatio,
        &sampledLightPosition, &sampledLightNormal, &sampledEmission,
        &ray2light, canReachLight
    );
}

typedef struct {
    float t;
    float pdf;
} SampledScattering;

SampledScattering sampleScattering(float sigma, float min, mwc64x_state_t* rng) {
    float randomCDF = rand(rng);
    float t = -log(1 - randomCDF) / sigma + min;
    SampledScattering sampledScattering = {
        .t = t,
        .pdf = sigma * exp(-sigma * t)
    };
    return sampledScattering;
}

float Schlick(float3* wi, float3* wo, float k) {
    float k2 = pow(k, 2);
    float c = dot(*wi, *wo) / length(*wi) / length(*wo);
    return (1 - k2) / (4 * M_PI * pow(1 + k * c, 2));
}

C_SampledDir sampleInScatteringDirection(mwc64x_state_t* rng) {
    float a = 2 * M_PI * rand(rng);
    float b = 2 * M_PI * rand(rng);

    float x = 1 * cos(a);
    float y = 1 * sin(a);
    float z = sqrt(x * x + y * y) * tan(b);

    float3 direction = (float3) (x, y, z);
    
    C_SampledDir sampledDir = {
        .dir = normalize(direction),
        .pdf = 1.0f / (4.0f * M_PI)
    };
    
    return sampledDir;
}

float3 shadeParticle(
    C_Ray* ray,
    float russiaRatio,
    SampledScattering* sampledScattering,
    ComputedTransmittance* computedTransmittance,
                     
    global C_BVH* bvhNodes,
    global C_BVH* volumeBvhNodes,
           
    global C_Object* objects,
    global C_Object* volumeObjects,
    global C_Object* lightObjects,
           
    global uint* numObjects,
    global uint* numVolumeObjects,
    global uint* numLightObjects,

    mwc64x_state_t* rng,
    float3* coefficient,
    C_Ray* nextRay,
    bool* next
) {
    *next = false;
    float sampledScatteringT = sampledScattering->t;
    float tPDF = sampledScattering->pdf;

    float3 particlePosition = ray->origin + ray->direction * sampledScatteringT;
    float3 particleColor = (float3) (0.f, 0.f, 0.f);
    
    float3 negRayDir = -ray->direction;

    if (enableSampleLight) {
        SampledLight sampledLightObj = sampleLight(&particlePosition, bvhNodes, objects, lightObjects, *numLightObjects, rng);
        if (sampledLightObj.canReachLight) {
            ComputedTransmittance computedTransmittance = computeTransmittance(&sampledLightObj.ray2light, bvhNodes, volumeBvhNodes, objects, volumeObjects);
            particleColor += sampledLightObj.sampledEmission
                             * computedTransmittance.transmittance
                             * (1.0f / 1.0f)
                             * Schlick(&sampledLightObj.ray2light.direction, &negRayDir, 0.5f)
                             * (sigS / sigT)
                             * (1.0f / sampledLightObj.sampleRatio);
        }
    }
    
    if (rand(rng) < russiaRatio) {
        C_SampledDir sampleIndirectDirectionPair = sampleInScatteringDirection(rng);
        *nextRay = initCray(&particlePosition, &sampleIndirectDirectionPair.dir);

        C_Intersection nextIter = intersectBVH(nextRay, bvhNodes, objects);

        if (!nextIter.happened || !hasEmission(&objects[nextIter.intersectedObjectIndex].material)) {
            *next = true;
            *coefficient = Schlick(&sampleIndirectDirectionPair.dir, &negRayDir, 0.5)
                * (sigS / sigT)
                * (1.0f / sampleIndirectDirectionPair.pdf)
                * (1.0f / russiaRatio)
                * sigT
                * exp( -sigT * ( computedTransmittance->tEnterVolume >= 0 ? (sampledScatteringT - computedTransmittance->tEnterVolume) : sampledScatteringT ) )
                * (1.0f / tPDF);
        }
    }

    particleColor = particleColor
                    * sigT
                    * exp( -sigT * ( computedTransmittance->tEnterVolume >= 0 ? (sampledScatteringT - computedTransmittance->tEnterVolume) : sampledScatteringT ) )
                    * (1.0f / tPDF);
    return particleColor;
}

float3 shadeSurface(
    C_Ray* ray,
    float russiaRatio,
    ComputedTransmittance* computed2eyeTransmittance,
                     
    global C_BVH* bvhNodes,
    global C_BVH* volumeBvhNodes,
           
    global C_Object* objects,
    global C_Object* volumeObjects,
    global C_Object* lightObjects,
           
    global uint* numObjects,
    global uint* numVolumeObjects,
    global uint* numLightObjects,

    mwc64x_state_t* rng,
    float3* coefficient,
    C_Ray* nextRay,
    bool* next
) {
    *next = false;
    
    C_Intersection iterObject = computed2eyeTransmittance->iterObject;
    float fromVolumeEntryToSurfaceT = computed2eyeTransmittance->iterObject.tMin - max(computed2eyeTransmittance->tEnterVolume, 0.f);
    
    if (hasEmission(&objects[iterObject.intersectedObjectIndex].material)) {
        return objects[iterObject.intersectedObjectIndex].material.emission
            * computed2eyeTransmittance->transmittance
            * ( 1.0f / ( exp(-sigT * fromVolumeEntryToSurfaceT ) ) );
    }
    
    float3 surfaceColor = (float3) (0.f, 0.f, 0.f);
    float3 negRayDir = -(ray->direction);
    float3 intersectedObjectNormal = computeNormal(&objects[iterObject.intersectedObjectIndex], &iterObject.hitPoint);

    if (enableSampleLight) {
        SampledLight sampledLightObj = sampleLight(&iterObject.hitPoint, bvhNodes, objects, lightObjects, *numLightObjects, rng);

        if (sampledLightObj.canReachLight) {
            ComputedTransmittance computed2lightTransmittance = computeTransmittance(&sampledLightObj.ray2light, bvhNodes, volumeBvhNodes, objects, volumeObjects);

            surfaceColor += sampledLightObj.sampledEmission
                            * BRDF(&objects[iterObject.intersectedObjectIndex].material, &sampledLightObj.ray2light.direction, &negRayDir, &intersectedObjectNormal)
                            * max(dot(sampledLightObj.ray2light.direction, intersectedObjectNormal) / length(sampledLightObj.ray2light.direction) / length(intersectedObjectNormal), 0.f)
                            * computed2lightTransmittance.transmittance
                            * (1.0f / sampledLightObj.sampleRatio)
                            * computed2eyeTransmittance->transmittance
                            * (1.0f / exp( -sigT * fromVolumeEntryToSurfaceT ) );
        }
    }

    if (rand(rng) < russiaRatio) {
        C_SampledDir sampleIndirectDirectionPair = sampleDir(&objects[iterObject.intersectedObjectIndex].material, &negRayDir, &intersectedObjectNormal, rng);

        float3 sampleIndirectDirection = sampleIndirectDirectionPair.dir;
        float sampleIndirectDirectionRatio = sampleIndirectDirectionPair.pdf;
        float cosNormalOutRay = dot(intersectedObjectNormal, sampleIndirectDirection);

        *nextRay = initCray(&iterObject.hitPoint, &sampleIndirectDirection);
        C_Intersection nextIter = intersectBVH(nextRay, bvhNodes, objects);

        if (!nextIter.happened || !hasEmission(&objects[nextIter.intersectedObjectIndex].material)) {
            *next = true;
            *coefficient = BRDF(&objects[iterObject.intersectedObjectIndex].material, &sampleIndirectDirection, &negRayDir, &intersectedObjectNormal)
                * cosNormalOutRay
                * (1.0f / sampleIndirectDirectionRatio)
                * computed2eyeTransmittance->transmittance
                * (1.0f / exp( -sigT * fromVolumeEntryToSurfaceT ))
                * (1.0f / russiaRatio);
        }
    }

    return surfaceColor;
}

float3 shade(
     global C_Ray* ray,
                   
     global C_BVH* bvhNodes,
     global C_BVH* volumeBvhNodes,
                   
     global C_Object* objects,
     global C_Object* volumeObjects,
     global C_Object* lightObjects,
                   
     global uint* numObjects,
     global uint* numVolumeObjects,
     global uint* numLightObjects,
     
     size_t gid
) {
    int depth = 0;
    float3 coefficient = (float3) (1.0f, 1.0f, 1.0f);
    float3 nextCoefficient;
    bool next = false;
    
    float3 computedPixelColor = (float3) (0.f, 0.f, 0.f);
    
    mwc64x_state_t rng = genSeed(gid, 1);
    C_Ray localRay = *ray;
    C_Ray nextRay;

    while (true) {
        depth ++;
        float russiaRatio = 1.0;
        if (depth > 5) { russiaRatio = 0.5; }

        float3 surfaceColor = (float3) (0.f, 0.f, 0.f);
        float3 particleColor = (float3) (0.f, 0.f, 0.f);

        ComputedTransmittance computedTransmittance = computeTransmittance(&localRay, bvhNodes, volumeBvhNodes, objects, volumeObjects);

        SampledScattering sampledScattering = sampleScattering(sigT, max( computedTransmittance.tEnterVolume, 0.f ), &rng);
        float sampledScatteringT = sampledScattering.t;
        float tPDF = sampledScattering.pdf;

        if (computedTransmittance.iterVolume.happened) {
            float3 particlePosition = localRay.origin + localRay.direction * sampledScatteringT;
            // if ray doesn't intersect any object,
            // or the sampled time lesser than intersected time
            if (!computedTransmittance.iterObject.happened || sampledScatteringT < computedTransmittance.iterObject.tMin) {
                particleColor += shadeParticle(
                    &localRay,
                    russiaRatio,
                    &sampledScattering,
                    &computedTransmittance,

                    bvhNodes,
                    volumeBvhNodes,

                    objects,
                    volumeObjects,
                    lightObjects,

                    numObjects,
                    numVolumeObjects,
                    numLightObjects,

                    &rng,
                    &nextCoefficient,
                    &nextRay,
                    &next
                );
            }
        }

        if (computedTransmittance.iterObject.happened && sampledScatteringT >= computedTransmittance.iterObject.tMin) {
            surfaceColor += shadeSurface(
                &localRay,
                russiaRatio,
                &computedTransmittance,

                bvhNodes,
                volumeBvhNodes,

                objects,
                volumeObjects,
                lightObjects,

                numObjects,
                numVolumeObjects,
                numLightObjects,

                &rng,
                &nextCoefficient,
                &nextRay,
                &next
            );
        }

        computedPixelColor += coefficient * (surfaceColor + particleColor);
        coefficient = nextCoefficient;
        
        if (!next) {
            break;
        }
    }
    
    return computedPixelColor;
}

#endif
