#ifndef OPENCL_INTERSECT_CL
#define OPENCL_INTERSECT_CL

#include "include/C_Bounds3.h"
#include "include/C_BVH.h"
#include "include/C_Intersection.h"
#include "include/C_Ray.h"
#include "include/object/C_Sphere.h"
#include "include/object/C_Object.h"

C_Intersection intersect(C_Ray* ray, global C_Object* objects, int objectIndex);
C_Intersection intersectBounds3(C_Ray* ray, global C_Bounds3* bounds3);
C_Intersection intersectSphere(C_Ray* ray, global C_Sphere* sphere, int objectIndex);
C_Intersection intersectTriangle(C_Ray* ray, global C_Triangle* triangle, int objectIndex);

global C_BVH* getParent(global C_BVH* bvhNodes, global C_BVH* bvhNode) {
    if (bvhNode->parentIndex == -1) {
        return 0;
    }
    return &bvhNodes[bvhNode->parentIndex];
}

global C_BVH* getLeftChild(global C_BVH* bvhNodes, global C_BVH* bvhNode) {
    if (bvhNode->leftChildIndex == -1) {
        return 0;
    }
    return &bvhNodes[bvhNode->leftChildIndex];
}

global C_BVH* getRightChild(global C_BVH* bvhNodes, global C_BVH* bvhNode) {
    if (bvhNode->rightChildIndex == -1) {
        return 0;
    }
    return &bvhNodes[bvhNode->rightChildIndex];
}

C_Intersection intersectBVHFaster(
    C_Ray* ray,
    global C_BVH* bvhNodes,
    global C_Object* objects,
    global void* heapBuffer
) {
    C_Intersection nearestIntersection = getEmptyIntersection();

    size_t globalGid = get_global_id(0);
    global int* bvhNodeIndexQueue = (global int*) &heapBuffer[globalGid * 63];
    size_t i = -1;
    size_t j = 1;
    
    bvhNodeIndexQueue[0] = 0;

    while (++ i < j) {
        global C_BVH* current = &bvhNodes[bvhNodeIndexQueue[i]];

        if (current->leftChildIndex < 0 && current->rightChildIndex < 0) {
            C_Intersection currentIntersection = intersect(ray, objects, current->objectIndex);

            if (currentIntersection.happened && currentIntersection.tMin < nearestIntersection.tMin) {
                nearestIntersection = currentIntersection;
            }
            continue;
        }

        C_Intersection intersection = intersectBounds3(ray, &current->bounds3);

        if (!intersection.happened) {
            continue;
        }

        if (current->leftChildIndex > 0) {
            bvhNodeIndexQueue[j ++] = current->leftChildIndex;
        }

        if (current->rightChildIndex > 0) {
            bvhNodeIndexQueue[j ++] = current->rightChildIndex;
        }
    }

    return nearestIntersection;
}

C_Intersection intersectBVHSlower(
    C_Ray* ray,
    global C_BVH* bvhNodes,
    global C_Object* objects
) {
    C_Intersection nearestIntersection = getEmptyIntersection();

    const int fromParent = 1, fromChild = 2;
    int status = fromParent;

    global C_BVH* current = &bvhNodes[0];
    global C_BVH* parent;
    global C_BVH* next;

    while (true) {
        parent = getParent(bvhNodes, current);
        next = 0;

        switch (status) {
            case fromParent: {
                if (current->leftChildIndex < 0 && current->rightChildIndex < 0) {
                    C_Intersection currentIntersection = intersect(ray, objects, current->objectIndex);

                    if (currentIntersection.happened && currentIntersection.tMin < nearestIntersection.tMin) {
                        nearestIntersection = currentIntersection;
                    }

                    next = current;
                    status = fromChild;
                    break;
                }

                C_Intersection intersection = intersectBounds3(ray, &current->bounds3);

                if (!intersection.happened) {
                    next = current;
                    status = fromChild;
                    break;
                }

                next = getLeftChild(bvhNodes, current);
                status = fromParent;

                if (!next) {
                    next = getRightChild(bvhNodes, current);

                    if (!next) {
                        next = current;
                        status = fromChild;
                    }
                }
                break;
            }
            case fromChild: {
                if (!parent) {
                    return nearestIntersection;
                }

                if (current == getLeftChild(bvhNodes, parent)) {
                    status = fromParent;
                    next = getRightChild(bvhNodes, parent);
                }

                if (!next) {
                    status = fromChild;
                    next = parent;
                }

                break;
            }
        }

        current = next;
    }
}


C_Intersection intersectBVH(
    C_Ray* ray,
    global C_BVH* bvhNodes,
    global C_Object* objects,
    global void* heapBuffer
) {
//    return intersectBVHFaster(ray, bvhNodes, objects, heapBuffer);
    return intersectBVHSlower(ray, bvhNodes, objects);
}

C_Intersection intersectTriangle(C_Ray* ray, global C_Triangle* triangle, int objectIndex) {
    float3 o = ray->origin;
    float3 d = ray->direction;
    float3 v13 = triangle->v0 - triangle->v2;
    float3 v23 = triangle->v1 - triangle->v2;

    float c11 = -d.x, c12 = v13.x, c13 = v23.x;
    float c21 = -d.y, c22 = v13.y, c23 = v23.y;
    float c31 = -d.z, c32 = v13.z, c33 = v23.z;

    float D = c11*c22*c33 + c12*c23*c31 + c13*c21*c32 - c13*c22*c31 - c11*c23*c32 - c12*c21*c33;

    float r1 = o.x - triangle->v2.x;
    float r2 = o.y - triangle->v2.y;
    float r3 = o.z - triangle->v2.z;

    float D1 = r1*c22*c33 + c12*c23*r3 + c13*r2*c32 - c13*c22*r3 - r1*c23*c32 - c12*r2*c33;
    float D2 = c11*r2*c33 + r1*c23*c31 + c13*c21*r3 - c13*r2*c31 - c11*c23*r3 - r1*c21*c33;
    float D3 = c11*c22*r3 + c12*r2*c31 + r1*c21*c32 - r1*c22*c31 - c11*r2*c32 - c12*c21*r3;

    float t = D1 / D;
    float a = D2 / D, b = D3 / D, c = 1.0f - a - b;

    float eps = 1e-15;

    if (t < 0 || a + eps < 0 || b + eps < 0 || c + eps < 0) {
        return getEmptyIntersection();
    }

    float3 hitPoint = triangle->v0 * a + triangle->v1 * b + triangle->v2 * c + normalize(triangle->n) * 0.0001f;
    float distance = length(ray->direction * t);

    return initIntersection(
        t,
        t,
        distance,
        &hitPoint,
        objectIndex
    );
}

C_Intersection intersectBounds3(C_Ray* ray, global C_Bounds3* bounds3) {
    float tMin = FLT_MIN;
    float tMax = FLT_MAX;

    float t1, t2;

    for (int i = 0; i < 3; ++i) {
        t1 = (bounds3->min[i] - ray->origin[i]) * ray->recDirection[i];
        t2 = (bounds3->max[i] - ray->origin[i]) * ray->recDirection[i];

        if (t1 < 0 && t2 < 0) { return getEmptyIntersection(); }

        tMin = max(min(t1, t2), tMin);
        tMax = min(max(t1, t2), tMax);
    }

    if (tMin <= tMax) {
        C_Intersection intersection = {
            .happened = true,
            .tMin = tMin,
            .tMax = tMax
        };
        return intersection;
    }

    return getEmptyIntersection();
}

C_Intersection intersectSphere(C_Ray* ray, global C_Sphere* sphere, int objectIndex) {
    const float3 d = ray->direction;
    const float3 p = ray->origin;
    const float3 center = sphere->center;

    float a = length(d) * length(d);
    float b = 2 * dot(d, p - center);
    float c = dot(p - center, p - center) - pow(sphere->radius, 2);

    float delta = b*b - 4*a*c;

    float tMin, tMax;

    if (delta < 0) {
        return getEmptyIntersection();
    } else if (delta == 0) {
        tMin = tMax = -b / (2*a);
    } else {
        tMin = ( -b - sqrt(delta) ) / (2*a);
        tMax = ( -b + sqrt(delta) ) / (2*a);
    }

    if (tMin <= 0 && tMax <= 0) {
        return getEmptyIntersection();
    }

    float3 hitPoint = ray->origin + ray->direction * (tMin < 0 ? tMax : tMin);
    float distance = length(ray->direction * (tMin < 0 ? tMax : tMin));

    return initIntersection(
        tMin,
        tMax,
        distance,
        &hitPoint,
        objectIndex
    );
}

C_Intersection intersect(C_Ray* ray, global C_Object* objects, int objectIndex) {
    switch ((objects + objectIndex)->subType) {
        case TRIANGLE:
            return intersectTriangle(ray, &((objects + objectIndex)->triangle), objectIndex);
        case SPHERE:
            return intersectSphere(ray, &((objects + objectIndex)->sphere), objectIndex);
    }
}

#endif
