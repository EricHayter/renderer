#include "renderer.h"
#include "vector3d.h"
#include "model.h"
#include <algorithm>
#include <functional>
#include <limits>

constexpr Vector3D light_dir{ 0, 0, -1 };

Vertex scale_vertex(const Vertex &v)
{
	return { 	(v.x + 1.0f) * SCREEN_WIDTH / 2, 
				(-v.y + 1.0f) * SCREEN_HEIGHT / 2,
				v.z};
}

void draw_point(Context& context, Point2D p)
{
	SDL_RenderDrawPoint(context.sdl_renderer, p.x, p.y);
}

void set_color(Context &context, Color clr)
{
    SDL_SetRenderDrawColor(context.sdl_renderer, 
            clr.r,
            clr.g,
            clr.b,
            clr.a);
}

// this is just a POC I'll be editting the values of the vertices in this
// function (will edit to remove this feature later)
void draw_model(Context &context, const Model &model)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<FaceTuple> face = model.face(i);
		Vertex v1{ model.vertex(face[0].vertex - 1) };

		// Triangle fanning
		for (int j = 2; j < face.size(); j++) { // outer loop for start points
			Vertex v2{ model.vertex(face[j-1].vertex - 1) };
			Vertex v3{ model.vertex(face[j].vertex - 1) };

			// find the normal of vectors from v1 to v2 and v1 to v3
			Vector3D normal{ normalize(cross_product(v3 - v1, v2 - v1)) };

			// intensity of light reflected will be equal to dot product of 
			// view vector and normal of face
			float intensity{ dot_product(normal, light_dir) };
			Vertex v1n{ scale_vertex(v1) };
			Vertex v2n{ scale_vertex(v2) };
			Vertex v3n{ scale_vertex(v3) };
			if (intensity > 0)
				draw_face(context, v1n, v2n, v3n, 
						{(uint8_t)(255 * intensity), 
						 (uint8_t)(255 * intensity), 
						 (uint8_t)(255 * intensity),
						 255}
						 );
		}
	}
}

void draw_face(Context &context, Vertex v1, Vertex v2, Vertex v3, Color color)
{
	set_color(context, color);
	if (v1.y < v2.y)
		std::swap(v1, v2);
	if (v2.y < v3.y)
		std::swap(v2, v3);
	if (v1.y < v2.y)
		std::swap(v1, v2);

	if (v3.y - v1.y == 0)
		return;

	// parametrization of a line from p1 to p3
	float t{ (v2.y - v3.y)/(v3.y - v1.y) };	
	float x{ (v3.x - v1.x)*t + v3.x };
	float y{ v2.y };
	float z{ (v3.z - v1.z)*t + v3.z };

	Vertex vt{ x, y, z};
	draw_face_upper(context, v1, v2, vt, color);
	draw_face_lower(context, vt, v2, v3, color);
}

void draw_face_upper(Context &context, Vertex v1, Vertex v2, Vertex v3, Color color)
{
	if (v1.y == v2.y) { // this triangle is pointing down
		return;
	}
	
	if (v2.x > v3.x) // put v2 on the left and v3 on the right
		std::swap(v2, v3);

	std::function<float (const Point2D&)> solFunc{ findPlaneSolution(v1, v2, v3) };

	float s2{ static_cast<float>(v1.x - v2.x)/(v1.y - v2.y)};
	float s3{ static_cast<float>(v1.x - v3.x)/(v1.y - v3.y)};
	float pointer2{ (float)v2.x - s2 };
	float pointer3{ (float)v3.x - s3 };

	for (int y = v2.y; y <= v1.y; y++) {
		pointer2 += s2;
		pointer3 += s3;
		for (int x = pointer2; x <= pointer3; x++) {
			float z{ solFunc({x, y}) };
			if (z >= context.zbuffer[x][y]) {
				draw_point(context, {x, y});	
				context.zbuffer[x][y] = z;
			}
		}
	}
}

void draw_face_lower(Context &context, Vertex v1, Vertex v2, Vertex v3, Color color)
{
	if (v2.y == v3.y) {
		return;
	}

	if (v1.x > v2.x) // make sure that v1 is on the left
		std::swap(v1, v2);

	std::function<float (const Point2D&)> solFunc{ findPlaneSolution(v1, v2, v3) };

	float sl{ ((float)v3.x - v1.x)/(v3.y - v1.y)};
	float sr{ ((float)v3.x - v2.x)/(v3.y - v2.y)};
	float pointerl{ (float)v3.x - sl };
	float pointerr{ (float)v3.x - sr};
	for (int y = v3.y; y <= v2.y; y++) {
		pointerl += sl;
		pointerr += sr;	
		for (int x = pointerl; x <= pointerr; x++) { 
			float z{ solFunc({x, y}) };
			if (z >= context.zbuffer[x][y]) {
				draw_point(context, {x, y});	
				context.zbuffer[x][y] = z;
			}
		}
	}
}

// given a point (X, Y) and 3 points to define a plane what is the solution for (X, Y)
// in the plane?
std::function<float (const Point2D&)> findPlaneSolution(const Vertex &v1, const Vertex &v2, const Vertex &v3)
{
	Vector3D normal{ cross_product(v3 - v1, v2 - v1) };
	if (normal.z == 0)
		return [](const Point2D& p){ 
			return std::numeric_limits<float>::min();  // normal has no z component
		};
	float a{ normal.x };
	float b{ normal.y };
	float c{ normal.z };
	float x0{ v1.x };
	float y0{ v1.y };
	float z0{ v1.z };
	return [a, b, c, x0, y0, z0](const Point2D& p){ 
		return (-a * (p.x - x0) - b * (p.y - y0))/c + z0;
	};
}
