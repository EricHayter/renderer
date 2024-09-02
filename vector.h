#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <cstddef>
#include <cmath>
#define X 0
#define Y 1
#define Z 2
#define W 3

#include <array>

template<size_t rows, size_t cols>
struct Matrix {
	// just a wrapped for an array
	std::array<std::array<float, cols>, rows> m;

	Matrix() : m{}
	{
		static_assert(rows > 0 && cols > 0, "row and col count must be positive!");
	}

	Matrix(const std::array<std::array<float, cols>, rows> &m) : m{m}
   	{
		static_assert(rows > 0 && cols > 0, "row and col count must be positive!");
	};

	// Matrix multiplication
	template<size_t k>
	Matrix<rows, k> operator* (const Matrix<cols, k> &m)
	{
		Matrix<rows, k> product;	
		for (size_t row = 0; row < rows; row++) {
			for (size_t col = 0; col < k; col++) {
				product[row][col] = 0;
				for (size_t h = 0; h < cols; h++)
					product[row][col] += this[row][h] * m[h][col];
			}
		}
		return product;
	}

	const std::array<float, cols> operator[](size_t i) const
	{
		if (i < 0 || i >= rows)
			throw "Illegal index!";
		return m[i];
	}

	std::array<float, cols> operator[](size_t i)
	{
		if (i < 0 || i >= rows)
			throw "Illegal index!";
		return m[i];
	}
};

template<size_t len>
struct Vector : public Matrix<len, 1> {
	// Constructors
	Vector<len>() : Matrix<len, 1>{} {};
	Vector<len>(const std::array<float, len> &v) : Matrix<len, 1>{{v}} {};

	// Arithmetic operator overloading
	Vector<len> operator+(const Vector<len> &v) const
	{
		Vector<len> nv{};
		for (size_t i = 0; i < len; i++)
			nv[i] = this[i] + v[i];
		return nv;
	}

	Vector<len> operator-(const Vector<len> &v) const
	{
		Vector<len> nv{};
		for (size_t i = 0; i < len; i++)
			nv[i] = this[i] - v[i];
		return nv;
	}

	// Subscript operator overloading
	const float& operator[](size_t i) const
	{
		if (i < 0 || i >= len)
			throw "Illegal index!";
		return this[i][0];
	}

	float& operator[](size_t i)
	{
		if (i < 0 || i >= len)
			throw "Illegal index!";
		return this[i][0];
	}

	Vector<len> normalize(const Vector<len> &vec) const
	{
		float magn{};
		Vector<len> nv{};
		for (size_t i = 0; i < len; i++) {
			nv[i] = vec[i];
			magn += vec[i] * vec[i];
		}
		if (magn == 0)
			throw "Cannot normalize zero vector!";
		magn = sqrt(magn);
		for (size_t i = 0; i < len; i++)
			nv[i] /= magn;
		return nv;
	}

	Vector<(len + 1)> homogenize() const
	{
		Vector<len + 1> nv{};
		for (size_t i = 0; i < len; i++)
			nv[i] = this[i];
		return nv;
	}

	Vector<(len - 1)> dehomogenize() const
	{
		if (len == 1)
			throw "Cannot dehomogenize single-length vector!";

		// copy len - 1 values into new point
		Vector<len - 1> nv{};
		for (size_t i = 0; i < len - 1; i++)
			nv[i] = this[i];

		// check if it is a "vector" and not a point
		if (this[len - 1] == 0) // no need to divide if it's a vector and not a point
			return nv;

		// normalize remaining coordinates
		for (size_t i = 0; i < len - 1; i++) // divide by added coordinate
			nv[i] /= this[len - 1];	

		return nv;
	}
};

template<size_t len>
float dot_product(const Vector<len> &v1, const Vector<len> &v2)
{
	float p{};
	for (size_t i = 0; i < len; i++)
		p += v1[i] * v2[i];
	return p;
}

Vector<3> cross_product(const Vector<3> &v1, const Vector<3> &v2);

#endif
