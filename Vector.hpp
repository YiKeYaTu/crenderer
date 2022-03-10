//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_VECTOR_HPP
#define CRENDERER_VECTOR_HPP

#include <functional>
#include <vector>
#include <iostream>
#include <cmath>
#include <assert.h>

class Vec3f {
private:
    std::vector<double> vec_;
public:
    friend std::ostream &operator<<(std::ostream &os, const Vec3f& vec3f) {
        os << vec3f.x() << ", " << vec3f.y() << ", " << vec3f.z();
        return os;
    }
    friend double dot(const Vec3f& l, const Vec3f& r) { return l.dot(r); }
    friend Vec3f normalized(const Vec3f& vec3f) { return vec3f.normalized(); }
    friend double cos(const Vec3f& l, const Vec3f& r) { return l.cos(r); }
    friend Vec3f crossProduct(const Vec3f& l, const Vec3f& r) { return l.cross(r); }

    Vec3f operator+(const Vec3f& rhs) const { return Vec3f{x() + rhs.x(), y() + rhs.y(), z() + rhs.z()}; }
    Vec3f operator-(const Vec3f& rhs) const { return Vec3f{x() - rhs.x(), y() - rhs.y(), z() - rhs.z()}; }
    Vec3f operator*(const Vec3f& rhs) const { return Vec3f{x() * rhs.x(), y() * rhs.y(), z() * rhs.z()}; }
    Vec3f operator/(const Vec3f& rhs) const { return Vec3f{x() / rhs.x(), y() / rhs.y(), z() / rhs.z()}; }
    Vec3f& operator+=(const Vec3f& rhs) { vec_[0] += rhs.x(); vec_[1] += rhs.y(); vec_[2] += rhs.z(); return *this; }
    Vec3f& operator-=(const Vec3f& rhs) { vec_[0] -= rhs.x(); vec_[1] -= rhs.y(); vec_[2] -= rhs.z(); return *this; }
    Vec3f& operator*=(const Vec3f& rhs) { vec_[0] *= rhs.x(); vec_[1] *= rhs.y(); vec_[2] *= rhs.z(); return *this; }
    Vec3f& operator/=(const Vec3f& rhs) { vec_[0] /= rhs.x(); vec_[1] /= rhs.y(); vec_[2] /= rhs.z(); return *this; }
    Vec3f operator+(double rhs) const { return Vec3f{x() + rhs, y() + rhs, z() + rhs}; }
    Vec3f operator-(double rhs) const { return Vec3f{x() - rhs, y() - rhs, z() - rhs}; }
    Vec3f operator*(double rhs) const { return Vec3f{x() * rhs, y() * rhs, z() * rhs}; }
    Vec3f operator/(double rhs) const { return Vec3f{x() / rhs, y() / rhs, z() / rhs}; }
    Vec3f& operator+=(double rhs) { vec_[0] += rhs; vec_[1] += rhs; vec_[2] += rhs; return *this; }
    Vec3f& operator-=(double rhs) { vec_[0] -= rhs; vec_[1] -= rhs; vec_[2] -= rhs; return *this; }
    Vec3f& operator*=(double rhs) { vec_[0] *= rhs; vec_[1] *= rhs; vec_[2] *= rhs; return *this; }
    Vec3f& operator/=(double rhs) { vec_[0] /= rhs; vec_[1] /= rhs; vec_[2] /= rhs; return *this; }

    Vec3f operator-() const { return Vec3f{-x(), -y(), -z()}; }

    Vec3f(const Vec3f& vec3f): vec_(vec3f.vec_) { }
    Vec3f(const std::initializer_list<double> parameters): vec_{parameters} { assert(parameters.size() == 3); }
    Vec3f(double x, double y, double z): vec_{ x, y, z } {}
    Vec3f(std::size_t size, double e): vec_(size, e) { assert(size == 3); }
    Vec3f(): vec_(3, 0) {}

    const double x() const { return vec_[0]; }
    const double y() const { return vec_[1]; }
    const double z() const { return vec_[2]; }
    double cos(const Vec3f& rhs) const { return dot(rhs) / norm() / rhs.norm(); }
    double norm() const { return sqrt(vec_[0]*vec_[0] + vec_[1]*vec_[1] + vec_[2]*vec_[2]); }
    double norm2() const { double n = norm(); return n*n; }
    Vec3f normalized() const { const double m = norm(); return Vec3f{vec_[0]/m, vec_[1]/m, vec_[2]/m}; }
    double dot(const Vec3f& rhs) const { return vec_[0]*rhs.vec_[0] + vec_[1]*rhs.vec_[1] + vec_[2]*rhs.vec_[2]; }
    const double operator[](const int i) const { return vec_[i]; }
    double& operator[](const int i) { return vec_[i]; }
    std::size_t size() const { return vec_.size(); }
    Vec3f cross(const Vec3f &rhs) const {
        return Vec3f(
                y() * rhs.z() - z() * rhs.y(),
                z() * rhs.x() - x() * rhs.z(),
                x() * rhs.y() - y() * rhs.x()
        );
    }

    static Vec3f maxVec3f() { return Vec3f(3, std::numeric_limits<double>::max()); }
    static Vec3f minVec3f() { return Vec3f(3, std::numeric_limits<double>::min()); }

    static Vec3f min(std::vector<Vec3f> vectors) {
        Vec3f minVec(3, std::numeric_limits<double>::max());
        for (const Vec3f& vec3f : vectors) {
            minVec[0] = std::fmin(vec3f[0], minVec[0]);
            minVec[1] = std::fmin(vec3f[1], minVec[1]);
            minVec[2] = std::fmin(vec3f[2], minVec[2]);
        }
        return minVec;
    }

    static Vec3f max(std::vector<Vec3f> vectors) {
        Vec3f minVec(3, std::numeric_limits<double>::min());
        for (const Vec3f& vec3f : vectors) {
            minVec[0] = std::fmax(vec3f[0], minVec[0]);
            minVec[1] = std::fmax(vec3f[1], minVec[1]);
            minVec[2] = std::fmax(vec3f[2], minVec[2]);
        }
        return minVec;
    }
};

#endif //CRENDERER_VECTOR_HPP
