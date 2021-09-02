#include "engine.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>

bool running = true;

void input();
void update();
void render();

bool engine_init(int argc, char** argv);
void engine_quit();
void engine_set_resolution(int width, int height);
void engine_toggle_fullscreen();
void engine_clock_tick();

int main(int argc, char** argv) {
    if(!engine_init(argc, argv)) {
        return 0;
    }

    while(running) {
        input();
        update();
        render();
        engine_clock_tick();
    }

    engine_quit();

    return 0;
}

// Game loop functions

void input() {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            running = false;
        }
    }
}

void update() {

}

void render() {
    engine_render_clear();
    engine_render_text(("FPS " + std::to_string(fps)).c_str(), COLOR_WHITE, 0, 0);
    engine_render_present();
}
