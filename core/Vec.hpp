//
// Created by lc on 2022/3/29.
//

#ifndef CRENDERER_VEC_HPP
#define CRENDERER_VEC_HPP

#include <functional>
#include <vector>
#include <iostream>
#include <cmath>
#include <assert.h>

#define arithmeticalCreatingVec(op) \
friend \
Vec operator op(const Vec& lhs, const Vec& rhs) { \
    Vec newVec(lhs); \
    _map(newVec, [&rhs](const size_type i, const T& value) { \
        return value op rhs[i]; \
    }); \
    return newVec; \
}

#define arithmeticalAssignmentVec(op, ar) \
friend \
Vec& operator op(Vec& lhs, const Vec& rhs) { \
    _map(lhs, [&rhs](const size_type i, const T& value) { \
        return value ar rhs[i]; \
    }); \
    return lhs; \
}

#define arithmeticalCreatingSca(op) \
friend \
Vec operator op(const Vec& lhs, const T& rhs) { \
    Vec newVec(lhs); \
    _map(newVec, [&rhs](const size_type i, const T& value) { \
        return value op rhs; \
    }); \
    return newVec; \
}

#define arithmeticalAssignmentSca(op, ar) \
friend \
Vec& operator op(Vec& lhs, const T& rhs) { \
    _map(lhs, [&rhs](const size_type i, const T& value) { \
        return value ar rhs; \
    }); \
    return lhs; \
}

template <unsigned int N, typename T>
class Vec {
    static_assert(std::is_floating_point_v<T> == true);
public:
    typedef std::size_t size_type;
    Vec(): Vec(N, 0) {}
    Vec(size_type count, const T& value): _container(count, value) {}
    Vec(const std::initializer_list<T> parameters): _container{parameters} {
        assert(parameters.size() == N);
    }
    template <unsigned int N1>
    Vec(const Vec<N1, T>& vec): Vec() {
        assert(_container.size() <= vec.size());
        for (int i = 0; i < _container.size(); ++i) {
            _container[i] = vec[i];
        }
    }

    inline const T operator[](const int i) const { return _container[i]; }
    inline T& operator[](const int i) { return _container[i]; }
    inline const size_type size() const { return _container.size(); }

    Vec operator-() {
        Vec negVec(*this);
        _map(negVec, [](const size_type i, const T& value) { return -value; });
        return negVec;
    }

    friend
    void _foreach(const Vec<N, T>& vec, std::function<void (const T& value)> fn) {
        for (const T& item : vec._container) {
            fn(item);
        }
    }
    friend
    void _map(Vec<N, T>& vec, std::function<T (const size_type i, const T& value)> fn) {
        for (int i = 0; i < vec._container.size(); ++i) {
            vec[i] = fn(i, vec._container[i]);
        }
    }
    friend
    std::ostream &operator<<(std::ostream &os, const Vec<N, T>& vec) {
        _foreach(vec, [&os](const T& value) -> void {
            os << value << " ";
        });
        return os;
    }

    arithmeticalCreatingVec(+)
    arithmeticalCreatingVec(-)
    arithmeticalCreatingVec(*)
    arithmeticalCreatingVec(/)
    arithmeticalCreatingSca(+)
    arithmeticalCreatingSca(-)
    arithmeticalCreatingSca(*)
    arithmeticalCreatingSca(/)

    arithmeticalAssignmentVec(+=, +)
    arithmeticalAssignmentVec(-=, -)
    arithmeticalAssignmentVec(*=, *)
    arithmeticalAssignmentVec(/=, /)
    arithmeticalAssignmentSca(+=, +)
    arithmeticalAssignmentSca(-=, -)
    arithmeticalAssignmentSca(*=, *)
    arithmeticalAssignmentSca(/=, /)

protected:
    std::vector<T> _container;
};

template <typename T>
class Vec3: public Vec<3, T> {
public:
    using Vec<3, T>::Vec;
    Vec3(const T& x, const T& y, const T& z): Vec<3, T>{ x, y, z } {}

    inline const T x() const { return this->_container[0]; }
    inline const T y() const { return this->_container[1]; }
    inline const T z() const { return this->_container[2]; }
};

template <typename T>
class Vec4: public Vec<4, T> {
public:
    using Vec<4, T>::Vec;
    Vec4(const T& x, const T& y, const T& z, const T& w): Vec<4, T>{ x, y, z, w } {}
    inline const T x() const { return this->_container[0]; }
    inline const T y() const { return this->_container[1]; }
    inline const T z() const { return this->_container[2]; }
    inline const T w() const { return this->_container[3]; }
};

typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;

#endif //CRENDERER_VEC_HPP
