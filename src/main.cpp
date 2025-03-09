#include <math.h>
#include <chrono>
#include <iostream>
#include "model.h"
#include "renderer.h"
#include "vector.h"

#define DEFAULT_MODEL "obj_files/head.obj"

int main(int argc, char** argv) {
    char const* model_name{DEFAULT_MODEL};
    if (argc == 2)
        model_name = argv[1];

    try {
        int frames{16};
        Model model{model_name};
        Renderer* renderer = Renderer::GetRenderer();
        renderer->yaw = 0;
        renderer->pitch = 0;
        for (int i = 0; i < frames; i++) {
            renderer->yaw = 1.f / 4 * M_PI * i / frames;

            // let's time the execution time
            auto t1 = std::chrono::high_resolution_clock::now();
            renderer->draw_model(model);
            auto t2 = std::chrono::high_resolution_clock::now();
            auto ms_int =
                std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            std::cout << "took " << ms_int.count() << " milliseconds\n";

            renderer->clear_screen();
        }
        SDL_Delay(1000 * 10);
    } catch (const char* ex) {
        std::cout << ex << "\n";
    }

    return 0;
}
