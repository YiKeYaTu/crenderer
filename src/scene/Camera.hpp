//
// Created by lc on 2022/4/2.
//

#ifndef CRENDERER_CAMERA_HPP
#define CRENDERER_CAMERA_HPP

#include <core/Vec.hpp>

class Camera {
private:
    mutable Vec3f _cameraPos, _targetPos, _upDir;
    mutable Vec3f _front, _right, _up;

    void _initCoordinates() const {
        _front = Vec3f(_targetPos - _cameraPos).normalized();
        _right = crossProduct(_upDir, _front).normalized();
        _up = crossProduct(_front, _right).normalized();
    }

public:
    Camera(const Vec3f& pos, const Vec3f& target, const Vec3f& upDir)
        : _cameraPos(pos), _targetPos(target), _upDir(upDir) {
        _initCoordinates();
    }

    Mat4f calcViewMatrix() const {
        return Mat4f::LookAt(_cameraPos, _targetPos, _upDir);
    }

    void rotateRaw(float radian) const {
        _targetPos = Mat4f::Translation(_cameraPos)
            * Mat4f::Rotation(radian, _up)
            * Mat4f::Translation(-_cameraPos)
            * Vec4f(_targetPos, 1.0);
        _initCoordinates();
    }
    void rotatePitch(float radian) const {
        _targetPos = Mat4f::Translation(_cameraPos)
            * Mat4f::Rotation(radian, _right)
            * Mat4f::Translation(-_cameraPos)
            * Vec4f(_targetPos, 1.0);
        _initCoordinates();
    }

    void moveForward(float delta) const {
        _cameraPos = _cameraPos + _front * delta;
        _targetPos = _targetPos + _front * delta;
    }

    void moveSide(float delta) const {
        _cameraPos = _cameraPos + _right * delta;
        _targetPos = _targetPos + _right * delta;
    }
};


#endif //CRENDERER_CAMERA_HPP
