#include <iostream>
#include <chrono>
#include "Vector.hpp"
#include "object/MeshTriangle.hpp"
#include "object/Triangle.hpp"
#include "acceleration/BVH.hpp"
#include "Scene.hpp"

const std::string prefix = "G:/crenderer/models/";

Material redMaterial(Vec3f(0.63f, 0.065f, 0.05f), Vec3f());
Material greenMaterial(Vec3f(0.14f, 0.45f, 0.091f), Vec3f());
Material whiteMaterial(Vec3f(0.725f, 0.71f, 0.68f), Vec3f());
Material lightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));

void renderBunny() {
    MeshTriangle bunny = MeshTriangle::loadObjectFromObjFile(prefix + "bunny/bunny.obj", redMaterial);
    MeshTriangle bunnyLight = MeshTriangle::loadObjectFromObjFile(prefix + "bunny/bunny_light.obj", lightMaterial);

    Scene scene(300, 300, Vec3f(0, 0.065, -0.2), Vec3f(0.2, 0.3, 0.4));
    scene.addObject(bunny);
    scene.addObject(bunnyLight);

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

    Scene scene(300, 300, Vec3f(278, 273, -800), Vec3f());

    scene.addObject(floor);
    scene.addObject(left);
    scene.addObject(right);
    scene.addObject(tallbox);
    scene.addObject(shortbox);
    scene.addObject(light);

    auto start = std::chrono::system_clock::now();
    std::vector<Vec3f> frameBuffer = scene.render();
    auto stop = std::chrono::system_clock::now();

    util::saveFrameBufferAsPPM("cornellBox.ppm", frameBuffer, scene.width(), scene.height());

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";
}

int main() {
    renderCornellBox();
//    renderBunny();

    return 0;
}
