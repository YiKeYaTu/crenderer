//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_DEFAULTSCENE_HPP
#define CRENDERER_DEFAULTSCENE_HPP

#include "Scene.hpp"
#include "object/MeshTriangle.hpp"
#include "object/Sphere.hpp"
#include "material/CommonLightMaterial.hpp"
#include "material/SpotLightMaterial.hpp"

namespace DefaultScene {
    const std::string prefix = "../models/";

    std::shared_ptr<Scene> getCornellVolumeBox() {
        static CommonMaterial specularMaterial(Vec3f(0.725f, 0.71f, 0.68f), Vec3f(), Vec3f(), PropertyType ::SPECULAR);
        static CommonLightMaterial lightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));
        static SpotlightMaterial spotlightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));

        static CommonMaterial redMaterial(Vec3f(0.63f, 0.065f, 0.05f) / 5, Vec3f());
        static CommonMaterial greenMaterial(Vec3f(0.14f, 0.45f, 0.091f) / 5, Vec3f());
        static CommonMaterial whiteMaterial(Vec3f(0.725f, 0.71f, 0.68f) / 5, Vec3f());

        static MeshTriangle tallbox = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/tallbox.obj",
                                                                          whiteMaterial);
        static MeshTriangle shortbox = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/shortbox.obj",
                                                                           whiteMaterial);
        static MeshTriangle light = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/light.obj", lightMaterial);
        static MeshTriangle floor = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/floor.obj", whiteMaterial);
        static MeshTriangle spotlight = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/light.obj",
                                                                            spotlightMaterial);
        static MeshTriangle left = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/left.obj", redMaterial);
        static MeshTriangle right = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/right.obj", greenMaterial);
        static Sphere sphereVolume(Vec3f(280, 305, 250.333), 400.0, &specularMaterial);

        std::shared_ptr<Scene> scene(new Scene(2560, 1600, Vec3f(278, 273, -700), Vec3f(), 45));
        scene->addObject(&left);
        scene->addObject(&right);
        scene->addObject(&spotlight);
        scene->addObject(&floor);
        scene->addObject(&tallbox);
        scene->addObject(&shortbox);
        scene->addVolume(&sphereVolume);
        scene->buildBvh();

        return scene;
    }
}

#endif //CRENDERER_DEFAULTSCENE_HPP
