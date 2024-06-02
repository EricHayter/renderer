#ifndef H_RENDERER
#define H_RENDERER

#include <SDL2/SDL.h>
#include <cstdint>

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct Point_t {
    float x;
    float y;
    float z;
    float w; // optional argument (defaults to 0)

    Point_t(float x, float y) : 
        x{ x },
        y{ y },
        z{ 0 },
        w{ 1 }
    {}

    Point_t() : 
        x{ 0 },
        y{ 0 },
        z{ 0 },
        w{ 1 }
    {}

    Point_t(float x, float y, float z) :
        x{ x },
        y{ y },
        z{ z },
        w{ 1 }
    {}

    Point_t(float x, float y, float z, float w) :
        x{ x },
        y{ y },
        z{ z },
        w{ w }
    {}
} Point;

typedef struct Line_t {
    Point to;
    Point from; 

    Line_t(Point to, Point from) :
        to{ to },
        from {from }
    {}
} Line;

void draw_line(Line l, SDL_Renderer *renderer, Color color);

#endif
