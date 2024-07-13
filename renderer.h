#ifndef H_RENDERER
#define H_RENDERER

#include "model.h"
#include <SDL2/SDL.h>
#include <cstdint>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

Point2D project_vertex(const Vertex &v);
void draw_model(const Model &model, SDL_Renderer *renderer);
void draw_point(SDL_Renderer *renderer, Point2D p);
void set_color(SDL_Renderer *renderer, Color clr);

void draw_triangle(Point2D p1, Point2D p2, Point2D p3, SDL_Renderer *renderer, Color color);
void draw_triangle_upper(Point2D p1, Point2D p2, Point2D p3, SDL_Renderer *renderer, Color color);
void draw_triangle_lower(Point2D p1, Point2D p2, Point2D p3, SDL_Renderer *renderer, Color color);
void draw_line(Line l, SDL_Renderer *renderer, Color color);

#endif
