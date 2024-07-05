#include "renderer.h"
#include "model.h"
#include <algorithm>
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

Vector3D normalize(const Vector3D& vec)
{
	float d = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	return { vec.x/d, vec.y/d, vec.z/d };
}

Point2D project_vertex(const Vertex &v)
{
	return { (int)((v.x + 1.0) * SCREEN_WIDTH / 2), 
		(int)((1. - v.y) * SCREEN_HEIGHT / 2) };
}

void draw_point(SDL_Renderer *renderer, Point2D p)
{
	SDL_RenderDrawPoint(renderer, p.x, p.y);
}

void set_color(SDL_Renderer *renderer, Color clr)
{
    SDL_SetRenderDrawColor(renderer, 
            clr.r,
            clr.g,
            clr.b,
            clr.a);
}

// this is just a POC I'll be editting the values of the vertices in this
// function (will edit to remove this feature later)
void draw_model(const Model &model, SDL_Renderer *renderer)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<FaceTuple> face = model.face(i);

		for (int j = 0; j < face.size(); j++) { // outer loop for start points
			Point2D start = project_vertex(model.vertex(face[j].vertex - 1)); // indexes start at 1
			for (int k = j + 1; k < face.size(); k++) {
				Point2D stop = project_vertex(model.vertex(face[k].vertex - 1));
				draw_line({start, stop}, renderer, {255, 255, 255});
			}
		}
	}
}

void draw_triangle(Point2D p1, Point2D p2, Point2D p3, SDL_Renderer *renderer, Color color)
{
	if (p1.y < p2.y)
		std::swap(p1, p2);
	if (p2.y < p3.y)
		std::swap(p2, p3);
	if (p1.y < p2.y)
		std::swap(p1, p2);

	// slope from p1 to p3	
	Point2D vt {};
	float m{((float) p1.y - p3.y)/(p1.x - p3.x)};
	if (m == 0)
		vt = p3; // what should this actually be?
	else
		vt = {(int)((p2.y - p1.y)/m) + p1.x, p2.y};
	draw_triangle_upper(p1, p2, vt, renderer, color);
	draw_triangle_lower(vt, p2, p3, renderer, color);
}

void draw_triangle_upper(Point2D p1, Point2D p2, Point2D p3, SDL_Renderer *renderer, Color color)
{
	if (p1.y == p2.y) { // this triangle is pointing down
		return;
	}
	
	if (p2.x > p3.x)
		std::swap(p2, p3);

	float s2{ static_cast<float>(p1.x - p2.x)/(p1.y - p2.y)};
	float s3{ static_cast<float>(p1.x - p3.x)/(p1.y - p3.y)};
	float pointer2{ (float)p2.x }, pointer3{ (float)p3.x };

	for (int y = p2.y; y <= p1.y; y++) {
		pointer2 += s2;
		pointer3 += s3;
		for (int x = pointer2; x <= pointer3; x++) {
			draw_point(renderer, {x, y});	
		}
	}
}
void draw_triangle_lower(Point2D p1, Point2D p2, Point2D p3, SDL_Renderer *renderer, Color color) {
	if (p2.y == p3.y) {
		return;
	}

	if (p1.x > p2.x) // make sure that p1 is on the left
		std::swap(p1, p2);

	float sl{ ((float)p3.x - p1.x)/(p3.y - p1.y)};
	float sr{ ((float)p3.x - p2.x)/(p3.y - p2.y)};
	float pointerl{ (float)p3.x }, pointerr{ (float)p3.x };
	for (int y = p3.y; y <= p2.y; y++) {
		pointerl += sl;
		pointerr += sr;	
		for (int x = pointerl; x <= pointerr; x++) { // dependent on x values!!!
			draw_point(renderer, {x, y});	
		}
	}
}

void draw_line(Line l, SDL_Renderer *renderer, Color color)
{
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
            draw_point(renderer, {y, x});
        } else {
			draw_point(renderer, {x, y});
        }
    }
}
