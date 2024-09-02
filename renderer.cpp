#include "renderer.h"
#include "vector.h"
#include "model.h"
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <array>
#include <functional>
#include <limits>

const Vector<3> light_dir{{ 0, 0, -1 }};

Vector<3> scale_vertex(const Vector<3> &v)
{
	return {{ 	
		(v[X] + 1.0f) * SCREEN_WIDTH / 2, 
		(-v[Y] + 1.0f) * SCREEN_HEIGHT / 2,
		v[Z]
	}};
}

Matrix<4, 4> get_retro_proj_matrix(const Renderer &renderer)
{
	return {{{ // this is a lot of curly braces
			{1.f, 0.f, 0.f, 0.f},		 	
			{0.f, 1.f, 0.f, 0.f},		 	
			{0.f, 0.f, 1.f, 0.f},		 	
			{0.f, 0.f, -1/7.f, 1.f}
	}}};
}

Vector<3> project_vertex(const Renderer &renderer, const Vector<3> &v)
{
	Vector<4> vh{ v.homogenize() };

	// where should I pass in the transformation matrix???
	
	// make it ourselves or should I base it on the context structure?
	//

	return vh.dehomogenize();
}

// let's make a constructor for the renderer here
Renderer::Renderer_t() :
	zbuffer{},
	light_dir{{ 0,  0, -1 }}
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        exit(-1);
    }

    window = SDL_CreateWindow(
            "renderer",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
            );

    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        exit(-1);
    }
    
	sdl_renderer = SDL_CreateRenderer(window, -1, 0);

	// set the 
	for (auto i = 0; i < zbuffer.size(); i++) {
		for (auto j = 0; j < zbuffer[i].size(); j++) {
			zbuffer[i][j] = -std::numeric_limits<float>::max();
		}	
	}
}

Renderer::~Renderer_t() {
    SDL_DestroyWindow(window);
}

// make a function to clear the screen (don't forget about zbuffer too)
void clear_screen(Renderer &renderer)
{
	set_color(renderer, {0, 0, 0, 255});
	SDL_RenderClear(renderer.sdl_renderer);
	for (auto i = 0; i < renderer.zbuffer.size(); i++) {
		for (auto j = 0; j < renderer.zbuffer[i].size(); j++) {
			renderer.zbuffer[i][j] = -std::numeric_limits<float>::max();
		}	
	}
}

void draw_point(Renderer& renderer, Point2D p)
{
	SDL_RenderDrawPoint(renderer.sdl_renderer, p.x, p.y);
}

void set_color(Renderer &renderer, Color clr)
{
    SDL_SetRenderDrawColor(renderer.sdl_renderer, 
            clr.r,
            clr.g,
            clr.b,
            clr.a);
}

// this is just a POC I'll be editting the values of the vertices in this
// function (will edit to remove this feature later)
void draw_model(Renderer &renderer, const Model &model)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<FaceTuple> face = model.face(i);
		Vector<3> v1{ model.vertex(face[0].vertex - 1) };

		// Triangle fanning
		for (int j = 2; j < face.size(); j++) { // outer loop for start points
			Vector<3> v2{ model.vertex(face[j-1].vertex - 1) };
			Vector<3> v3{ model.vertex(face[j].vertex - 1) };

			// find the normal of vectors from v1 to v2 and v1 to v3
			Vector<3> normal{ cross_product(v3 - v1, v2 - v1).normalize() };

			// intensity of light reflected will be equal to dot product of 
			// view vector and normal of face
			float intensity{ dot_product(normal, light_dir) };
			Vector<3> v1n{ scale_vertex(v1) };
			Vector<3> v2n{ scale_vertex(v2) };
			Vector<3> v3n{ scale_vertex(v3) };
			if (intensity > 0)
				draw_face(renderer, v1n, v2n, v3n, 
						{(uint8_t)(255 * intensity), 
						 (uint8_t)(255 * intensity), 
						 (uint8_t)(255 * intensity),
						 255}
						 );
		}
	}
}

void draw_face(Renderer &renderer, Vector<3> v1, Vector<3> v2, Vector<3> v3, Color color)
{
	set_color(renderer, color);
	if (v1[Y] < v2[Y])
		std::swap(v1, v2);
	if (v2[Y] < v3[Y])
		std::swap(v2, v3);
	if (v1[Y] < v2[Y])
		std::swap(v1, v2);

	if (v3[Y] - v1[Y] == 0)
		return;

	// parametrization of a line from p1 to p3
	float t{ (v2[Y] - v3[Y])/(v3[Y] - v1[Y]) };	
	float x{ (v3[X] - v1[X])*t + v3[X] };
	float y{ v2[Y] };
	float z{ (v3[Z] - v1[Z])*t + v3[Z] };

	Vector<3> vt{{ x, y, z }};
	draw_face_upper(renderer, v1, v2, vt, color);
	draw_face_lower(renderer, vt, v2, v3, color);
}

void draw_face_upper(Renderer &renderer, Vector<3> v1, Vector<3> v2, Vector<3> v3, Color color)
{
	if (v1[Y] == v2[Y]) { // this triangle is pointing down
		return;
	}
	
	if (v2[X] > v3[X]) // put v2 on the left and v3 on the right
		std::swap(v2, v3);

	std::function<float (const Point2D&)> solFunc{ findPlaneSolution(v1, v2, v3) };

	float s2{ static_cast<float>(v1[X] - v2[X])/(v1[Y] - v2[Y])};
	float s3{ static_cast<float>(v1[X] - v3[X])/(v1[Y] - v3[Y])};
	float pointer2{ (float)v2[X] - s2 };
	float pointer3{ (float)v3[X] - s3 };

	for (int y = v2[Y]; y <= v1[Y]; y++) {
		pointer2 += s2;
		pointer3 += s3;
		for (int x = pointer2; x <= pointer3; x++) {
			float z{ solFunc({x, y}) };
			if (z >= renderer.zbuffer[x][y]) {
				draw_point(renderer, {x, y});	
				renderer.zbuffer[x][y] = z;
			}
		}
	}
}

void draw_face_lower(Renderer &renderer, Vector<3> v1, Vector<3> v2, Vector<3> v3, Color color)
{
	if (v2[Y] == v3[Y]) {
		return;
	}

	if (v1[X] > v2[X]) // make sure that v1 is on the left
		std::swap(v1, v2);

	std::function<float (const Point2D&)> solFunc{ findPlaneSolution(v1, v2, v3) };

	float sl{ ((float)v3[X] - v1[X])/(v3[Y] - v1[Y])};
	float sr{ ((float)v3[X] - v2[X])/(v3[Y] - v2[Y])};
	float pointerl{ (float)v3[X] - sl };
	float pointerr{ (float)v3[X] - sr};
	for (int y = v3[Y]; y <= v2[Y]; y++) {
		pointerl += sl;
		pointerr += sr;	
		for (int x = pointerl; x <= pointerr; x++) { 
			float z{ solFunc({x, y}) };
			if (z >= renderer.zbuffer[x][y]) {
				draw_point(renderer, {x, y});	
				renderer.zbuffer[x][y] = z;
			}
		}
	}
}

// given 3 points to define a plane return a function for solutions of 
// Z given some X, Y.
std::function<float (const Point2D&)> findPlaneSolution(const Vector<3> &v1, const Vector<3> &v2, const Vector<3> &v3)
{
	Vector<3> normal{ cross_product(v3 - v1, v2 - v1) };
	if (normal[Z] == 0)
		return [](const Point2D& p){ 
			return std::numeric_limits<float>::min();  // normal has no z component
		};
	float a{ normal[X] };
	float b{ normal[Y] };
	float c{ normal[Z] };
	float x0{ v1[X] };
	float y0{ v1[Y] };
	float z0{ v1[Z] };
	return [a, b, c, x0, y0, z0](const Point2D& p){ 
		return (-a * (p.x - x0) - b * (p.y - y0))/c + z0;
	};
}
