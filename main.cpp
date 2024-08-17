#include <SDL2/SDL_render.h>
#include <iostream>
#include "model.h"
#include "renderer.h"
#include <SDL2/SDL.h>
#include <limits>
#include <algorithm>

int main(int argc, char** argv)
{
    Model model{ "head.obj" };    
	Renderer renderer{};
	draw_model(renderer, model);


    SDL_RenderPresent(renderer.sdl_renderer);

    SDL_Delay(3000);
    SDL_Quit();

    return 0;
}

