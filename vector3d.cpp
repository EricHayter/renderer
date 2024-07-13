#include "vector3d.h"
#include <cmath>

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
	float d = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	return { vec.x/d, vec.y/d, vec.z/d };
}

