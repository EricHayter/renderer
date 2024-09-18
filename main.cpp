#include <iostream>
#include "model.h"
#include "renderer.h"
#include <math.h>
#include <chrono>

#define DEFAULT_MODEL "head.obj"

int main(int argc, char** argv)
{
	char const *model_name{ DEFAULT_MODEL };
	if (argc == 2)
		model_name = argv[1];

	try {
		Model model{ model_name };    
  		Renderer renderer{};
		renderer.yaw = 0;
		renderer.pitch = 0;
		for (int i = 0; i < 4; i++) {
			renderer.yaw = M_PI * i / 4.f;	

			// let's time the execution time
		    auto t1 = std::chrono::high_resolution_clock::now();
			draw_model(renderer, model);
		    auto t2 = std::chrono::high_resolution_clock::now();
			auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
			std::cout << "took " << ms_int.count() << " milliseconds\n";

			SDL_RenderPresent(renderer.sdl_renderer);
			SDL_Delay(2000);
			clear_screen(renderer);
		}
	

		SDL_Quit();
	} catch (const char* ex) {
		std::cout << ex << "\n";
	}

    return 0;
}

