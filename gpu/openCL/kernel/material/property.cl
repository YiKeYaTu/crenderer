#ifndef OPENCL_PROPERTY_CL
#define OPENCL_PROPERTY_CL

#include "include/OpenCLGlobal.h"
#include "include/material/C_Property.h"
#include "kernel/random.cl"

C_SampledDir sampleDiffuseDir(global C_Material* material, float3* wi, float3* n, mwc64x_state_t* rng) {
    float pdf = 1.0f / 2.0f / M_PI;
    // ensure that the cos of normal and sampled direction bigger than 0
    float a = 2 * M_PI * rand(rng);
    float b = 2 * M_PI * rand(rng);

    float x = 1 * cos(a);
    float y = 1 * sin(a);
    float z = sqrt(x * x + y * y) * tan(b);

    float3 dir = (float3) (x, y, z);

    if (dot(dir, *n) < 0) {
        dir = -dir;
    }
    
    dir = normalize(dir);

    return initSampledDir(&dir, pdf);
}
C_SampledDir sampleSpecularDir(global C_Material* material, float3* wi, float3* n, mwc64x_state_t* rng) {
    float pdf = 1.0;
    float3 wo = normalize(*n * 2 * (dot(*wi, *n) / length(*wi) / length(*n))  - *wi);
    return initSampledDir(&wo, pdf);
}
C_SampledDir sampleDir(global C_Material* material, float3* wi, float3* n, mwc64x_state_t* rng) {
    switch (material->propertyType) {
        case DIFFUSE:
            return sampleDiffuseDir(material, wi, n, rng);
        case SPECULAR:
            return sampleSpecularDir(material, wi, n, rng);
    }
}

float3 BRDFDiffuse(global C_Material* material, float3* wi, float3* wo, float3* n) {
    float c = dot(*wi, *n) / length(*wi) / length(*n);
    if (c <= 0) {
        return getZeroEmission();
    }
    return material->kd / M_PI;
}
float3 BRDFSpecular(global C_Material* material, float3* wi, float3* wo, float3* n) {
    if (dot(*wi, *n) < 0 || dot(*wo, *n) < 0) {
        return getZeroEmission();
    }
    float c = dot(*wi + *wo, *n) / length(*wi + *wo) / length(*n);
    return material->kd * pow(c, 150);
}
float3 BRDF(global C_Material* material, float3* wi, float3* wo, float3* n) {
    switch (material->propertyType) {
        case DIFFUSE:
            return BRDFDiffuse(material, wi, wo, n);
        case SPECULAR:
            return BRDFSpecular(material, wi, wo, n);
    }
}

#endif
