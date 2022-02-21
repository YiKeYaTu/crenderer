#include <iostream>
#include "Vector.hpp"
#include "Polygon/Triangle.hpp"
#include "Polygon/MeshTriangle.hpp"

int main() {
    Vec3f rayf1 { 1, 3, 5 };
    Vec3f rayf2 { -1, -321, -225 };

    std::cout << (rayf1 + rayf2) << std::endl;
    std::cout << (rayf1) << std::endl;
    std::cout << (rayf2) << std::endl;
    std::cout << (rayf1.cross(rayf2)) << std::endl;

    std::cout << "Hello, World!" << std::endl;

    MeshTriangle::loadFromObjFile("../models/cornellbox/tallbox.obj");

    return 0;
}
