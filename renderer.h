#ifndef H_RENDERER
#define H_RENDERER

#include "model.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <array>
#include <functional>

#define SCREEN_WIDTH 900 // 640
#define SCREEN_HEIGHT 900 // 480
#define DEPTH 900

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct Point2D {
	int x, y;
} Point2D;

typedef struct Renderer {
	SDL_Renderer *sdl_renderer;
	SDL_Window *window;
	Vector<3> light_dir;
	Vector<3> pos;
	float yaw; 
	float pitch;
	std::array<std::array<float, SCREEN_HEIGHT>, SCREEN_WIDTH> zbuffer;

	Renderer();
	~Renderer();
} Renderer;

// blacks out the entire screen and resets the value of z-buffer 
void clear_screen(Renderer &renderer);

// draws a point of given color on the screen
void draw_point(const Renderer &renderer, const Point2D &p, const Color &clr);

// renderes a givne model
void draw_model(Renderer &renderer, const Model &model);

// given 3 vectors and their normal vectors render the face with a given 
// color and shading and interpolate normal along face using phone shading
void draw_face(Renderer &renderer, 
		const Vector<3> &v1, 
		const Vector<3> &v1n, 
		const Vector<3> &v2, 
		const Vector<3> &v2n, 
		const Vector<3> &v3,
		const Vector<3> &v3n,
		const Color &clr);

// given 3 points to define a triangle return a function that takes a point
// as a parameter and returns if the point is inside the triangle
// using method found in: A Parallel Algorithm for Polygon Rasterization
// by J. Pineda
std::function<bool (float, float)> 
get_edge_func(const Vector<3> &v1,  
			  const Vector<3> &v2, 
			  const Vector<3> &v3);

// given 3 points to define a plane return a function that finds a solution 
// on the plane given some parameter of a point (x, y)
std::function<float (float, float)> 
findPlaneSolution(const Vector<3> &v1, 
				  const Vector<3> &v2, 
				  const Vector<3> &v3);


float triangleArea(const Vector<3> &v1, const Vector<3> &v2, const Vector<3> &v3);

// given a plane with 3 verticies with a normal for each vertex
// return a function to interpolate a normal for a given point (x,y) on the plane
std::function<Vector<3> (float, float)> 
findNormalSolution(const Vector<3> &v1, const Vector<3> &v1n, 
				   const Vector<3> &v2, const Vector<3> &v2n, 
				   const Vector<3> &v3, const Vector<3> &v3n);

#endif
