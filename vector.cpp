#include "vector.h"
#include <cmath>

Vector<3> cross_product(const Vector<3> &v1, const Vector<3> &v2)
{
	Vector<3> nv{};
	nv[X] = v1[Y] * v2[Z] - v1[Z] * v2[Y];
	nv[Y] = v1[Z] * v2[X] - v1[X] * v2[Z];
	nv[Z] = v1[X] * v2[Y] - v1[Y] * v2[X];
	return nv;
}

Vector<3> view_vector(float yaw, float pitch)
{
	//float x{ std::cos(yaw) };
	//float y{ std::sin(yaw) };
	//float z{ std::tan(pitch) };
	float x{ std::sin(yaw) };
	float y{ std::tan(pitch) };
	float z{ std::cos(yaw) };
	Vector<3> v{ x, y, z };
	return v.normalize();
}
