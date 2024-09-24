#include "renderer.h"
#include "vector.h"
#include "model.h"
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <array>
#include <functional>
#include <limits>

//=============================================================================
// Renderer Object
//=============================================================================
Renderer::Renderer() :
	zbuffer{},
	light_dir{ 0,  0, -1 },
	pos{0, 0, -4} // maybe tweak this value later
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

Renderer::~Renderer() {
    SDL_DestroyWindow(window);
}

//=============================================================================
// Utility Functions
//=============================================================================
void clear_screen(Renderer &renderer)
{
	// make sure render color is black before clearing
    SDL_SetRenderDrawColor(renderer.sdl_renderer, 0, 0, 0, 0);

	SDL_RenderClear(renderer.sdl_renderer);
	for (auto i = 0; i < renderer.zbuffer.size(); i++) {
		for (auto j = 0; j < renderer.zbuffer[i].size(); j++) {
			renderer.zbuffer[i][j] = -std::numeric_limits<float>::max();
		}	
	}
}

void draw_point(const Renderer& renderer, const Point2D &p, const Color &clr)
{
    SDL_SetRenderDrawColor(renderer.sdl_renderer, 
            clr.r,
            clr.g,
            clr.b,
            clr.a);

	SDL_RenderDrawPoint(renderer.sdl_renderer, p.x, p.y);
}

//=============================================================================
// Rendering Models
//=============================================================================
void draw_model(Renderer &renderer, const Model &model)
{
	Vector<3> z{ view_vector(renderer.yaw, renderer.pitch).normalize() };
	Vector<3> x{ cross_product({0, 1, 0}, z).normalize() }; 
	Vector<3> y{ cross_product(z, x).normalize() };

	Matrix<4, 4> modelView{
		{x[0], x[1], x[2], renderer.pos[0]},
		{y[0], y[1], y[2], renderer.pos[1]},
		{z[0], z[1], z[2], renderer.pos[2]},
		{0.f,  0.f,  0.f,  1.f}	
	};

	// let's create our transformation matrix here
	Matrix<4, 4> projMatrix{
		{1.f, 		0.f, 		0.f, 				0.f},		 	
		{0.f, 		1.f, 		0.f, 				0.f},		 	
		{0.f, 		0.f, 		1.f, 				0.f},		 	
		{0.f, 		0.f, 		1/renderer.pos[Z], 	1.f}
	};



	// this will scale our points to appropriate sizes for our screen
	Matrix<4, 4> viewPort{
		{ SCREEN_WIDTH/2.f, 	0, 					0, 				SCREEN_WIDTH/2.f },
		{ 0, 					-SCREEN_HEIGHT/2.f, 0, 				SCREEN_HEIGHT/2.f},
		{ 0, 					0, 					DEPTH/2.f, 	DEPTH/2.f},
		{ 0, 					0, 					0, 				1.f }
	};
		
	Matrix<4, 4> transMatrix{ viewPort * projMatrix * modelView};

	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<FaceTuple> face = model.face(i);
		Vector<3> v1{ model.vertex(face[0].vertex - 1) };
		Vector<3> v1s{Vector<4>(transMatrix * v1.homogenize()).dehomogenize()};

		// Triangle fanning
		for (int j = 2; j < face.size(); j++) { // outer loop for start points
			//Vector<3> v2{ project_vertex(tm, model.vertex(face[j-1].vertex - 1)) };
			//Vector<3> v3{ project_vertex(tm, model.vertex(face[j].vertex - 1)) };
			Vector<3> v2{ model.vertex(face[j-1].vertex - 1) };
			Vector<3> v3{ model.vertex(face[j].vertex - 1) };
			
			// find the normal of vectors from v1 to v2 and v1 to v3
			Vector<3> normal{ cross_product(v3 - v1, v2 - v1).normalize() };
			float intensity{ dot_product(normal, renderer.light_dir) };

			// transform vectors for screen
			Vector<3> v2s{Vector<4>(transMatrix * v2.homogenize()).dehomogenize()};
			Vector<3> v3s{Vector<4>(transMatrix * v3.homogenize()).dehomogenize()};

			
			
			// intensity of light reflected will be equal to dot product of 
			// view vector and normal of face
			if (intensity > 0)
				draw_face(renderer, v1s, v2s, v3s, 
						{(uint8_t)(255 * intensity), 
						 (uint8_t)(255 * intensity), 
						 (uint8_t)(255 * intensity),
						 255}
						);
		}
	}
}

void draw_face(Renderer &renderer, 
		const Vector<3> &v1, 
		const Vector<3> &v1n, 
		const Vector<3> &v2, 
		const Vector<3> &v2n, 
		const Vector<3> &v3,
		const Vector<3> &v3n,
		const Color &clr)
{
	auto edgeFunc{ get_edge_func(v1, v2, v3) };
	auto solFunc{ findPlaneSolution(v1, v2, v3) };
	// auto normalFunc{ findNormalSolution(v1, 

	// create a bounding box around our triangle
	float maxX{ std::max({ v1[X], v2[X], v3[X] }) };	
	maxX = maxX >= SCREEN_WIDTH ? SCREEN_WIDTH - 1 : maxX;

	float minX{ std::min({ v1[X], v2[X], v3[X] }) };	
	minX = minX <= 0 ? 0 : minX;

	float maxY{ std::max({ v1[Y], v2[Y], v3[Y] }) };	
	maxY = maxY >= SCREEN_HEIGHT ? SCREEN_HEIGHT - 1 : maxY;

	float minY{ std::min({ v1[Y], v2[Y], v3[Y] }) };	
	minY = minY <= 0 ? 0 : minY;

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			if (!edgeFunc({(float)x, (float)y, 0.f}))
					continue;
			float z{ solFunc({x, y}) };
			if (z >= renderer.zbuffer[x][y]) {
				draw_point(renderer, {x, y}, color);	
				renderer.zbuffer[x][y] = z;
			}
		}
	}
}

std::function<bool (const Vector<3> &)> 
get_edge_func(const Vector<3> &v1,  const Vector<3> &v2, const Vector<3> &v3)
{
	return [v1, v2, v3](const Vector<3> &p){
		bool ret{ true };
		Vector<3> dv12{ v1 - v2 };
		Vector<3> dv23{ v2 - v3 };
		Vector<3> dv31{ v3 - v1 };

		ret &= ((p[Y] - v1[Y])*dv12[X] - (p[X] - v1[X])*dv12[Y] >= 0);
		ret &= ((p[Y] - v2[Y])*dv23[X] - (p[X] - v2[X])*dv23[Y] >= 0);
		ret &= ((p[Y] - v3[Y])*dv31[X] - (p[X] - v3[X])*dv31[Y] >= 0);

		return ret;	
	};
}

// given 3 points to define a plane return a function for solutions of 
// Z given some X, Y.
std::function<float (const Point2D&)> 
findPlaneSolution(const Vector<3> &v1, const Vector<3> &v2, const Vector<3> &v3)
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

std::function<Vector<3> (const Point2D&)> 
findNormalSolution(const Vector<3> &v1, const Vector<3> &v1n, 
				   const Vector<3> &v2, const Vector<3> &v2n, 
				   const Vector<3> &v3, const Vector<3> &v3n)
{
	// disregard the z coordinate
	// these will form our basis of R2
	Vector<3> dv2{ v2 - v1 };
	Vector<3> dv3{ v3 - v1 };

	// check if these are co-linear in x and y
	// also implicitly checks for zero vectors
	if (dv3[X] * dv2[Y] == dv3[Y] * dv2[X])
		return [dv3, dv2](const Point2D &p) { return cross_product(dv3, dv2); };

	// make sure non-zero in top-left for RREF
	if (dv3[X] == 0)
		std::swap(dv3, dv2);
	
	// this is some row reduction to be hardcoded for R^2	
	return [v1, dv3, dv2, v1n, v2n, v3n](const Point2D &p){
		// find s, t such that np = s * dv1 + t * dv2
		float s{};
		float t{};
		Point2D np{ (int)(p.x - v1[X]), (int)(p.y - v1[Y]) };

		// reduce second row
		if (dv3[Y] == 0) {
			// NOTE: dv2[Y] has been checked to be non-zero at colinear check
			t = np.y / dv2[Y];
		} else {
			t = np.y / dv3[X] - np.x;
		}

		// reduce first row
		s = np.x / dv3[X] - t;

		return v1n + s * (v3n - v1n) + t * (v2n - v1n); 
	};
}
