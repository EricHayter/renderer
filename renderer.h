#ifndef H_RENDERER
#define H_RENDERER

#include "model.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <cstdint>
#include <array>
#include <functional>

#define SCREEN_WIDTH 900 // 640
#define SCREEN_HEIGHT 900 // 480

typedef struct {
    uint8_t r, g, b, a;
} Color;

void set_color(SDL_Renderer *renderer, Color color);

typedef Vertex Vector3D;

typedef struct Point2D_t {
	int x;
	int y;
} Point2D;

typedef struct Line_t {
    Point2D to;
    Point2D from; 
} Line;

typedef struct Context_t {
	SDL_Renderer *sdl_renderer;
	Vector3D light_dir;
	std::array<std::array<float, SCREEN_HEIGHT>, SCREEN_WIDTH> zbuffer;
} Context;

Vertex scale_vertex(const Vertex &v);
void draw_model(Context &context, const Model &model);
void draw_point(Context &context, Point2D p);
void set_color(Context &context, Color clr);

void draw_face(Context &context, Vertex v1, Vertex v2, Vertex v3, Color color);
void draw_face_upper(Context &context, Vertex v1, Vertex v2, Vertex v3, Color color);
void draw_face_lower(Context &context, Vertex v1, Vertex v2, Vertex v3, Color color);

std::function<float (const Point2D&)> findPlaneSolution(const Vertex &v1, const Vertex &v2, const Vertex &v3);

void draw_line(Context &context, Line l, Color color);

#endif
