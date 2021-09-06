#pragma once

#include "sprite.hpp"
#include <SDL2/SDL.h>

const SDL_Color COLOR_WHITE = { .r = 255, .g = 255, .b = 255, .a = 255 };
const int RENDER_POSITION_CENTERED = -1;

// Timing variables
extern int fps;
extern float delta;
extern float dps;

bool engine_init(int argc, char** argv);
void engine_quit();
void engine_set_resolution(int width, int height);
void engine_toggle_fullscreen();
void engine_clock_tick();

void engine_render_clear();
void engine_render_present();
void engine_render_text(const char* text, int x, int y);
void engine_render_dialog(char* dialog_rows[2], size_t dialog_display_length);
// void engine_render_ui_frame(int x, int y, int width, int height);
void engine_render_sprite(Sprite sprite, int x, int y);
void engine_render_sprite_frame(Sprite sprite, int frame, int x, int y);
void engine_render_animation(Animation animation, int x, int y);
