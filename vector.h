#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cstddef>
#define X 0
#define Y 1
#define Z 2
#define W 3

#include <array>

template<size_t rows, size_t cols>
struct Matrix {
	std::array<std::array<float, cols>, rows> m;

	template<size_t cols2>
	Matrix<rows, cols2> operator*(const Matrix<cols, cols2>& m);	
	std::array<float, cols> operator[](size_t i);
};

template<size_t len>
struct Vector : Matrix<len, 1> {
	Vector<len> operator+(const Vector<len> &v);
	Vector<len> operator-(const Vector<len> &v);
	const float& operator[](size_t i) const;
	float& operator[](size_t i);
	Vector<len> normalize();
	Vector<len + 1> homogenize();
	Vector<len - 1> dehomogenize();
};

template<size_t len>
float dot_product(const Vector<len> &v1, const Vector<len> &v2);
Vector<3> cross_product(const Vector<3> &v1, const Vector<3> &v2);

#endif
