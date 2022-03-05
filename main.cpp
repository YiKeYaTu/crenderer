#include <iostream>
#include <chrono>
#include "Vector.hpp"
#include "object/MeshTriangle.hpp"
#include "object/Triangle.hpp"
#include "object/Sphere.hpp"
#include "acceleration/BVH.hpp"
#include "shader/VolumeLightShader.hpp"
#include "sample.hpp"

#include "material/CommonMaterial.hpp"
#include "material/CommonLightMaterial.hpp"
#include "material/SpotLightMaterial.hpp"

#include "material/property/DiffuseProperty.hpp"

const std::string prefix = "../models/";

CommonMaterial specularMaterial(Vec3f(0.725f, 0.71f, 0.68f), Vec3f(), Vec3f(), CommonMaterial::SPECULAR);
//CommonMaterial redMaterial(Vec3f(0.63f, 0.065f, 0.05f), Vec3f());
//CommonMaterial greenMaterial(Vec3f(0.14f, 0.45f, 0.091f), Vec3f());
//CommonMaterial whiteMaterial(Vec3f(0.725f, 0.71f, 0.68f), Vec3f());
CommonLightMaterial lightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));
SpotlightMaterial spotlightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));

CommonMaterial redMaterial(Vec3f(0.63f, 0.065f, 0.05f) / 5, Vec3f());
CommonMaterial greenMaterial(Vec3f(0.14f, 0.45f, 0.091f) / 5, Vec3f());
CommonMaterial whiteMaterial(Vec3f(0.725f, 0.71f, 0.68f) / 5, Vec3f());

void renderBunny() {
    MeshTriangle bunny = MeshTriangle::loadObjectFromObjFile(prefix + "bunny/bunny.obj", redMaterial);
    MeshTriangle bunnyLight = MeshTriangle::loadObjectFromObjFile(prefix + "bunny/bunny_light.obj", lightMaterial);

    Scene scene(400, 300, Vec3f(0, 0.065, -0.2), Vec3f(0.2, 0.3, 0.4));
//    scene.addObject(bunny);
//    scene.addObject(bunnyLight);

    auto start = std::chrono::system_clock::now();
    std::vector<Vec3f> frameBuffer = scene.render();
    auto stop = std::chrono::system_clock::now();

    util::saveFrameBufferAsPPM("bunny.ppm", frameBuffer, scene.width(), scene.height());

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";
}

void renderCornellBox() {
    MeshTriangle floor = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/floor.obj", whiteMaterial);
    MeshTriangle left = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/left.obj", redMaterial);
    MeshTriangle right = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/right.obj", greenMaterial);
    MeshTriangle tallbox = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/tallbox.obj", whiteMaterial);
    MeshTriangle shortbox = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/shortbox.obj", whiteMaterial);
    MeshTriangle light = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/light.obj", lightMaterial);
    MeshTriangle spotlight = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/light.obj", spotlightMaterial);
    Sphere sphere(Vec3f(150, 400, 208.333), 100.0, &specularMaterial);
    Sphere sphereVolume(Vec3f(250, 400, 250.333), 200.0, &specularMaterial);


    Scene scene(300, 300, Vec3f(278, 273, -800), Vec3f(), 45);
//    Scene scene(1920, 1080, Vec3f(278, 273, -800), Vec3f(), 45);
//    scene.addVolume(&sphereVolume);

//    scene.addObject(&sphereVolume);
    scene.addObject(&floor);
    scene.addObject(&left);
    scene.addObject(&right);
//    scene.addObject(&tallbox);
//    scene.addObject(&shortbox);
    scene.addObject(&light);
//    scene.addObject(&sphere);
//    scene.addObject(spotlight);

    PathTraceShader pathTraceShader(&scene);
    scene.setShader(&pathTraceShader);

    auto start = std::chrono::system_clock::now();
    std::vector<Vec3f> frameBuffer = scene.render(2);
    auto stop = std::chrono::system_clock::now();

    util::saveFrameBufferAsPPM("cornellBox.ppm", frameBuffer, scene.width(), scene.height());

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";
}

void renderCornellVolumeBox() {
    CommonLightMaterial lightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));
//    CommonLightMaterial lightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));

    std::cout << "rendering volume...." << std::endl;
    MeshTriangle tallbox = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/tallbox.obj", whiteMaterial);
    MeshTriangle shortbox = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/shortbox.obj", whiteMaterial);
    MeshTriangle light = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/light.obj", lightMaterial);
    MeshTriangle floor = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/floor.obj", whiteMaterial);
    MeshTriangle spotlight = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/light.obj", spotlightMaterial);
    MeshTriangle left = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/left.obj", redMaterial);
    MeshTriangle right = MeshTriangle::loadObjectFromObjFile(prefix + "cornellbox/right.obj", greenMaterial);
    Sphere sphereVolume(Vec3f(280, 305, 250.333), 400.0, &specularMaterial);


//    Scene scene(100, 100, Vec3f(278, 273, -800), Vec3f(), 45);

    Scene scene(300, 300, Vec3f(278, 273, -700), Vec3f(), 45);
    scene.addObject(&left);
    scene.addObject(&right);
    scene.addObject(&light);
    scene.addObject(&floor);
    scene.addObject(&tallbox);
    scene.addObject(&shortbox);
    scene.addVolume(&sphereVolume);

//    // scene.addObject(spotlight);

    VolumeLightShader volumeLightShader(&scene);
//    PathTraceShader pathTraceShader(&scene);
    scene.setShader(&volumeLightShader);

    auto start = std::chrono::system_clock::now();
    std::vector<Vec3f> frameBuffer = scene.render(32);
    auto stop = std::chrono::system_clock::now();

    util::saveFrameBufferAsPPM("cornellVolumeBox.ppm", frameBuffer, scene.width(), scene.height());

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";
}


int main() {
//    renderCornellBox();
//    renderBunny();
    renderCornellVolumeBox();

//    auto p = util::ThreadPool(16, 4 );
//    int ans = 0;
//
//    std::mutex m;
//
//    for (int i = 0; i < 10000000; ++i) {
//        p.tryAppendTask([i, &ans, &m]{
////            std::lock_guard<std::mutex> l(m);
//            ans += i;
//        });
//    }
//
//    p.stop();
//    p.allJoin();
//    std::cout << ans << std::endl;
//
//    ans = 0;
//    for (int i = 0; i < 10000000; ++i) {
//        ans += i;
//    }
//
//
//    std::cout << ans;



    return 0;
}
