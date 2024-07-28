#ifndef H_RENDERER
#define H_RENDERER

#include "model.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <cstdint>
#include <array>

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
	std::array<std::array<int, SCREEN_HEIGHT>, SCREEN_WIDTH> zbuffer;
} Context;

Point2D project_vertex(const Vertex &v);
void draw_model(const Context &context, const Model &model);
void draw_point(const Context &context, Point2D p);
void set_color(const Context &context, Color clr);

void draw_face(const Context &context, Point2D p1, Point2D p2, Point2D p3, Color color);
void draw_triangle_up(const Context &context, Point2D p1, Point2D p2, Point2D p3, Color color);
void draw_face_lower(const Context &context, Point2D p1, Point2D p2, Point2D p3, Color color);
void draw_line(const Context &context, Line l, Color color);

#endif
