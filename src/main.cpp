#include "engine.hpp"
#include "world.hpp"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    bool init_fullscreened = false;
    int resolution_width = Engine::SCREEN_WIDTH * 4;
    int resolution_height = Engine::SCREEN_HEIGHT * 4;

    // Parse system arguments
    for(int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if(arg == "--fullscreen"){
            init_fullscreened = true;
        } else if(arg == "--resolution") {
            if(i + 1 == argc) {
                std::cout << "No resolution was specified!" << std::endl;
                return 0;
            }

            i++;
            std::string resolution_input = std::string(argv[i]);

            size_t x_index = resolution_input.find_first_of("x");
            if(x_index == std::string::npos) {
                std::cout << "Incorrect resolution format!" << std::endl;
                return 0;
            }

            resolution_width = atoi(resolution_input.substr(0, x_index).c_str());
            resolution_height = atoi(resolution_input.substr(x_index + 1, resolution_input.length() - x_index + 1).c_str());

            if(resolution_width == 0 || resolution_height == 0) {
                std::cout << "Incorrect resolution format!" << std::endl;
                return 0;
            }
        }
    }

    Engine engine;

    if(!engine.init(resolution_width, resolution_height, init_fullscreened)) {
        return 0;
    }

    World world(&engine);

    bool running = true;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                running = false;
            } else{
                world.handle_input(e);
            }
        }

        world.update();

        engine.render_clear();
        world.render();
        engine.render_text(("FPS " + std::to_string(engine.fps)).c_str(), 0, 0);
        engine.render_present();

        engine.clock_tick();
    }

    engine.quit();

    return 0;
}
