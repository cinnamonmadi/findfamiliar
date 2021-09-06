#include "global.hpp"
#include "engine.hpp"
#include "sprite.hpp"
#include "map.hpp"
#include "ui.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>

const int DIRECTION_KEYMAP[4] = { SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT };
bool inputs_held[4] = { false, false, false, false };
int player_input_direction = -1;

bool running = true;
Map map;

UI ui;

void input();
void input_handle_keydown(int key);
void input_handle_keyup(int key);

void update();
void render();

void render_map();
void render_ui();

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
        }else if(e.type == SDL_KEYDOWN) {
            input_handle_keydown(e.key.keysym.sym);
        } else if(e.type == SDL_KEYUP) {
            input_handle_keyup(e.key.keysym.sym);
        }
    }
}

void input_handle_keydown(int key) {
    if(key == SDLK_x) {
        if(!ui.dialog_is_open) {
            ui.dialog_open("Hello this is a message.\n And this is a much longer message, let's take a look at it shall we?");
        } else {
            ui.dialog_progress();
        }
    }

    for(int i = 0; i < 4; i++) {
        if(key == DIRECTION_KEYMAP[i]) {
            inputs_held[i] = true;
            player_input_direction = i;
            return;
        }
    }
}

void input_handle_keyup(int key) {
    for(int i = 0; i < 4; i++) {
        if(key == DIRECTION_KEYMAP[i]) {
            inputs_held[i] = false;
            player_input_direction = -1;
            for(int j = 0; j < 4; j++) {
                if(inputs_held[j]) {
                    player_input_direction = j;
                    break;
                }
            }
            return;
        }
    }
}

void update() {
    map.update(player_input_direction);
    ui.update();
}

void render() {
    engine_render_clear();

    render_map();
    render_ui();

    engine_render_text(("FPS " + std::to_string(fps)).c_str(), 0, 0);
    engine_render_present();
}

void render_map() {
    for(int y = 0; y < map.tile_height; y++) {
        for(int x = 0; x < map.tile_width; x++) {
            engine_render_sprite_frame(SPRITE_TILES, map.tiles[y][x], x * 16, y * 16);
        }
    }

    for(int i = 0; i < map.actor_count; i++) {
        engine_render_sprite(map.actors[i].sprite, map.actors[i].position.x, map.actors[i].position.y);
    }
}

void render_ui() {
    if(ui.dialog_is_open) {
        engine_render_dialog(ui.dialog_rows, ui.dialog_display_length);
    }
}
