#include "model.h"
#include "renderer.h"
#include <SDL2/SDL.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480



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
    draw_line({{30, 80}, {42, 20}}, renderer, {255, 255, 255, 255});
    SDL_RenderPresent(renderer);

    SDL_Delay(4000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    Model model{ "head.obj" };    

    return 0;
}

