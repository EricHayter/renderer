#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>

typedef struct Vector3D_t {
    float x;
    float y;
    float z;
} Vector3D;

Vector3D operator+(const Vector3D &v1, const Vector3D &v2);
Vector3D operator-(const Vector3D &v1, const Vector3D &v2);
Vector3D normalize(const Vector3D& vec);
float dot_product(const Vector3D& v1, const Vector3D& v2);
Vector3D cross_product(const Vector3D& v1, const Vector3D& v2);

#endif
