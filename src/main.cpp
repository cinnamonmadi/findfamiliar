#include "engine.hpp"
#include "sprite.hpp"
#include "map.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>

const int DIRECTION_KEYMAP[4] = { SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT };
bool inputs_held[4] = { false, false, false, false };
int player_input_direction = -1;

bool running = true;
Map map;

void input();
void update();
void render();

void render_map();

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
            int key = e.key.keysym.sym;
            for(int i = 0; i < 4; i++) {
                if(key == DIRECTION_KEYMAP[i]) {
                    inputs_held[i] = true;
                    player_input_direction = i;
                    break;
                }
            }
        } else if(e.type == SDL_KEYUP) {
            int key = e.key.keysym.sym;
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
                    break;
                }
            }
        }
    }
}

void update() {
    map.update(delta, player_input_direction);
}

void render() {
    engine_render_clear();

    render_map();

    engine_render_text(("FPS " + std::to_string(fps)).c_str(), COLOR_WHITE, 0, 0);
    engine_render_present();
}

void render_map() {
    for(int y = 0; y < map.tile_height; y++) {
        for(int x = 0; x < map.tile_width; x++) {
            engine_render_sprite_frame(SPRITE_TILES, map.tiles[y][x], x * 16, y * 16);
        }
    }

    engine_render_sprite(map.player.sprite, map.player.position.x, map.player.position.y);
}
