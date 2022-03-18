#ifndef OPENCL_RANDOM_CL
#define OPENCL_RANDOM_CL

#include "mwc64x/cl/mwc64x.cl"

mwc64x_state_t genSeed(uint baseOffset, uint perStreamOffset) {
    mwc64x_state_t rng;
    MWC64X_SeedStreams(&rng, baseOffset, perStreamOffset);
    return rng;
}

float rand(mwc64x_state_t* rng) {
    return ((float) (MWC64X_NextUint(rng) % UINT_MAX)) / UINT_MAX;
}

#endif