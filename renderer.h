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
#define DEPTH 255

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

void clear_screen(Renderer &renderer);
void draw_point(const Renderer &renderer, const Point2D &p, const Color &clr);

void draw_model(Renderer &renderer, const Model &model);
void draw_face(Renderer &renderer, const Vector<3> &v1, const Vector<3> &v2, const Vector<3> &v3, const Color &color);
std::function<bool (const Vector<3> &)> get_edge_func(const Vector<3> &v1,  const Vector<3> &v2, const Vector<3> &v3);
std::function<float (const Point2D&)> findPlaneSolution(const Vector<3> &v1, const Vector<3> &v2, const Vector<3> &v3);

#endif
