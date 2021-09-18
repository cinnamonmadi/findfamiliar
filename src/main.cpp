#include "engine.hpp"
#include "state.hpp"
#include "world.hpp"
#include "edit.hpp"
#include <string>
#include <iostream>
#include <stack>

int main(int argc, char** argv) {
    bool edit_mode = false;
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
        } else if(arg == "--edit") {
            edit_mode = true;
        }
    }

    Engine engine;

    if(!engine.init(resolution_width, resolution_height, init_fullscreened)) {
        return 0;
    }

    std::stack<State*> states;

    if(edit_mode) {
        states.push(new Edit());
    } else {
        states.push(new World());
    }

    State* current_state = states.top();

    bool running = true;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                running = false;
            } else{
                current_state->handle_input(e);
            }
        }

        current_state->update();

        engine.render_clear();
        current_state->render(&engine);
        engine.render_text(("FPS " + std::to_string(engine.fps)).c_str(), 0, 0);
        engine.render_present();

        engine.clock_tick();
    }

    while(states.size() != 0) {
        delete states.top();
        states.pop();
    }

    engine.quit();

    return 0;
}
