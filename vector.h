#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <array>

template <int rows, int cols>
struct Matrix {
	std::array<std::array<float, cols>, rows> m;

	template<int orows, int ocols>
	Matrix operator* (const Matrix<orows, ocols>& m);	
};

template<int len>
struct Vector : Matrix<len, 1> {
	Vector operator+(const Vector &v);
	Vector operator-(const Vector &v);
	float dot_product(const Vector& v);
	Vector<3> cross_product(const Vector<3>& v);
	Vector normalize();
	Vector homogenize();
	Vector dehomogenize();
};

#endif
