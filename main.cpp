#include <iostream>
#include "Vector.hpp"
#include "MeshTriangle.hpp"
#include "geometry/Triangle.hpp"
#include "acceleration/BVH.hpp"
#include "Scene.hpp"

int main() {
//    Vec3f rayf1 { 1, 3, 5 };
//    Vec3f rayf2 { 313, -321, -225 };
//
//    Vec3f v0 { 0, 0, -10 };
//    Vec3f v1 { 1, 1, 5 };
//    Vec3f v2 { 1, 1, 19 };
//
//    Vec3f v3(v0 + 10);
//    Vec3f v4 (v1 + 10);
//    Vec3f v5 (v2 + 10);
//
//    Vec3f v6(v0 + 20);
//    Vec3f v7 (v1 + 20);
//    Vec3f v8 (v2 + 20);
//
//    Triangle t1(v0, v1, v2);
//    Triangle t2(v3, v4, v5);
//    Triangle t3(v6, v7, v8);
//
//    std::vector<std::shared_ptr<Geometry>> geometries{
//        std::make_shared<Triangle>(t1),
//        std::make_shared<Triangle>(t2),
//        std::make_shared<Triangle>(t3)
//    };

//    std::shared_ptr<BVH> bvh = BVH::buildBVH(geometries);

//    std::cout << t.bounds3 << std::endl;

//    std::cout << (rayf1 + rayf2) / 2 << std::endl;
//    std::cout << (rayf1) << std::endl;
//    std::cout << (rayf2) << std::endl;
//    std::cout << (rayf1.cross(rayf2)) << std::endl;
//
//    std::cout << "Hello, World!" << std::endl;
//
//    double x = 281.33333333333331, y = 110, z = 160;
//    Ray ray1({ x, y, z}, { 281.33333333333331 - x, 110 - y, 349.33333333333331 - z });
//
//    Intersection nearestIntersection;
//
//    for (const auto& item : meshTriangle.triangles()) {
//        auto intersection = item->intersect(ray1);
//        if (intersection.happened() && intersection.t() < nearestIntersection.t()) {
//            nearestIntersection = intersection;
//        }
//    }
//
//
//    Intersection intersection = meshTriangle.intersect(ray1);
//    assert(intersection.t() == nearestIntersection.t());
//
//    std::cout << Vec3f::min({ rayf1, rayf2 }) << std::endl;
//    std::cout << Vec3f::max({ Vec3f { 1, 10, 2 }, Vec3f { -1, 11, 3 } }) << std::endl;

//    Bounds3 b1(Vec3f { 0, 0, 0 }, { 1, 1, 1 } );
//    Ray ray1({-1, 0, -1}, { 1, 1, 1 });
//    Ray ray2({0.5, 0.5, 0.5}, { 1, 1, 1 });
//    Ray ray3({0, 0, 1.1}, { 0, 0, 1 });
//    Ray ray4({-0.5, 0.5, 0.5}, { 0.9, 1, 0 });
//
//
//    Intersection intersection1 = b1.intersect(ray1);
//    Intersection intersection2 = b1.intersect(ray2);
//    Intersection intersection3 = b1.intersect(ray3);
//    Intersection intersection4 = b1.intersect(ray4);

    Material redMaterial(Vec3f(0.63f, 0.065f, 0.05f), Vec3f());
    Material greenMaterial(Vec3f(0.14f, 0.45f, 0.091f), Vec3f());
    Material whiteMaterial(Vec3f(0.725f, 0.71f, 0.68f), Vec3f());
    Material lightMaterial(Vec3f(0.65f, 0.65f, 0.65f), Vec3f(), (Vec3f(0.747f+0.058f, 0.747f+0.258f, 0.747f)*8.0f + Vec3f(0.740f+0.287f,0.740f+0.160f,0.740f)*15.6f + Vec3f(0.737f+0.642f,0.737f+0.159f,0.737f)*18.4f));
//    Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f *Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
//    light->Kd = Vector3f(0.65f);

    MeshTriangle floor = MeshTriangle::loadObjectFromObjFile("../models/cornellbox/floor.obj", whiteMaterial);
    MeshTriangle left = MeshTriangle::loadObjectFromObjFile("../models/cornellbox/left.obj", redMaterial);
    MeshTriangle right = MeshTriangle::loadObjectFromObjFile("../models/cornellbox/right.obj", greenMaterial);
    MeshTriangle tallbox = MeshTriangle::loadObjectFromObjFile("../models/cornellbox/tallbox.obj", whiteMaterial);
    MeshTriangle shortbox = MeshTriangle::loadObjectFromObjFile("../models/cornellbox/shortbox.obj", whiteMaterial);
    MeshTriangle light = MeshTriangle::loadObjectFromObjFile("../models/cornellbox/light.obj", lightMaterial);

    Scene scene(300, 300);
    scene.addObject(floor);
    scene.addObject(left);
    scene.addObject(right);
    scene.addObject(tallbox);
    scene.addObject(shortbox);
    scene.addObject(light);

    auto start = std::chrono::system_clock::now();
    std::vector<Vec3f> frameBuffer = scene.render();
    auto stop = std::chrono::system_clock::now();

    util::saveFrameBufferAsPPM(frameBuffer, scene.width(), scene.height());

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";


    return 0;
}
