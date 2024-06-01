#include "model.h"
#include <algorithm>
#include <SDL2/SDL.h>
#include <cstdint>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

typedef struct {
    uint8_t r, g, b, a;
} Color;

void line(int x0, int y0, int x1, int y1, SDL_Renderer *renderer, Color color);


void line(int x0, int y0, int x1, int y1, SDL_Renderer *renderer, Color color)
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
    if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        is_steep = true;
    }

    if (x1 < x0) {
        std::swap(x1, x0);
    }

    for (int x = x0; x <= x1; x++) {
        float t {static_cast<float>(x - x0)/(x1 - x0)};
        y = y0 + (y1 - y0) * t;
        if (is_steep) {
            SDL_RenderDrawPoint(renderer, y, x);
        } else {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

int main(int argc, char** argv)
{
    SDL_Window *window {};
    SDL_Renderer *renderer {};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
            "renderer",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
            );

    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return 1;
    }
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, 50, 50);
    line(30, 80, 42, 20, renderer, {255, 255, 255, 255});
    SDL_RenderPresent(renderer);

    SDL_Delay(4000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    Model model{ "head.obj" };    

    return 0;
}

