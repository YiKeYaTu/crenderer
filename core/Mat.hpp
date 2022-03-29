//
// Created by lc on 2022/3/29.
//

#ifndef CRENDERER_MAT_HPP
#define CRENDERER_MAT_HPP

#include <core/Vec.hpp>
#include <iostream>

//template <typename T>
//class Mat4 {
//    static_assert(std::is_floating_point_v<T> == true);
//
//    friend
//    std::ostream& operator<<(std::ostream &os, const Mat4<T>& mat4) {
//        for (int i = 0; i < 4; ++i) {
//            for (int j = 0; j < 4; ++j) {
//                os << mat4._container[j][i] << " ";
//            }
//            os << std::endl;
//        }
//        return os;
//    }
//
//    friend
//    Mat4 operator*(const Mat4<T>& lhs, const Mat4<T>& rhs) {
//        Mat4 newMat4;
//        for (int i = 0; i < 4; ++i) {
//            for (int j = 0; j < 4; ++j) {
//                T temp = 0;
//                for (int k = 0; k < 4; ++k) {
//                    temp += lhs[k][i] * rhs[j][k];
//                }
//                newMat4[j][i] = temp;
//            }
//        }
//        return newMat4;
//    }
//
//    friend
//    Vec4<T> operator*(const Mat4<T>& lhs, const Vec4<T>& rhs) {
//        Vec4<T> newVec4;
//        for (int i = 0; i < 4; ++i) {
//            T temp = 0;
//            for (int k = 0; k < 4; ++k) {
//                temp += lhs[k][i] * rhs[k];
//            }
//            newVec4[i] = temp;
//        }
//        return newVec4;
//    }
//
//public:
//    typedef std::size_t size_type;
//
//    Mat4() = delete;
//    Mat4(const std::initializer_list<Vec4<T>>& vectors) {
//        assert(vectors.size() <= 4);
//        _container.resize(16);
//        const auto begin = vectors.begin();
//        for (int i = 0; i < vectors.size(); ++i) {
//            _container[i] = *(begin + i);
//        }
//    }
//
//    inline const Vec4<T> operator[](const int i) const { return _container[i]; }
//    inline Vec4<T>& operator[](const int i) { return _container[i]; }
//
//    static Mat4<T> Identity() {
//        return Mat4<T>({
//           Vec4<T> { 1, 0, 0, 0 },
//           Vec4<T> { 0, 1, 0, 0 },
//           Vec4<T> { 0, 0, 1, 0 },
//           Vec4<T> { 0, 0, 0, 1 },
//        });
//    }
//
//    static Mat4<T> Zero() {
//        return Mat4<T>({
//            Vec4<T> { 0, 0, 0, 0 },
//            Vec4<T> { 0, 0, 0, 0 },
//            Vec4<T> { 0, 0, 0, 0 },
//            Vec4<T> { 0, 0, 0, 0 },
//        });
//    }
//
//    static Mat4<T> Translation(const Vec3<T>& translation) {
//        return Mat4<T>({
//            Vec4<T> { 1, 0, 0, 0 },
//            Vec4<T> { 0, 1, 0, 0 },
//            Vec4<T> { 0, 0, 1, 0 },
//            Vec4<T> { translation.x(), translation.y(), translation.z(), 1 },
//        });
//    }
//
//    static Mat4<T> Rotation(float radian, const Vec3<T>& axis) {
//        float c = cos(radian), s = sin(radian);
//
//        return c*Mat4<T>::Identity() + (1 - c)*axis*axis.transpose() + s*[ 0, -axis.z];
//    }
//
//    static Mat4<T> Perspective(float zNear, float zFar, float fov, float aspectRatio) {
//
//    }
//
//protected:
//    std::vector<Vec4<T>> _container;
//
//};
//
//typedef Mat4<float> Mat4f;

#define cwiseOperationMat(operatorName, op) \
Mat operatorName(const Mat& rhs) { \
    Mat retMat; \
    for (int i = 0; i < row; ++i) { \
        for (int j = 0; j < col; ++j) { \
            retMat[j][i] = _container[j][i] op rhs[j][i]; \
        } \
    } \
    return retMat; \
}


#define cwiseOperationScalar(operatorName, op) \
Mat operatorName(const T& rhs) { \
    Mat retMat; \
    for (int i = 0; i < row; ++i) { \
        for (int j = 0; j < col; ++j) { \
            retMat[j][i] = _container[j][i] op rhs; \
        } \
    } \
    return retMat; \
}


template <unsigned int row, unsigned int col, typename T>
class Mat {
    static_assert(std::is_floating_point_v<T> == true);
    static_assert(row > 0 && col > 0);

public:
    Mat(): Mat(std::vector<std::vector<T>>(col, std::vector<T>(row, 0))) {}
    Mat(const std::vector<std::vector<T>>& mat): _container(mat) {
        assert(mat.size() == col && mat[0].size() == row);
    }

    inline const std::vector<T>& operator[](const int i) const { return _container[i]; }
    inline std::vector<T>& operator[](const int i) { return _container[i]; }

    template<unsigned int row2, unsigned int col2>
    Mat<row, col2, T> operator*(const Mat<row2, col2, T>& rhs) {
        static_assert(col == row2);
        Mat<row, col2, T> newMat;
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col2; ++j) {
                T temp = 0;
                for (int k = 0; k < col; ++k) {
                    temp += _container[k][i] * rhs[j][k];
                }
                newMat[j][i] = temp;
            }
        }
        return newMat;
    }

    Mat operator-() {
        Mat negMat;
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                negMat[j][i] = -_container[j][i];
            }
        }
        return negMat;
    }

    Mat<col, row, T> transpose() {
        Mat<col, row, T> transposedMat;
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                transposedMat[i][j] = _container[j][i];
            }
        }
        return transposedMat;
    }

    cwiseOperationMat(cwiseAdd, +)
    cwiseOperationMat(cwiseSubtract, -)
    cwiseOperationMat(cwiseProduct, *)
    cwiseOperationMat(cwiseDivide, /)
    cwiseOperationScalar(cwiseAdd, +)
    cwiseOperationScalar(cwiseSubtract, -)
    cwiseOperationScalar(cwiseProduct, *)
    cwiseOperationScalar(cwiseDivide, /)

    static Mat Identity() {
        Mat identity;
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                if (i == j) {
                    identity[j][i] = 1;
                }
            }
        }
        return identity;
    }

protected:
    std::vector<std::vector<T>> _container;
};

template<unsigned int row, unsigned int col, typename T>
std::ostream& operator<<(std::ostream &os, const Mat<row, col, T>& mat) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            os << mat[j][i] << " ";
        }
        os << std::endl;
    }
    return os;
}

typedef Mat<4, 4, float> Mat4f;


#endif //CRENDERER_MAT_HPP
