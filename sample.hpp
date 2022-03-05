//
// Created by lc on 2022/3/2.
//

#ifndef CRENDERER_SAMPLE_HPP
#define CRENDERER_SAMPLE_HPP

#include <algorithm>
#include <vector>
#include <random>
#include <stdexcept>

template <typename T = double, bool Interpolate = true>
class SampledDistribution {
    struct Sample {
        T prob, value;
        Sample(const T p, const T v): prob(p), value(v) {}
        friend bool operator<(T p, const Sample& s) { return p < s.prob; }
    };

    std::vector<Sample> SampledCDF;

public:
    struct InvalidBounds:   std::runtime_error { using std::runtime_error::runtime_error; };
    struct CDFNotMonotonic: std::runtime_error { using std::runtime_error::runtime_error; };

    template <typename F>
    SampledDistribution(F&& cdfFunc, const T low, const T high, const unsigned resolution = 256) {
        if (low >= high) throw InvalidBounds("");
        SampledCDF.reserve( resolution );
        const T cdfLow = cdfFunc(low);
        const T cdfHigh = cdfFunc(high);
        for (unsigned i = 0; i < resolution; ++i) {
            const T x = (high - low)*i/(resolution-1) + low;
            const T p = (cdfFunc(x) - cdfLow)/(cdfHigh - cdfLow); // normalising
            if (SampledCDF.size() > 0 && p < SampledCDF.back()) throw CDFNotMonotonic("");
            SampledCDF.emplace_back(p, x);
        }
    }

    template <typename Engine>
    T operator()(Engine& g) {
        const T cdf = std::uniform_real_distribution<T>{0.,1.}(g);
        auto s = std::upper_bound(SampledCDF.begin(), SampledCDF.end(), cdf);
        if (Interpolate && s != SampledCDF.begin()) {
            auto bs = s - 1;
            const T r = (cdf - bs->prob)/(s->prob - bs->prob);
            return r*bs->value + (1 - r)*s->value;
        }
        return s->value;
    }
};

#endif //CRENDERER_SAMPLE_HPP
