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
    friend double cos(const Vec<T>& l, const Vec<T>& r) { return l.dot(r) / l.norm() / r.norm(); }

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

    Vec<T> operator+(const double rhs) const { return combine(*this, rhs, [](const T &v1, const double v2) { return v1 + v2; }); }
    Vec<T> operator-(const double rhs) const { return combine(*this, rhs, [](const T &v1, const double v2) { return v1 - v2; }); }
    Vec<T> operator*(const double rhs) const { return combine(*this, rhs, [](const T &v1, const double v2) { return v1 * v2; }); }
    Vec<T> operator/(const double rhs) const { return combine(*this, rhs, [](const T &v1, const double v2) { return v1 / v2; }); }

    Vec<T> operator+=(const double rhs) { return combine(rhs, [](const T &v1, const double v2) { return v1 + v2; }); }
    Vec<T> operator/=(const double rhs) { return combine(rhs, [](const T &v1, const double v2) { return v1 / v2; }); }

    Vec<T>& operator+=(const Vec<T> &rhs) { return combine(rhs, [](const T &v1, const T &v2) { return v1 + v2; }); }
    Vec<T>& operator-=(const Vec<T> &rhs) { return combine(rhs, [](const T &v1, const T &v2) { return v1 - v2; }); }
    Vec<T>& operator*=(const Vec<T> &rhs) { return combine(rhs, [](const T &v1, const T &v2) { return v1 * v2; }); }
    Vec<T>& operator/=(const Vec<T> &rhs) { return combine(rhs, [](const T &v1, const T &v2) { return v1 / v2; }); }

    Vec<T> operator-() {
        Vec<T> reverseVec(v);
        for (int i = 0; i < reverseVec.size(); ++i) {
            reverseVec[i] = -reverseVec[i];
        }
        return reverseVec;
    }

    T norm() const {
        T n = 0;
        for (const auto &item : v) {
            n += item * item;
        }
        n = sqrt(n);
        return n;
    }

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

    static Vec<T> minVec(std::size_t size) {
        return Vec(size, std::numeric_limits<double>::min());
    }

    static Vec<T> maxVec(std::size_t size) {
        return Vec(size, std::numeric_limits<double>::max());
    }

    static Vec<T> min(std::vector<Vec<T>> vectors) {
        assert(vectors.size() > 0);
        Vec<T> minVec(vectors[0].size(), std::numeric_limits<T>::max());
        for (const Vec<T>& vec : vectors) {
            for (int i = 0; i < vec.size(); ++ i) {
                minVec[i] = std::fmin(vec[i], minVec[i]);
            }
        }
        return minVec;
    }

    static Vec<T> max(std::vector<Vec<T>> vectors) {
        assert(vectors.size() > 0);
        Vec<T> minVec(vectors[0].size(), std::numeric_limits<T>::min());
        for (const Vec<T>& vec : vectors) {
            for (int i = 0; i < vec.size(); ++ i) {
                minVec[i] = std::fmax(vec[i], minVec[i]);
            }
        }
        return minVec;
    }

protected:
    std::vector<T> v;
    Vec<T> combine(const Vec<T> &lhs, const Vec<T> &rhs, std::function<T (const T&, const T&)> binaryOperator) const {
        assert(lhs.size() == rhs.size());
        Vec target(lhs.size(), 0);
        for (int i = 0; i < lhs.size(); ++i) {
            target[i] = binaryOperator(lhs[i], rhs[i]);
        }
        return target;
    }
    Vec<T> combine(const Vec<T> &lhs, const double &rhs, std::function<T (const T&, const T&)> binaryOperator) const {
        Vec target(lhs.size(), 0);
        for (int i = 0; i < lhs.size(); ++i) {
            target[i] = binaryOperator(lhs[i], rhs);
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

    Vec<T>& combine(const double &rhs, std::function<T (const T&, const T&)> binaryOperator) {
        for (int i = 0; i < v.size(); ++i) {
            v[i] = binaryOperator(v[i], rhs);
        }
        return *this;
    }
};

class Vec3f: public Vec<double> {
    friend Vec3f cross(const Vec3f &lhs, const Vec3f &rhs) {
        return lhs.cross(rhs);
    }
public:
    Vec3f(const Vec3f &v): Vec(v) { assert(v.size() == 3); }
    Vec3f(const Vec<double> &v): Vec(v) { assert(v.size() == 3); }
    Vec3f(const std::initializer_list<double> parameters): Vec{parameters} { assert(parameters.size() == 3); }
    Vec3f(double x, double y, double z): Vec{ x, y, z } {}
    Vec3f(): Vec<double>(3, 0) {}

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

    static Vec3f maxVec3f() {
        return Vec(3, std::numeric_limits<double>::max());
    }

    static Vec3f minVec3f() {
        return Vec(3, std::numeric_limits<double>::min());
    }
};

#endif //CRENDERER_VECTOR_HPP
