#ifndef H_RENDERER
#define H_RENDERER

#include "model.h"
#include <SDL2/SDL.h>
#include <cstdint>

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

void draw_line(Line l, SDL_Renderer *renderer, Color color);

#endif
