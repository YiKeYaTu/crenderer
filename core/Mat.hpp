//
// Created by lc on 2022/3/29.
//

#ifndef CRENDERER_MAT_HPP
#define CRENDERER_MAT_HPP

#include <iostream>
#include <cmath>
#include <array>

#define cwiseOperationMatMember(operatorName, op) \
Mat operatorName(const Mat& rhs) const { \
    Mat retMat; \
    for (int i = 0; i < row; ++i) { \
        for (int j = 0; j < col; ++j) { \
            retMat[j][i] = _container[j][i] op rhs[j][i]; \
        } \
    } \
    return retMat; \
}

#define cwiseOperationScalarMember(operatorName, op) \
Mat operatorName(const T& rhs) const { \
    Mat retMat; \
    for (int i = 0; i < row; ++i) { \
        for (int j = 0; j < col; ++j) { \
            retMat[j][i] = _container[j][i] op rhs; \
        } \
    } \
    return retMat; \
}

#define cwiseOperationScalar(operatorName, op) \
template <unsigned int row, unsigned int col, typename T> \
Mat<row, col, T> operatorName(const T& lhs, Mat<row, col, T>& rhs) { \
    Mat<row, col, T> retMat; \
    for (int i = 0; i < row; ++i) { \
        for (int j = 0; j < col; ++j) { \
            retMat[j][i] = rhs[j][i] op rhs; \
        } \
    } \
    return retMat; \
}

template <unsigned int row, unsigned int col, typename T>
class Mat {
    static_assert(std::is_floating_point_v<T> == true);
    static_assert(row > 0 && col > 0);

    typedef std::size_t sizeType;

public:
    constexpr const static float EPSILON = 0.000001;

    Mat(): _container() {}
    Mat(const std::array<std::array<T, row>, col>& mat): _container(mat) {}
    Mat(const std::initializer_list<std::initializer_list<T>>& mat) {
        assert(mat.size() == col && mat.begin()->size() == row);
        unsigned int counter = 0;
        for (auto& item : mat) {
            std::copy(item.begin(), item.end(), _container[counter++].begin());
        }
    }
    template<unsigned int row2, unsigned int col2>
    Mat(const Mat<row2, col2, T>& rhs, const T& constant): Mat() {
        static_assert((row2 < row && col2 <= col) || (row2 <= row && col2 < col));
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                if (i < row2 && j < col2) {
                    _container[j][i] = rhs[j][i];
                } else {
                    _container[j][i] = constant;
                }
            }
        }
    }

    template<unsigned int row2, unsigned int col2>
    Mat(const Mat<row2, col2, T>& rhs): Mat() {
        using std::min;
        for (int i = 0; i < min<unsigned int>(row, row2); ++i) {
            for (int j = 0; j < min<unsigned int>(col, col2); ++j) {
                if (i < row2 && j < col2) {
                    _container[j][i] = rhs[j][i];
                }
            }
        }
    }

    inline const std::array<T, row>& operator[](const int i) const { return _container[i]; }
    inline std::array<T, row>& operator[](const int i) { return _container[i]; }

    template<unsigned int row2, unsigned int col2>
    Mat<row, col2, T> operator*(const Mat<row2, col2, T>& rhs) const {
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

    Mat operator-() const {
        Mat negMat;
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                negMat[j][i] = -_container[j][i];
            }
        }
        return negMat;
    }

    const std::array<std::array<T, row>, col> container() const { return _container; }

    Mat<col, row, T> transpose() const {
        Mat<col, row, T> transposedMat;
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                transposedMat[i][j] = _container[j][i];
            }
        }
        return transposedMat;
    }

    const T* data() const {
        return &_container[0][0];
    }

    cwiseOperationMatMember(operator+, +)
    cwiseOperationMatMember(operator-, -)
    cwiseOperationMatMember(cwiseProduct, *)
    cwiseOperationMatMember(operator/, /)
    cwiseOperationScalarMember(operator+, +)
    cwiseOperationScalarMember(operator-, -)
    cwiseOperationScalarMember(cwiseProduct, *)
    cwiseOperationScalarMember(operator/, /)

    static Mat Identity() {
        static_assert(col == row);
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
    std::array<std::array<T, row>, col> _container;
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

cwiseOperationScalar(operator+, +)
cwiseOperationScalar(operator-, -)
cwiseOperationScalar(operator*, *)
cwiseOperationScalar(operator/, /)

template <typename T>
class Mat4: public Mat<4, 4, T> {
public:
    using Mat<4, 4, float>::Mat;
    Mat4(const Mat<4, 4, T>& mat): Mat<4, 4, T>(mat) {}

    static Mat<4, 4, T> Rotation(float radian, const Mat<3, 1, T>& axis) {
        T c = cos(radian), s = sin(radian);
        T kz = axis[0][2], ky = axis[0][1], kx = axis[0][0];

        Mat<3, 3, T> rotation = Mat<3, 3, T>::Identity().cwiseProduct(c)
                                + axis * axis.transpose().cwiseProduct(1 - c)
                                + Mat<3, 3, T>({ { 0, kz, -ky }, { -kz, 0, kx }, { ky, -kx, 0 } }).cwiseProduct(s);
        Mat<4, 4, T> ret(rotation);
        ret[3][3] = 1;

        return ret;
    }

    static Mat<4, 4, T> Flip(const Mat<3, 1, T>& axis) {
        Mat<4, 4, T> ret(Mat<4, 4, T>::Identity());
        for (int i = 0; i < 3; ++i) {
            if (axis[0][i] > 0) {
                ret[i][i] = -ret[i][i];
            }
        }
        return ret;
    }

    static Mat<4, 4, T> Translation(const Mat<3, 1, T>& translations) {
        Mat<4, 4, T> ret(Mat<4, 4, T>::Identity());
        for (int i = 0; i < 3; ++i) {
            ret[3][i] = translations[0][i];
        }
        return ret;
    }

    static Mat<4, 4, T> Scale(const Mat<3, 1, T>& scales) {
        Mat<4, 4, T> ret(Mat<4, 4, T>::Identity());
        for (int i = 0; i < 3; ++i) {
            ret[i][i] *= scales[0][i];
        }
        return ret;
    }

    static Mat<4, 4, T> Perspective(float zn, float zf, float fov, float aspectRatio) {
        float t = tan(fov / 2), a = aspectRatio;

        Mat<4, 4, T> perspective;
        perspective[0][0] = 1 / (t * a);
        perspective[1][1] = 1 / t;
        perspective[2][2] = - (zn + zf) / (zn - zf);
        perspective[2][3] = 1;
        perspective[3][2] = (2 * zn * zf) / (zn - zf);

        return perspective;
    }

    static Mat<4, 4, T> LookAt(const Mat<3, 1, T>&, const Mat<3, 1, T>&, const Mat<3, 1, T>&);
    static Mat<4, 4, T> LookAt(const Mat<3, 1, T>&, const Mat<3, 1, T>&, const Mat<3, 1, T>&, const Mat<3, 1, T>&);
};

typedef Mat4<float> Mat4f;
typedef Mat4<double> Mat4d;


#endif //CRENDERER_MAT_HPP
