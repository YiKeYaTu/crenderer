//
// Created by lc on 2022/3/29.
//

#ifndef CRENDERER_VEC_HPP
#define CRENDERER_VEC_HPP

#include "Mat.hpp"
#include <cmath>
#include <iostream>

template <unsigned int N, typename T>
class Vec: public Mat<N, 1, T> {
    typedef std::size_t sizeType;
public:
    Vec(): Mat<N, 1, T>() {}
    Vec(Vec& vec) = default;
    template <unsigned int M>
    Vec(Vec<M, T>& vec): Mat<N, 1, T>(vec) {}
    Vec(Vec<N - 1, T>& vec, T value): Mat<N, 1, T>(vec, value) {}
    Vec(const std::array<T, N>& args): Mat<N, 1, T>(std::array<std::array<T, N>, 1> { args }) {}
    Vec(const std::initializer_list<T>& args): Mat<N, 1, T>{ args } {}
    Vec(const Mat<N, 1, T>& mat): Mat<N, 1, T>(mat) {}

    Vec& operator=(const Mat<N, 1, T>& mat) {
        Mat<N, 1, T>::_container = mat.container();
        return *this;
    }

    Vec<N, T> operator*(const Vec& rhs) const {
        return Mat<N, 1, T>::cwiseProduct(rhs);
    }

    Vec<N, T> operator*(const T& rhs) const {
        return Mat<N, 1, T>::cwiseProduct(rhs);
    }

    Vec<N, T> normalized() const {
        return Vec<N, T>(*this / length());
    }

    T dot(const Vec& rhs) const {
        return (this->transpose() * rhs)[0][0];
    }

    T length() const {
        T length = 0;
        for (const auto& item : Mat<N, 1, T>::_container[0]) {
            length += item * item;
        }
        return sqrt(length);
    }

    inline const T& operator[](const int i) const { return Mat<N, 1, T>::_container[0][i]; }
    inline T& operator[](const int i) { return Mat<N, 1, T>::_container[0][i]; }
};

template <unsigned int N, typename T>
Vec<N, T> normalize(Vec<N, T>& a) {
    return a.normalized();
}

template <unsigned int N, typename T>
T dot(Vec<N, T>& a, Vec<N, T>& b) {
    return a.dot(b);
}

template <unsigned int N, typename T>
T length(Vec<N, T>& a) {
    return a.length();
}

template <unsigned int N, typename T>
std::ostream& operator<<(std::ostream &os, const Vec<N, T>& vec) {
    for (int i = 0; i < N; ++i) {
        os << vec[i] << std::endl;
    }
    return os;
}


template <typename T>
class Vec2: public Vec<2, T> {
public:
    using Vec<2, T>::Vec;
    Vec2(const Vec<2, T>& vec): Vec<2, T>(vec) {}
    Vec2(T u, T v): Vec<2, T> { u, v } {}
    T x() const { return (*this)[0]; }
    T y() const { return (*this)[1]; }
    T u() const { return (*this)[0]; }
    T v() const { return (*this)[1]; }
};

template <typename T>
class Vec3: public Vec<3, T> {
public:
    using Vec<3, T>::Vec;
    using Vec<3, T>::operator=;
    Vec3(const Vec<3, T>& vec): Vec<3, T>(vec) {}
    Vec3(T x, T y, T z): Vec<3, T> { x, y, z } {}
    Vec3 crossProduct(const Vec3& rhs) const {
        return Vec3{
            y() * rhs.z() - z() * rhs.y(),
            z() * rhs.x() - x() * rhs.z(),
            x() * rhs.y() - y() * rhs.x()
        };
    }
    T x() const { return (*this)[0]; }
    T y() const { return (*this)[1]; }
    T z() const { return (*this)[2]; }
};

template <typename T>
Vec3<T> crossProduct(const Vec3<T>& lhs, const Vec3<T>& rhs) {
    return lhs.crossProduct(rhs);
}

template <typename T>
class Vec4: public Vec<4, T> {
public:
    using Vec<4, T>::Vec;
    Vec4(const Vec<4, T>& vec): Vec<4, T>(vec) {}
    Vec4(T x, T y, T z, T w): Vec<4, T> { x, y, z, w } {}
    T x() const { return (*this)[0]; }
    T y() const { return (*this)[1]; }
    T z() const { return (*this)[2]; }
    T w() const { return (*this)[3]; }
};

template <typename T>
Mat<4, 4, T> Mat4<T>::LookAt(const Mat<3, 1, T> &eyePos, const Mat<3, 1, T> &targetPos, const Mat<3, 1, T> &upDir) {
    Vec3<T> front(targetPos - eyePos);
    Vec3<T> right(crossProduct(Vec3<T>(upDir), Vec3<T>(front)));
    Vec3<T> up(crossProduct(Vec3<T>(front), Vec3<T>(right)));

    front = front.normalized();
    right = right.normalized();
    up = up.normalized();

    auto lookAt = Mat<4, 4, T>::Identity();
    auto trans = Mat4f::Translation(-eyePos);

    lookAt[0][0] = right[0];
    lookAt[0][1] = up[0];
    lookAt[0][2] = front[0];
    lookAt[0][3] = 0;
    lookAt[1][0] = right[1];
    lookAt[1][1] = up[1];
    lookAt[1][2] = front[1];
    lookAt[1][3] = 0;
    lookAt[2][0] = right[2];
    lookAt[2][1] = up[2];
    lookAt[2][2] = front[2];
    lookAt[2][3] = 0;

    return lookAt * trans;
}


typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;
typedef Vec3<double> Vec3d;
typedef Vec4<double> Vec4d;

#endif //CRENDERER_VEC_HPP
