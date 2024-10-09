#include <iostream>
#include "model.h"
#include "renderer.h"
#include "vector.h"
#include <math.h>
#include <chrono>

#define DEFAULT_MODEL "obj_files/head.obj"

int main(int argc, char** argv)
{
	char const *model_name{ DEFAULT_MODEL };
	if (argc == 2)
		model_name = argv[1];

	try {
		int frames{16};
		Model model{ model_name };    
  		Renderer renderer{};
		renderer.yaw = 0;
		renderer.pitch = 0;
		for (int i = 0; i < frames; i++) {
			renderer.yaw = 1.f/4 * M_PI * i / frames;	

			// let's time the execution time
		    auto t1 = std::chrono::high_resolution_clock::now();
			draw_model(renderer, model);
		    auto t2 = std::chrono::high_resolution_clock::now();
			auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
			std::cout << "took " << ms_int.count() << " milliseconds\n";

			SDL_RenderPresent(renderer.sdl_renderer);
			clear_screen(renderer);
//			SDL_Delay(1000);
		}
	

		SDL_Delay(1000 * 10);
		SDL_Quit();
	} catch (const char* ex) {
		std::cout << ex << "\n";
	}

    return 0;
}

