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
	std::array<std::array<float, SCREEN_HEIGHT>, SCREEN_WIDTH> zbuffer;

	Renderer();
	~Renderer();
} Renderer;

Vector<3> scale_vertex(const Vector<3> &v);

void clear_screen(Renderer &renderer);
void set_color(Renderer &renderer, Color clr);
void draw_point(Renderer &renderer, Point2D p);

void draw_model(Renderer &renderer, const Model &model);
void draw_face(Renderer &renderer, Vector<3> v1, Vector<3> v2, Vector<3> v3, Color color);
void draw_face_upper(Renderer &renderer, Vector<3> v1, Vector<3> v2, Vector<3> v3, Color color);
void draw_face_lower(Renderer &renderer, Vector<3> v1, Vector<3> v2, Vector<3> v3, Color color);

std::function<float (const Point2D&)> findPlaneSolution(const Vector<3> &v1, const Vector<3> &v2, const Vector<3> &v3);

#endif
