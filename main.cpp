#include <iostream>
#include "model.h"
#include "renderer.h"
#include <SDL2/SDL.h>

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

    Model model{ "head.obj" };    
	draw_model(model, renderer);

    SDL_RenderPresent(renderer);

    SDL_Delay(10000);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}

