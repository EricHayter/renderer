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
	// draw_line({{270, 600}, {230, 550}}, renderer, {255, 0, 0});
	draw_point(renderer, {300, 600});
	draw_triangle({279, 594}, {293, 574}, {357, 573}, renderer, {255, 0, 255});
	set_color(renderer, {0, 255, 255});
	draw_point(renderer, {279, 594});
	draw_point(renderer, {293, 574});
	draw_point(renderer, {357, 573});

//	draw_triangle({50, 50}, {50, 100}, {100, 100}, renderer, {0, 255, 255});
//	draw_triangle({50, 50}, {200, 200}, {200, 50}, renderer, {255, 255, 0});

    SDL_RenderPresent(renderer);

    SDL_Delay(3000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

