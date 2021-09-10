#pragma once

#include "sprite.hpp"
#include <SDL2/SDL.h>

// Timing variables
extern int fps;
extern float delta;
extern float dps;

bool engine_init(int resolution_width, int resolution_height, bool init_fullscreened);
void engine_quit();
void engine_set_resolution(int width, int height);
void engine_toggle_fullscreen();
void engine_clock_tick();

void engine_render_clear();
void engine_render_present();
void engine_render_text(const char* text, int x, int y);
void engine_render_dialog(char* dialog_rows[2], size_t dialog_display_length);
void engine_render_sprite(Sprite sprite, int x, int y);
void engine_render_sprite_frame(Sprite sprite, int frame, int x, int y, bool flipped);
void engine_render_animation(Animation animation, int x, int y);
void engine_render_actor_animation(Animation animation, int direction, int x, int y);
