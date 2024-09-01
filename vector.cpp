#include "vector.h"
#include <cmath>

// =============================================================
// 3D VECTOR FUNCTIONS (for points and simple direction vectors)
// =============================================================
float dot_product(const Vector3D& v1, const Vector3D& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3D cross_product(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D v{};
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

Vector3D operator+(const Vector3D &v1, const Vector3D &v2)
{
	return {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z
	};
}

Vector3D operator-(const Vector3D &v1, const Vector3D &v2)
{
	return {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z
	};
}

Vector3D normalize(const Vector3D& vec)
{
	float magn = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	return { vec.x/magn, vec.y/magn, vec.z/magn };
}

// ============================================================================
// 4D VECTOR FUNCTIONS (for math related to the projections)
// ============================================================================

// 4D vectors are used as 3D vectors are homogonized so that affine 
// transformations can be performed using basic matrix multiplication 
// as opposed to using vector addition for shifts of vectors.

Vector4D homogenize_vec3d(const Vector3D& vec)
{
	return { vec.x, vec.y, vec.z, 1.0f };
}

Vector3D dehomogenize_vec4d(const Vector4D& vec)
{
	// vec is a vector and not a point in this case
	if (vec.w == 0)
		return { vec.x, vec.y, vec.z };
	return { vec.x / vec.w, vec.y / vec.w, vec.z / vec.w };
}

float dot_product(const Vector4D &vec1, const Vector4D &vec2)
{
	return vec1.x * vec2.x +
			vec1.y * vec2.y +
			vec1.z * vec2.z +
			vec1.w * vec2.w;
}

Vector4D matrix_mult(const Matrix4 &mat, const Vector4D &vec)
{
	return {
		dot_product(mat[0], vec),	
		dot_product(mat[1], vec),	
		dot_product(mat[2], vec),	
		dot_product(mat[3], vec)
	};
}
