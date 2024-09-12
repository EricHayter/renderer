#include <iostream>
#include "model.h"
#include "renderer.h"

#define DEFAULT_MODEL "head.obj"

int main(int argc, char** argv)
{
	char const *model_name{ DEFAULT_MODEL };
	if (argc == 2)
		model_name = argv[1];

	try {
		Model model{ model_name };    
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

