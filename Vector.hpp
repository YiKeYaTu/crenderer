//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_VECTOR_HPP
#define CRENDERER_VECTOR_HPP

#include <vector>
#include <iostream>
#include <cmath>

template <typename T = double>
class Vec;

class Vec3f;

template <typename T>
class Vec {
    friend std::ostream &operator<<(std::ostream &os, const Vec<T> &vec) {
        for (auto it = vec.v.begin(); it != vec.v.end(); it ++) {
            os << *it;
            if (it + 1 != vec.v.end()) {
                os << ", ";
            }
        }
        return os;
    }

    friend T dot(const Vec<T>& lhs, const Vec<T>& rhs) { return lhs.dot(rhs); }
    friend Vec<T> normalized(const Vec<T>& vec) { return vec.normalized(); }

public:
    Vec(const std::vector<T>& v): v(v) {}
    Vec(const std::initializer_list<T> parameters): v{parameters} {}
    Vec(std::size_t size, T el): v(size, el) {}
    Vec(std::size_t size): v(size, 0) {}
    Vec() = default;

    Vec<T> operator+(const Vec<T> &rhs) const { return combine(*this, rhs, [](const T &v1, const T &v2) { return v1 + v2; }); }
    Vec<T> operator-(const Vec<T> &rhs) const { return combine(*this, rhs, [](const T &v1, const T &v2) { return v1 - v2; }); }
    Vec<T> operator*(const Vec<T> &rhs) const { return combine(*this, rhs, [](const T &v1, const T &v2) { return v1 * v2; }); }
    Vec<T> operator/(const Vec<T> &rhs) const { return combine(*this, rhs, [](const T &v1, const T &v2) { return v1 / v2; }); }
    Vec<T>& operator+=(const Vec<T> &rhs) const { return combine(rhs, [](const T &v1, const T &v2) { return v1 + v2; }); }
    Vec<T>& operator-=(const Vec<T> &rhs) const { return combine(rhs, [](const T &v1, const T &v2) { return v1 - v2; }); }
    Vec<T>& operator*=(const Vec<T> &rhs) const { return combine(rhs, [](const T &v1, const T &v2) { return v1 * v2; }); }
    Vec<T>& operator/=(const Vec<T> &rhs) const { return combine(rhs, [](const T &v1, const T &v2) { return v1 / v2; }); }

    Vec<T> normalized() const {
        T n = 0;
        for (const auto &item : v) {
            n += sqrt(item * item);
        }
        return Vec{v[0] / n, v[1] / n, v[2] / n};
    }

    T dot(const Vec<T>& rhs) const {
        T acc = 0;
        for (int i = 0; i < size(); ++i) {
            acc += v[i] * rhs[i];
        }
        return acc;
    }

    T& operator[](const int i) {
        return v[i];
    }

    const T& operator[](const int i) const {
        return v[i];
    }

    std::size_t size() const {
        return v.size();
    }

protected:
    std::vector<T> v;

    Vec<T> combine(const Vec<T> &lhs, const Vec<T> &rhs, std::function<T (const T&, const T&)> binaryOperator) {
        assert(lhs.size() == rhs.size());
        Vec target(lhs.size(), 0);
        for (int i = 0; i < lhs.size(); ++i) {
            target[i] = binaryOperator(lhs[i], rhs[i]);
        }
        return target;
    }

    Vec<T>& combine(const Vec<T> &rhs, std::function<T (const T&, const T&)> binaryOperator) {
        assert(size() == rhs.size());
        for (int i = 0; i < v.size(); ++i) {
            v[i] = binaryOperator(v[i], rhs[i]);
        }
        return *this;
    }
};

class Vec3f: public Vec<double> {
    friend Vec3f cross(const Vec3f &lhs, const Vec3f &rhs) {
        return lhs.cross(rhs);
    }
public:
    Vec3f(const Vec<double> &v): Vec(v) { assert(v.size() == 3); }
    Vec3f(const Vec3f &v): Vec(v) { assert(v.size() == 3); }
    Vec3f(const std::initializer_list<double> parameters): Vec{parameters} { assert(parameters.size() == 3); }
    Vec3f(double x, double y, double z): Vec{ x, y, z } {}
    Vec3f() = default;

    const double x() const {
        return v[0];
    }

    const double y() const {
        return v[1];
    }

    const double z() const {
        return v[2];
    }

    Vec3f cross(const Vec3f &rhs) const {
        return Vec3f(
                y() * rhs.z() - z() * rhs.y(),
                z() * rhs.x() - x() * rhs.z(),
                x() * rhs.y() - y() * rhs.x()
        );
    }
};

#endif //CRENDERER_VECTOR_HPP
