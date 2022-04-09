//
// Created by lc on 2022/4/2.
//

#ifndef CRENDERER_CAMERA_HPP
#define CRENDERER_CAMERA_HPP

#include <core/Vec.hpp>

class Camera {
private:
    mutable Vec3f _cameraPos, _upDir;
    mutable Vec3f _front, _right, _up;
public:
    Camera(const Vec3f& pos, const Vec3f& target, const Vec3f& upDir)
        : _cameraPos(pos), _upDir(upDir.normalized()) {
        _front = Vec3f(target - _cameraPos).normalized();
        _right = crossProduct(_upDir, _front).normalized();
        _up = crossProduct(_front, _right).normalized();
    }

    Mat4f calcViewMatrix() const { return Mat4f::LookAt(_cameraPos, _right, _up, _front); }
    Mat4f calcProjectionMatrix(float aspectRatio = 1.0, float zn = 0.01, float zf = 2000, float fov = 60.0 / 180 *
            M_PI) const {
        return Mat4f::Perspective(zn, zf, fov, aspectRatio);
    }
    Mat4f calcOrthogonalProjectionMatrix(
        float left = -4, float right = 4,
        float bottom = -4, float top = 4,
        float near = 0.1, float far = 2000
    ) const {
        return Mat4f::Orthogonal(left, right, bottom, top, near, far);
    }

    void rotateRaw(float radian) const {
        auto rotationMatrix = Mat4f::Rotation(radian, _upDir);
        _front = Vec3f(rotationMatrix * Vec4f(_front, 1.0)).normalized();
        _right = Vec3f(rotationMatrix * Vec4f(_right, 1.0)).normalized();
        _up = crossProduct(_front, _right).normalized();
    }
    void rotatePitch(float radian) const {
        auto rotationMatrix = Mat4f::Rotation(radian, _right);
        _front = Vec3f(rotationMatrix * Vec4f(_front, 1.0)).normalized();
        _up = Vec3f(rotationMatrix * Vec4f(_up, 1.0)).normalized();
        _right = crossProduct(_up, _front).normalized();
    }

    void moveForward(float delta) const {
        Vec3f forwardDir(_front.x(), 0, _front.z());
        _cameraPos = _cameraPos + forwardDir * delta;
    }

    void moveSide(float delta) const {
        Vec3f sideDir(_right.x(), 0, _right.z());
        _cameraPos = _cameraPos + sideDir * delta;
    }
};


#endif //CRENDERER_CAMERA_HPP
