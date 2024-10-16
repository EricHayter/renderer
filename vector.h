#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <cstddef>
#include <cmath>
#define X 0
#define Y 1
#define Z 2
#define W 3

#include <array>
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

template<size_t len>
struct Vector; // Forward declaration

template<size_t rows, size_t cols>
struct Matrix {
	// just a wrapped for an array
	std::array<std::array<float, cols>, rows> m;

	Matrix() : m{} {}

	Matrix(std::initializer_list<std::initializer_list<float>> v) 
		: m{}
   	{
		// do some size checking here to ensure gridlike initalizer list
		if (v.size() != rows)
			throw std::runtime_error("Initializer list row count mismatch.");
        auto row_iter = v.begin();
        for (size_t i = 0; i < rows; ++i) {
            if (row_iter->size() != cols) {
                throw std::runtime_error("Initializer list column count mismatch.");
            }
            std::copy(row_iter->begin(), row_iter->end(), m[i].begin());
            ++row_iter;
        }
	}

	operator Vector<rows>() const
    {
        if (cols != 1) {
            throw std::runtime_error("Cannot convert Matrix to Vector: Matrix must have exactly one column.");
        }
        Vector<rows> v{};
        for (size_t i = 0; i < rows; ++i) {
            v[i] = m[i][0];
        }
        return v;
    }

	// Matrix multiplication
	template<size_t k>
	Matrix<rows, k> operator*(const Matrix<cols, k> &m) const
	{
		Matrix<rows, k> product{};	
		for (size_t row = 0; row < rows; row++) {
			for (size_t col = 0; col < k; col++) {
				for (size_t h = 0; h < cols; h++)
					product[row][col] += (*this)[row][h] * m[h][col];
			}
		}
		return product;
	}

	const std::array<float, cols>& operator[](size_t i) const
	{
		if (i >= rows)
			throw std::out_of_range("Matrix index out of range.");
		return m[i];
	}

	std::array<float, cols>& operator[](size_t i)
	{
		if (i >= rows)
			throw std::out_of_range("Matrix index out of range.");
		return m[i];
	}

	// return the transpose of a matrix
	Matrix<cols, rows> transpose()
	{
		Matrix<cols, rows> nm{};	
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
				nm[j][i] = (*this)[i][j];
		return nm;
	}
};

// using laplace expansion for this
// I think the recursive call will be heavy on memory
// maybe look to find a better solution although it's good 
// enough for 4 x 4 matricies

float determinant(const Matrix<1, 1> &m);

template<size_t n>
float determinant(const Matrix<n, n> &m)
{
	float det{ 0.f };
	for (size_t i = 0; i < n; i++) {
		// get the sub-matricies for the recursive calls
		float coefficient{ i % 2 == 0 ? m[0][i] : -m[0][i] };
		int skipped{ 0 }; // have I skipped over the column of the coefficient?

		Matrix<n-1, n-1> sm{};
		for (size_t col = 0; col < n; col++) {
			if (col == i) {
				skipped = 1;
				continue;
			}
			for (size_t row = 1; row < n; row++) {
						sm[row-1][col - skipped] = m[row][col];
			}
		}

		// add coefficient times determinant of submatrix to determinant
		det += coefficient * determinant(sm);
	}
	return det;
}

template<size_t n>
Matrix<n, n> cofactor(const Matrix<n, n> &m)
{
	Matrix<n, n> rm{};
	for (size_t i = 0; i < n; i++) { 		// row
		for (size_t j = 0; j < n; j++) {	// col
			int sign{ (i + j) % 2 == 0 ? 1 : -1 };
			// submatrix to find the determinant of
			Matrix<n-1, n-1> sm{};
				
			int skipped_row{ 0 };			
			for (size_t row = 0; row < n; row++) {
				int skipped_col{ 0 };			
				if (row == i) {
					skipped_row = 1;	
					continue;
				}
				for (size_t col = 0; col < n; col++) {
					if (col == j) {
						skipped_col = 1;	
						continue;
					}
					sm[row - skipped_row][col - skipped_col] = m[row][col];
				}
			}

			// submatrix is created find the determinant
			rm[i][j] = sign*determinant(sm);
		}
	}
	return rm;
}

template<size_t n>
Matrix<n, n> inverse(const Matrix<n, n> &m)
{
	return 1/determinant(m)*cofactor(m).transpose();	
}

template <size_t rows, size_t cols>
std::ostream& operator<<(std::ostream& out, const Matrix<rows, cols> &m)
{
	for (size_t row = 0; row < rows; row++) {
		for (size_t col = 0; col < cols; col++) {
			out << m[row][col] << " ";	
		}
		out << "\n";
	}
	return out;
}

template<size_t len>
struct Vector : public Matrix<len, 1> {
	Vector() : Matrix<len, 1>{} {};
	Vector(const Matrix<len, 1> &m) : Matrix<len, 1>{m} {};
	Vector(std::initializer_list<float> v) 
		: Matrix<len, 1>{} {
			if (v.size() != len)
				throw "Invalid number of arguments!";
			for (size_t i = 0; i < v.size(); i++)
				this->m[i][0] = v.begin()[i];	
		};

	// Arithmetic operator overloading
	Vector<len> operator+(const Vector<len> &v) const
	{
		Vector<len> nv{};
		for (size_t i = 0; i < len; i++)
			nv[i] = (*this)[i] + v[i];
		return nv;
	}

	Vector<len> operator-(const Vector<len> &v) const
	{
		Vector<len> nv{};
		for (size_t i = 0; i < len; i++)
			nv[i] = (*this)[i] - v[i];
		return nv;
	}


	// Subscript operator overloading
	const float& operator[](size_t i) const
	{
		return this->m[i][0];
	}

	float& operator[](size_t i)
	{
		return this->m[i][0];
	}

	Vector<len> normalize() const
	{
		float magn{};
		Vector<len> nv{};
		for (size_t i = 0; i < len; i++) {
			nv[i] = (*this)[i];
			magn += (*this)[i] * (*this)[i];
		}
		if (magn == 0)
			return *this;
		magn = sqrt(magn);
		for (size_t i = 0; i < len; i++)
			nv[i] /= magn;
		return nv;
	}

	Vector<(len + 1)> homogenize() const
	{
		Vector<len + 1> nv{};
		for (size_t i = 0; i < len; i++)
			nv[i] = (*this)[i];
		nv[len] = 1.f;
		return nv;
	}

	Vector<(len - 1)> dehomogenize() const
	{
		if (len == 1)
			throw "Cannot dehomogenize single-length vector!";

		// copy len - 1 values into new point
		Vector<len - 1> nv{};
		for (size_t i = 0; i < len - 1; i++)
			nv[i] = (*this)[i];

		// check if it is a "vector" and not a point
		if ((*this)[len - 1] == 0) // no need to divide if it's a vector and not a point
			return nv;

		// normalize remaining coordinates
		for (size_t i = 0; i < len - 1; i++) // divide by added coordinate
			nv[i] /= (*this)[len - 1];	

		return nv;
	}
};

template <size_t row, size_t col>
Matrix<row, col> operator*(float scalar, const Matrix<row, col> &m)
{
	Matrix<row, col> nm{};
	for (size_t i = 0; i < row; i++) {
		for (size_t j = 0; j < col; j++) {
			nm[i][j] = scalar * m[i][j];	
		}
	}
	return nm;
}

template <size_t len>
Vector<len> operator*(float scalar, const Vector<len> &v)
{
	Vector<len> nv{};
	for (size_t i = 0; i < len; i++)
		nv[i] = scalar * v[i];	
	return nv;
}


template<size_t len>
float dot_product(const Vector<len> &v1, const Vector<len> &v2)
{
	float p{};
	for (size_t i = 0; i < len; i++)
		p += v1[i] * v2[i];
	return p;
}

Vector<3> cross_product(const Vector<3> &v1, const Vector<3> &v2);

Vector<3> view_vector(float yaw, float pitch);

#endif
