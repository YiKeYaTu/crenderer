#include <iostream>
#include "core/Vector.hpp"
#include "geometry/MeshTriangle.hpp"
#include "geometry/Triangle.hpp"
#include "acceleration/BVH.hpp"

int main() {
    Vec3f rayf1 { 1, 3, 5 };
    Vec3f rayf2 { 313, -321, -225 };

    Vec3f v0 { 0, 0, -10 };
    Vec3f v1 { 1, 1, 5 };
    Vec3f v2 { 1, 1, 19 };

    Vec3f v3(v0 + 10);
    Vec3f v4 (v1 + 10);
    Vec3f v5 (v2 + 10);

    Vec3f v6(v0 + 20);
    Vec3f v7 (v1 + 20);
    Vec3f v8 (v2 + 20);

    Triangle t1(v0, v1, v2);
    Triangle t2(v3, v4, v5);
    Triangle t3(v6, v7, v8);

    std::vector<std::shared_ptr<Geometry>> geometries{
        std::make_shared<Triangle>(t1),
        std::make_shared<Triangle>(t2),
        std::make_shared<Triangle>(t3)
    };

//    std::shared_ptr<BVH> bvh = BVH::buildBVH(geometries);

//    std::cout << t.bounds3 << std::endl;

//    std::cout << (rayf1 + rayf2) / 2 << std::endl;
//    std::cout << (rayf1) << std::endl;
//    std::cout << (rayf2) << std::endl;
//    std::cout << (rayf1.cross(rayf2)) << std::endl;
//
//    std::cout << "Hello, World!" << std::endl;
//
    MeshTriangle meshTriangle = MeshTriangle::loadFromObjFile("../models/bunny/bunny.obj");

    std::cout << Vec3f::min({ rayf1, rayf2 }) << std::endl;
    std::cout << Vec3f::max({ Vec3f { 1, 10, 2 }, Vec3f { -1, 11, 3 } }) << std::endl;

    return 0;
}
