//
// Created by lc on 2022/3/15.
//

#ifndef CRENDERER_BVH2LIST_HPP
#define CRENDERER_BVH2LIST_HPP

#include <vector>

#include "../../../acceleration/BVH.hpp"
#include "../../../object/MeshTriangle.hpp"
#include "../../../object/Triangle.hpp"
#include "../../../object/Sphere.hpp"

#include "../include/C_BVH.h"

#include "bounds32cBounds3.hpp"
#include "vec32float3.hpp"
#include "triangle2cTriangle.hpp"
#include "sphere2cSphere.hpp"


void bvh2cBvh_(std::vector<C_BVH>& cBvhs, std::vector<C_Object>& objects, BVH* bvh, int parentIndex, bool isLeftChild) {
    if (!bvh) {
        return;
    }

    if (bvh->geometry() && bvh->geometry()->type() == MESH_TRIANGLE) {
        bvh2cBvh_(cBvhs, objects, static_cast<MeshTriangle *>(bvh->geometry())->bvh().get(), parentIndex,
                  isLeftChild);
        return;
    }

    cBvhs.push_back({
        .bounds3 = bounds32cBounds3(bvh->bounds3()),
        .parentIndex = parentIndex,
        .leftChildIndex = -1,
        .rightChildIndex = -1,
        .objectIndex = -1
    });

    int currentBvhIndex = cBvhs.size() - 1;
    C_BVH& current = cBvhs.back();

    if (parentIndex > -1) {
        if (isLeftChild) {
            cBvhs[parentIndex].leftChildIndex = currentBvhIndex;
        } else {
            cBvhs[parentIndex].rightChildIndex = currentBvhIndex;
        }
    }

    if (!bvh->left() && !bvh->right()) {
        C_Object object = {
                .bounds3 = bounds32cBounds3(bvh->geometry()->bounds3()),
                .centroid = vec32float3(bvh->geometry()->centroid()),
                .normal = vec32float3(bvh->geometry()->normal()),
                .subType = bvh->geometry()->type()
        };

        switch (object.subType) {
            case TRIANGLE:
                object.triangle = triangle2cTriangle(static_cast<Triangle*>(bvh->geometry()));
                break;
            case SPHERE:
                object.sphere = sphere2cSphere(static_cast<Sphere*>(bvh->geometry()));
                break;
        }

        cBvhs[currentBvhIndex].objectIndex = objects.size();
        objects.push_back(object);
    } else {
        bvh2cBvh_(cBvhs, objects, bvh->left().get(), currentBvhIndex, true);
        bvh2cBvh_(cBvhs, objects, bvh->right().get(), currentBvhIndex, false);
    }
}


std::pair<std::vector<C_BVH>, std::vector<C_Object>> bvh2cBvh(BVH* bvh) {
    std::vector<C_BVH> cBvhs;
    std::vector<C_Object> objects;

    bvh2cBvh_(cBvhs, objects, bvh, -1, false);

    return { cBvhs, objects };
}

#endif //CRENDERER_BVH2LIST_HPP
