#include "renderer.h"
#include "vector3d.h"
#include "model.h"
#include <algorithm>
#include <limits>

constexpr Vector3D light_dir{ 0, 0, -1 };

Point2D project_vertex(const Vertex &v)
{
	return { (int)((v.x + 1.0) * SCREEN_WIDTH / 2), 
		(int)((1. - v.y) * SCREEN_HEIGHT / 2) };
}

void draw_point(const Context& context, Point2D p)
{
	SDL_RenderDrawPoint(context.sdl_renderer, p.x, p.y);
}

void set_color(const Context &context, Color clr)
{
    SDL_SetRenderDrawColor(context.sdl_renderer, 
            clr.r,
            clr.g,
            clr.b,
            clr.a);
}

// this is just a POC I'll be editting the values of the vertices in this
// function (will edit to remove this feature later)
void draw_model(const Context &context, const Model &model)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<FaceTuple> face = model.face(i);
		Vertex v1{ model.vertex(face[0].vertex - 1) };
		Point2D p1{ project_vertex(v1) };

		// Triangle fanning
		for (int j = 2; j < face.size(); j++) { // outer loop for start points
			Vertex v2{ model.vertex(face[j-1].vertex - 1) };
			Point2D p2{ project_vertex(v2) };

			Vertex v3{ model.vertex(face[j].vertex - 1) };
			Point2D p3{ project_vertex(v3) };

			// find the normal of vectors from v1 to v2 and v1 to v3
			Vector3D normal{ normalize(cross_product(v3 - v1, v2 - v1)) };

			// intensity of light reflected will be equal to dot product of view vector and normal of face
			float intensity{ dot_product(normal, light_dir) };
			if (intensity > 0)
				draw_face(context, p1, p2, p3, {(uint8_t)(255 * intensity), (uint8_t)(255 * intensity), (uint8_t)(255 * intensity)});
		}
	}
}

void draw_face(const Context &context, Vertex v1, Vertex v2, Vertex v3, Color color)
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
	float t{ v2.y/((float) v3.y - v1.y) - v3.y };	
	float x{ (v3.x - v1.x)*t + v3.x };
	float y{ v2.y };
	float z{ (v3.z - v1.z)*t + v3.z };

	Vertex vt{ x, y, z};
	draw_face_upper(context, v1, v2, vt, color);
	draw_face_lower(context, vt, v2, v3, color);
}

void draw_triangle_upper(const Context &context, Vertex v1, Vertex v2, Vertex v3, Color color)
{
	if (v1.y == v2.y) { // this triangle is pointing down
		return;
	}
	
	if (v2.x > v3.x) // put v2 on the left and v3 on the right
		std::swap(v2, v3);

	float s2{ static_cast<float>(v1.x - v2.x)/(v1.y - v2.y)};
	float s3{ static_cast<float>(v1.x - v3.x)/(v1.y - v3.y)};
	float pointer2{ (float)v2.x - s2 };
	float pointer3{ (float)v3.x - s3 };

	for (int y = v2.y; y <= v1.y; y++) {
		pointer2 += s2;
		pointer3 += s3;
		for (int x = pointer2; x <= pointer3; x++) {
			draw_point(context, {x, y});	
		}
	}
}

void draw_triangle_lower(const Context &context, Vertex v1, Vertex v2, Vertex v3, Color color)
{
	if (v2.y == v3.y) {
		return;
	}

	if (v1.x > v2.x) // make sure that v1 is on the left
		std::swap(v1, v2);

	float sl{ ((float)v3.x - v1.x)/(v3.y - v1.y)};
	float sr{ ((float)v3.x - v2.x)/(v3.y - v2.y)};
	float pointerl{ (float)v3.x - sl };
	float pointerr{ (float)v3.x - sr};
	for (int y = v3.y; y <= v2.y; y++) {
		pointerl += sl;
		pointerr += sr;	
		for (int x = pointerl; x <= pointerr; x++) { 
			draw_point(context, {x, y});	
		}
	}
}

void draw_line(const Context& context, Line l, Color color)
{
	set_color(context, color);
	Point2D to{ l.to };
	Point2D from{ l.from };
    int y {};
    bool is_steep {false};
    if (std::abs(from.y - to.y) > std::abs(from.x - to.x)) { // transpose if we have a steep line
        std::swap(from.x, from.y);
        std::swap(to.x, to.y);
        is_steep = true;
    }

    if (to.x < from.x) { // make sure direction doesn´t effect render
        std::swap(to.x, from.x);
		std::swap(from.y, to.y);
    }

    for (int x = from.x; x <= to.x; x++) {
        float t {static_cast<float>(x - from.x)/(to.x - from.x)};
        y = from.y + (to.y - from.y) * t;
        if (is_steep) {
            draw_point(context, {y, x});
        } else {
			draw_point(context, {x, y});
        }
    }
}
