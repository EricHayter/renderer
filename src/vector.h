#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <cmath>
#include <cstddef>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <type_traits>

constexpr int X = 0;
constexpr int Y = 1;
constexpr int Z = 2;
constexpr int W = 3;

template <int len>
struct Vector;  // Forward declaration

template <int rows, int cols>
struct Matrix {
    // just a wrapped for an array
    std::array<std::array<float, cols>, rows> m;

    Matrix() : m{} {}

    Matrix(std::initializer_list<std::initializer_list<float>> v) : m{} {
        // do some size checking here to ensure gridlike initalizer list
        if (v.size() != rows)
            throw std::runtime_error("Initializer list row count mismatch.");
        auto row_iter = v.begin();
        for (int i = 0; i < rows; ++i) {
            if (row_iter->size() != cols) {
                throw std::runtime_error(
                    "Initializer list column count mismatch.");
            }
            std::copy(row_iter->begin(), row_iter->end(), m[i].begin());
            ++row_iter;
        }
    }

    operator Vector<rows>() const {
        if (cols != 1) {
            throw std::runtime_error(
                "Cannot convert Matrix to Vector: Matrix must have exactly one "
                "column.");
        }
        Vector<rows> v{};
        for (int i = 0; i < rows; ++i) {
            v[i] = m[i][0];
        }
        return v;
    }

    // Matrix multiplication
    template <int k>
    Matrix<rows, k> operator*(const Matrix<cols, k>& m) const {
        Matrix<rows, k> product{};
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < k; col++) {
                for (int h = 0; h < cols; h++)
                    product[row][col] += (*this)[row][h] * m[h][col];
            }
        }
        return product;
    }

    const std::array<float, cols>& operator[](int i) const {
        if (i >= rows)
            throw std::out_of_range("Matrix index out of range.");
        return m[i];
    }

    std::array<float, cols>& operator[](int i) {
        if (i >= rows)
            throw std::out_of_range("Matrix index out of range.");
        return m[i];
    }

    // return the transpose of a matrix
    Matrix<cols, rows> transpose() {
        Matrix<cols, rows> nm{};
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                nm[j][i] = (*this)[i][j];
        return nm;
    }
};

// using laplace expansion for this
// I think the recursive call will be heavy on memory
// maybe look to find a better solution although it's good
// enough for 4 x 4 matricies

float determinant(const Matrix<1, 1>& m);

template <int n>
float determinant(const Matrix<n, n>& m) {
    float det{0.f};
    for (int i = 0; i < n; i++) {
        // get the sub-matricies for the recursive calls
        float coefficient{i % 2 == 0 ? m[0][i] : -m[0][i]};
        int skipped{0};  // have I skipped over the column of the coefficient?

        Matrix<n - 1, n - 1> sm{};
        for (int col = 0; col < n; col++) {
            if (col == i) {
                skipped = 1;
                continue;
            }
            for (int row = 1; row < n; row++) {
                sm[row - 1][col - skipped] = m[row][col];
            }
        }

        // add coefficient times determinant of submatrix to determinant
        det += coefficient * determinant(sm);
    }
    return det;
}

template <int n>
Matrix<n, n> cofactor(const Matrix<n, n>& m) {
    Matrix<n, n> rm{};
    for (int i = 0; i < n; i++) {      // row
        for (int j = 0; j < n; j++) {  // col
            int sign{(i + j) % 2 == 0 ? 1 : -1};
            // submatrix to find the determinant of
            Matrix<n - 1, n - 1> sm{};

            int skipped_row{0};
            for (int row = 0; row < n; row++) {
                int skipped_col{0};
                if (row == i) {
                    skipped_row = 1;
                    continue;
                }
                for (int col = 0; col < n; col++) {
                    if (col == j) {
                        skipped_col = 1;
                        continue;
                    }
                    sm[row - skipped_row][col - skipped_col] = m[row][col];
                }
            }

            // submatrix is created find the determinant
            rm[i][j] = sign * determinant(sm);
        }
    }
    return rm;
}

template <int n>
Matrix<n, n> inverse(const Matrix<n, n>& m) {
    return 1 / determinant(m) * cofactor(m).transpose();
}

template <int rows, int cols>
std::ostream& operator<<(std::ostream& out, const Matrix<rows, cols>& m) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            out << m[row][col] << " ";
        }
        out << "\n";
    }
    return out;
}

template <int len>
class Vector {
   public:
    Vector() = default;

    Vector(std::array<float, len> arr) { data = arr; }

    // Constructor that takes an initializer list
    Vector(std::initializer_list<float> args) {
        if (args.size() != len) {
            throw std::invalid_argument(
                "Number of arguments must match the size of the vector");
        }

        // Initialize the array from the initializer list
        std::copy(args.begin(), args.end(), data.begin());
    }

    // Arithmetic operator overloading
    Vector<len> operator+(const Vector<len>& v) const {
        Vector<len> nv{};
        for (int i = 0; i < len; i++)
            nv[i] = (*this)[i] + v[i];
        return nv;
    }

    Vector<len> operator-(const Vector<len>& v) const {
        Vector<len> nv{};
        for (int i = 0; i < len; i++)
            nv[i] = (*this)[i] - v[i];
        return nv;
    }

    // Subscript operator overloading
    const float& operator[](int i) const { return data[i]; }
    float& operator[](int i) { return data[i]; }

    Vector<len> normalize() const {
        float magn{};
        Vector<len> nv{};
        for (int i = 0; i < len; i++) {
            nv[i] = (*this)[i];
            magn += (*this)[i] * (*this)[i];
        }
        if (magn == 0)
            return *this;
        magn = sqrt(magn);
        for (int i = 0; i < len; i++)
            nv[i] /= magn;
        return nv;
    }

    Vector<(len + 1)> homogenize() const {
        Vector<len + 1> nv{};
        for (int i = 0; i < len; i++)
            nv[i] = (*this)[i];
        nv[len] = 1.f;
        return nv;
    }

    Vector<(len - 1)> dehomogenize() const {
        if (len == 1)
            throw "Cannot dehomogenize single-length vector!";

        // copy len - 1 values into new point
        Vector<len - 1> nv{};
        for (int i = 0; i < len - 1; i++)
            nv[i] = (*this)[i];

        // check if it is a "vector" and not a point
        if ((*this)[len - 1] == 0)
            return nv;

        // normalize remaining coordinates
        for (int i = 0; i < len - 1; i++)  // divide by added coordinate
            nv[i] /= (*this)[len - 1];

        return nv;
    }

   private:
    std::array<float, len> data{};
};

template <int row, int col>
Matrix<row, col> operator*(float scalar, const Matrix<row, col>& m) {
    Matrix<row, col> nm{};
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            nm[i][j] = scalar * m[i][j];
        }
    }
    return nm;
}

template <int len>
Vector<len> operator*(float scalar, const Vector<len>& v) {
    Vector<len> nv{};
    for (int i = 0; i < len; i++)
        nv[i] = scalar * v[i];
    return nv;
}

template <int rows, int cols>
Vector<rows> operator*(const Matrix<rows, cols>& m, const Vector<cols>& v) {
    Vector<rows> product;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            product[row] += m[row][col] * v[col];
        }
    }
    return product;
}

template <int len>
float dot_product(const Vector<len>& v1, const Vector<len>& v2) {
    float p{};
    for (int i = 0; i < len; i++)
        p += v1[i] * v2[i];
    return p;
}

Vector<3> cross_product(const Vector<3>& v1, const Vector<3>& v2);

Vector<3> view_vector(float yaw, float pitch);

#endif
