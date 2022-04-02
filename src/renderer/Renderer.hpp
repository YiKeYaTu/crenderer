//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_RENDERER_HPP
#define CRENDERER_RENDERER_HPP

#include <vector>
#include <core/Vec.hpp>
#include <scene/Scene.hpp>
#include <scene/Camera.hpp>

class Renderer {
public:
    Renderer() {}

    virtual void render(const Scene& scene, const Camera& camera) const = 0;
//    virtual std::vector<Vec3f> renderFrame(const Scene& scene) const = 0;
};

#endif //CRENDERER_RENDERER_HPP
