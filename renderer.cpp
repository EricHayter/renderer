#include "renderer.h"
#include "model.h"
#include <algorithm>

// this is just a POC I'll be editting the values of the vertices in this
// function (will edit to remove this feature later)
void draw_model(const Model &model, SDL_Renderer *renderer)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<FaceTuple> face = model.face(i);

		for (int j = 0; j < face.size(); j++) { // outer loop for start points
			Vertex start = model.vertex(face[j].vertex - 1); // indexes start at 1
			start.x = (start.x + 1.) * SCREEN_WIDTH / 2.;
			start.y = (1. - start.y) * SCREEN_HEIGHT / 2.;
			for (int k = j + 1; k < face.size(); k++) {
				Vertex stop = model.vertex(face[k].vertex - 1);
				stop.x = (stop.x + 1.) * SCREEN_WIDTH / 2.;
				stop.y = (1. - stop.y) * SCREEN_HEIGHT / 2.;
				draw_line({start, stop}, renderer, {255, 255, 255});
			}
		}
	}
}

void draw_triangle(Vertex v1, Vertex v2, Vertex v3, SDL_Renderer *renderer, Color color)
{
	if (v1.y < v2.y)
		std::swap(v1, v2);
	if (v2.y < v3.y)
		std::swap(v2, v3);
	if (v1.y < v3.y)
		std::swap(v1, v3);

	// make a check to ensure that v1 and 
	Vertex vt{ (v2.y - v1.y)*(v1.x - v3.x)/(v1.y - v3.y) + v1.x, v2.y};
	draw_triangle_upper(v1, v2, v3, renderer, color);
	draw_triangle_lower(v1, v2, v3, renderer, color);
}

void draw_triangle_upper(const Vertex &v1, const Vertex &v2, const Vertex &v3, SDL_Renderer *renderer, Color color)
{
	if (v1.y == v2.y) {
		draw_line({v1, v2}, renderer, color);	
		return;
	}
	float s2{ static_cast<float>(v1.x - v2.x)/(v1.y - v2.y)};
	float s3{ static_cast<float>(v1.x - v3.x)/(v1.y - v3.y)};
	for (int y = v2.y; y <= v1.y; y++) {
		draw_line(
			{{(int)(v2.x + y * s2), y}, {(int)(v3.x + y * s3), y}},
			renderer,
			color
		);		
	}
}
void draw_triangle_lower(const Vertex &v1, const Vertex &v2, const Vertex &v3, SDL_Renderer *renderer, Color color) {
	if (v2.y == v3.y) {
		draw_line({v2, v3}, renderer, color);	
		return;
	}
	float s1{ static_cast<float>(v3.x - v1.x)/(v3.y - v1.y)};
	float s2{ static_cast<float>(v3.x - v2.x)/(v3.y - v2.y)};
	for (int y = v2.y; y >= v3.y; y--) {
		draw_line(
			{{(int)(v1.x + y * s1), y}, {(int)(v2.x + y * s2), y}},
			renderer,
			color
		);		
	}
}

void draw_line(Line l, SDL_Renderer *renderer, Color color)

{
    SDL_SetRenderDrawColor(renderer, 
            color.r,
            color.g,
            color.b,
            color.a);

    int y {};
    bool is_steep {false};
    if (std::abs(l.from.y - l.to.y) > std::abs(l.from.x - l.to.x)) { // transpose if we have a steep line
        std::swap(l.from.x, l.from.y);
        std::swap(l.to.x, l.to.y);
        is_steep = true;
    }

    if (l.to.x < l.from.x) { // make sure direction doesn´t effect render
        std::swap(l.to.x, l.from.x);
		std::swap(l.from.y, l.to.y);
    }

    for (int x = l.from.x; x <= l.to.x; x++) {
        float t {static_cast<float>(x - l.from.x)/(l.to.x - l.from.x)};
        y = l.from.y + (l.to.y - l.from.y) * t;
        if (is_steep) {
            SDL_RenderDrawPoint(renderer, y, x);
        } else {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}
