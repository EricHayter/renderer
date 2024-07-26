#include <iostream>
#include "model.h"
#include "renderer.h"
#include <SDL2/SDL.h>
#include <limits>
#include <algorithm>

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
    
	Context context{
		SDL_CreateRenderer(window, -1, 0),		// sdl_renderer init
		{ 0, 0, -1 },							// light direction
		{}										// zbuffer
	};
	std::for_each(context.zbuffer.begin(), context.zbuffer.end(), [](int x) {
		std::for_each(context.zbuffer[x].begin(), context.zbuffer[x].end(), [](int y) {
					context.zbuffer[x][y] = std::min<int>();
				});
			});
    
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    Model model{ "head.obj" };    
	draw_model(context, model);
	// draw_line({{270, 600}, {230, 550}}, renderer, {255, 0, 0});

//	draw_triangle({50, 50}, {50, 100}, {100, 100}, renderer, {0, 255, 255});
//	draw_triangle({50, 50}, {200, 200}, {200, 50}, renderer, {255, 255, 0});

    SDL_RenderPresent(renderer);

    SDL_Delay(3000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

