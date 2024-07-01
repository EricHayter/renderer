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

typedef struct Line_t {
    Vertex to;
    Vertex from; 

    Line_t(Vertex to, Vertex from) :
        to{ to },
        from {from }
    {}
} Line;

void draw_model(const Model &model, SDL_Renderer *renderer);

void draw_triangle(Vertex v1, Vertex v2, Vertex v3, SDL_Renderer *renderer, Color color);
void draw_triangle_upper(const Vertex &v1, const Vertex &v2, const Vertex &v3, SDL_Renderer *renderer, Color color);
void draw_triangle_lower(const Vertex &v1, const Vertex &v2, const Vertex &v3, SDL_Renderer *renderer, Color color);
void draw_line(Line l, SDL_Renderer *renderer, Color color);

#endif
