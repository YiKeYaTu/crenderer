//
// Created by lc on 2022/3/29.
//

#include <core/Vec.hpp>
#include <core/Mat.hpp>

int main() {
    Mat4f m(Mat4f::Translation(Vec3f{ 1, 3, 5 }));
    Vec4f v { 1, 3, 5, 1 };

    std::cout << m * v << std::endl;

    return 0;
}