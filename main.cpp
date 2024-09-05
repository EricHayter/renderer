#include <iostream>
#include "model.h"
#include "renderer.h"

int main(int argc, char** argv)
{
	try {
		Model model{ "head.obj" };    
		Renderer renderer{};
		draw_model(renderer, model);

		SDL_RenderPresent(renderer.sdl_renderer);

		SDL_Delay(3000);
		SDL_Quit();
	} catch (const char* ex) {
		std::cout << ex << "\n";
	}

    return 0;
}

