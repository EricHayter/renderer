#include "renderer.h"
#include <algorithm>

void draw_line(Line l, SDL_Renderer *renderer, Color color)
{
    // save the current color
    Color prev_color {};
    uint8_t r, g, b, a; 
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a); // do a check for error in a sec
    prev_color.r = r;
    prev_color.g = g;
    prev_color.b = b;
    prev_color.a = a;

    SDL_SetRenderDrawColor(renderer, 
            color.r,
            color.g,
            color.b,
            color.a);

    int y {};
    bool is_steep {false};
    if (std::abs(l.to.y - l.from.y) > std::abs(l.to.x - l.from.x)) {
        std::swap(l.from.x, l.from.y);
        std::swap(l.to.x, l.to.y);
        is_steep = true;
    }

    if (l.to.x < l.from.x) {
        std::swap(l.to.x, l.from.x);
    }

    for (int x = l.from.x; x <= l.to.x; x++) {
        float t {static_cast<float>(x - l.from.x)/(l.to.x - l.from.x)};
        y = l.from.y + (l.to.y - l.from.y) * t;
        if (is_steep) {
            SDL_RenderDrawPoint(renderer, y, x);
        } else {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

