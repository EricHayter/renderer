#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <array>

typedef struct Vector3D_t {
    float x;
    float y;
    float z;
} Vector3D;

typedef struct Vector4D_t {
	float x;
	float y;
	float z;
	float w;
} Vector4D;

// 4 x 4 matrix
typedef std::array<Vector4D_t, 4> Matrix4;

Vector3D operator+(const Vector3D &v1, const Vector3D &v2);
Vector3D operator-(const Vector3D &v1, const Vector3D &v2);
Vector3D normalize(const Vector3D& vec);
float dot_product(const Vector3D& v1, const Vector3D& v2);
Vector3D cross_product(const Vector3D& v1, const Vector3D& v2);

Vector4D homogenize_vec3d(const Vector3D& vec);
Vector3D dehomogenize_vec4d(const Vector4D& vec);
float dot_product(const Vector4D &vec1, const Vector4D &vec2);
Vector4D matrix_mult(const Matrix4 &mat, const Vector4D &vec);

#endif
