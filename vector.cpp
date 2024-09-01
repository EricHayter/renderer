#include "vector.h"
#include <cmath>

// make it simpler for accessing indexes
// I couldn't get typedefs to work so the wrapper class is pretty baron
template<size_t rows, size_t cols>
std::array<float, cols> Matrix<rows, cols>::operator[](size_t i)
{
	if (i < 0 || i >= rows)
		throw "Illegal index!";
	return m[i];
}

// perform matrix multiplication with a matrix to the right
template<size_t i, size_t j>
template<size_t k>
Matrix<i, k> Matrix<i, j>::operator* (const Matrix<j, k> &m)
{
	Matrix<i, k> product;	
	for (size_t row = 0; row < i; row++) {
		for (size_t col = 0; col < k; col++) {
			product[row][col] = 0;
			for (size_t h = 0; h < j; h++)
				product[row][col] += this[row][h] * m[h][col];
		}
	}
	return product;
}

template<size_t len>
float& Vector<len>::operator[](size_t i)
{
	if (i < 0 || i >= len)
		throw "Illegal index!";
	return this[i][0];
}

template<size_t len>
const float& Vector<len>::operator[](size_t i) const
{
	if (i < 0 || i >= len)
		throw "Illegal index!";
	return this[i][0];
}

Vector<3> cross_product(const Vector<3> &v1, const Vector<3> &v2)
{
	Vector<3> nv{};
	nv[X] = v1[Y] * v2[Z] - v1[Z] * v2[Y];
	nv[Y] = v1[Z] * v2[X] - v1[X] * v2[Z];
	nv[Z] = v1[X] * v2[Y] - v1[Y] * v2[X];
	return nv;
}

template<size_t len>
Vector<len> operator+(const Vector<len> &v1, const Vector<len> &v2)
{
	Vector<len> nv{};
	for (size_t i = 0; i < len; i++)
		nv[i] = v1[i] + v2[i];
	return nv;
}

template<size_t len>
Vector<len> operator-(const Vector<len> &v1, const Vector<len> &v2)
{
	Vector<len> nv{};
	for (size_t i = 0; i < len; i++)
		nv[i] = v1[i] - v2[i];
	return nv;
}

template<size_t len>
Vector<len> normalize(const Vector<len> &vec)
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

template<size_t len>
Vector<len + 1> Vector<len>::homogenize()
{
	Vector<len + 1> nv{};
	for (size_t i = 0; i < len; i++)
		nv[i] = this[i];
	return nv;
}

template<size_t len>
Vector<len - 1> Vector<len>::dehomogenize()
{
	if (len == 1)
		throw "Cannot dehomogenize single-length vector!";
	Vector<len - 1> nv{};
	for (size_t i = 0; i < len - 1; i++)
		nv[i] = this[i];

	if (this[len - 1] == 0) // no need to divide if it's a vector and not a point
		return nv;

	for (size_t i = 0; i < len - 1; i++) // divide by added coordinate
		nv[i] /= this[len - 1];	
	return nv;
}

