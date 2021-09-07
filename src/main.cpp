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
            char* new_dialog = map.player_interact();
            if(new_dialog != NULL) {
                ui.dialog_open(new_dialog);
            }
        } else {
            ui.dialog_progress();
            if(!ui.dialog_is_open) {
                map.npc_being_talked_to = -1;
            }
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
    int player_input = -1;
    if(!ui.dialog_is_open) {
        player_input = player_input_direction;
    }
    map.update(player_input);
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
    vec2 start_tile = tile_at(map.camera_position);
    vec2 base_render_pos = position_of(start_tile) - map.camera_position;
    vec2 draw_size = vec2(SCREEN_WIDTH / TILE_SIZE, SCREEN_HEIGHT / TILE_SIZE);
    if(map.camera_position.x % TILE_SIZE != 0) {
        draw_size.x++;
    }
    if(map.camera_position.y % TILE_SIZE != 0) {
        draw_size.y++;
    }
    for(int y = 0; y < draw_size.y; y++) {
        for(int x = 0; x < draw_size.x; x++) {
            vec2 render_pos = base_render_pos + vec2(x * TILE_SIZE, y * TILE_SIZE);
            engine_render_sprite_frame(SPRITE_TILES, map.tiles[start_tile.y + y][start_tile.x + x], render_pos.x, render_pos.y, false);
        }
    }

    for(int i = 0; i < map.actor_count; i++) {
        vec2 render_pos = map.actors[i].position - map.camera_position;
        engine_render_actor_animation(map.actors[i].animation, map.actors[i].facing_direction, render_pos.x, render_pos.y);
    }
}

void render_ui() {
    if(ui.dialog_is_open) {
        engine_render_dialog(ui.dialog_rows, ui.dialog_display_length);
    }
}
