//
// Created by lc on 2022/3/29.
//

#ifndef CRENDERER_VEC_HPP
#define CRENDERER_VEC_HPP

#include <core/Mat.hpp>
#include <cmath>
#include <iostream>

template <unsigned int N, typename T>
class Vec: public Mat<N, 1, T> {
    typedef std::size_t size_type;
public:
    Vec(): Mat<N, 1, T>() {}
    Vec(const std::vector<T>& args): Mat<N, 1, T>(std::vector<std::vector<T>>(1, args)) {}
    Vec(const std::initializer_list<T>& args): Mat<N, 1, T>{ args } {}
    Vec(const Mat<N, 1, T>& mat): Mat<N, 1, T>(mat) {}

    template<unsigned int M>
    Vec<N, T> operator*(const Vec<M, T>& rhs) const {
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
class Vec3: public Vec<3, T> {
public:
    using Vec<3, T>::Vec;
    Vec3(const Vec<3, T>& vec): Vec<3, T>(vec) {}
    T x() { return (*this)[0]; }
    T y() { return (*this)[1]; }
    T z() { return (*this)[2]; }
};

template <typename T>
class Vec4: public Vec<4, T> {
public:
    using Vec<4, T>::Vec;
    Vec4(const Vec<4, T>& vec): Vec<4, T>(vec) {}
    T x() { return (*this)[0]; }
    T y() { return (*this)[1]; }
    T z() { return (*this)[2]; }
    T w() { return (*this)[3]; }
};


typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;
typedef Vec3<double> Vec3d;
typedef Vec4<double> Vec4d;

#endif //CRENDERER_VEC_HPP
