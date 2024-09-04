#include <SDL2/SDL_render.h>
#include <iostream>
#include "model.h"
#include "renderer.h"
#include <SDL2/SDL.h>
#include <limits>
#include <algorithm>
#include <array>

int main(int argc, char** argv)
{
	std::array<float, 5> a{ 1.f, 2.f, 3.f, 4.f, 5.f };
	std::array<std::array<float, 5>, 1> b{{a}};


	try {
    Model model{ "head.obj" };    
	Renderer renderer{};
	draw_model(renderer, model);

    SDL_RenderPresent(renderer.sdl_renderer);

    SDL_Delay(3000);
    SDL_Quit();
	} catch (const char* &ex) {
		std::cout << ex << "\n";
	}

    return 0;
}

