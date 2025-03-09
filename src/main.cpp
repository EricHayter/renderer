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
        // let's time the execution time
        auto start_time = std::chrono::high_resolution_clock::now();

        int frames{100};
        Model model{model_name};
        Renderer* renderer = Renderer::GetRenderer();
        renderer->yaw = 0;
        renderer->pitch = 0;
        for (int i = 0; i < frames; i++) {
            renderer->yaw = 1.f / 4.f * M_PI_2f * static_cast<float>(i) / static_cast<float>(frames);

            renderer->draw_model(model);

            renderer->clear_screen();
        }
        auto stop_time = std::chrono::high_resolution_clock::now();
        long milliseconds_elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(stop_time -
                                                                  start_time)
                .count();
        std::cout << "It took " << milliseconds_elapsed
                  << " milliseconds to print " << frames << " frames ("
                  << static_cast<float>(milliseconds_elapsed) /
                         static_cast<float>(frames)
                  << " FPS)\n";

    } catch (const char* ex) {
        std::cout << ex << "\n";
    }

    return 0;
}
