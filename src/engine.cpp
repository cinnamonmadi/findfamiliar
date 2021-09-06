#include "engine.hpp"

#include "global.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>

// Constants
const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;
const float FRAME_DURATION = 1.0f / 60.0f;

// Engine variables
SDL_Window* window;
SDL_Renderer* renderer;
bool engine_is_fullscreen = false;

// Timing variables
float last_frame_time = 0.0f;
float last_update_time = 0.0f;
float last_second_time = 0.0f;
int frames = 0;
float deltas = 0.0f;
int fps = 0;
float delta = 0.0f;
float dps = 0.0f;

// Textures
SDL_Texture* sprite_texture[SPRITE_COUNT];
int sprite_texture_width[SPRITE_COUNT];
int sprite_texture_height[SPRITE_COUNT];

bool engine_fonts_init();
void engine_fonts_quit();
bool engine_sprite_textures_init();
void engine_sprite_textures_quit();

// Engine init functions

bool engine_init(int argc, char** argv) {
    bool init_fullscreened = false;
    int resolution_width = SCREEN_WIDTH * 4;
    int resolution_height = SCREEN_HEIGHT * 4;

    // Parse system arguments
    for(int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if(arg == "--fullscreen"){
            init_fullscreened = true;
        } else if(arg == "--resolution") {
            if(i + 1 == argc) {
                std::cout << "No resolution was specified!" << std::endl;
                return false;
            }

            i++;
            std::string resolution_input = std::string(argv[i]);

            size_t x_index = resolution_input.find_first_of("x");
            if(x_index == std::string::npos) {
                std::cout << "Incorrect resolution format!" << std::endl;
                return false;
            }

            resolution_width = atoi(resolution_input.substr(0, x_index).c_str());
            resolution_height = atoi(resolution_input.substr(x_index + 1, resolution_input.length() - x_index + 1).c_str());

            if(resolution_width == 0 || resolution_height == 0) {
                std::cout << "Incorrect resolution format!" << std::endl;
                return false;
            }
        }
    }

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Unable to initialize SDL! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Highlands", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int img_flags = IMG_INIT_PNG;

    if(!(IMG_Init(img_flags) & img_flags)){
        std::cout << "Unable to initialize SDL_image! SDL Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if(!window || !renderer){
        std::cout << "Unable to initial engine!" << std::endl;
        return false;
    }

    engine_set_resolution(resolution_width, resolution_height);
    if(init_fullscreened) {
        engine_toggle_fullscreen();
    }

    if(!engine_sprite_textures_init()) {
        return false;
    }

    return true;
}

void engine_quit() {
    engine_sprite_textures_quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}

bool engine_sprite_textures_init() {
    for(int i = 0; i < SPRITE_COUNT; i++) {
        SDL_Surface* loaded_surface = IMG_Load(sprite_data[i].path);
        if(loaded_surface == NULL) {
            std::cout << "Unable to load texture image! SDL Error: " << IMG_GetError() << std::endl;
            return false;
        }

        sprite_texture[i] = SDL_CreateTextureFromSurface(renderer, loaded_surface);
        if(sprite_texture[i] == NULL) {
            std::cout << "Unable to create sprite texture! SDL Error: " << SDL_GetError() << std::endl;
            return false;
        }

        sprite_texture_width[i] = loaded_surface->w;
        sprite_texture_height[i] = loaded_surface->h;
        sprite_frame_count[i] = loaded_surface->w / sprite_data[i].frame_size[0];

        SDL_FreeSurface(loaded_surface);
    }

    return true;
}

void engine_sprite_textures_quit() {
    for(int i = 0; i < SPRITE_COUNT; i++) {
        SDL_DestroyTexture(sprite_texture[i]);
    }
}

void engine_set_resolution(int width, int height) {
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowSize(window, width, height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void engine_toggle_fullscreen() {
    if (engine_is_fullscreen){
        SDL_SetWindowFullscreen(window, 0);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    engine_is_fullscreen = !engine_is_fullscreen;
}

void engine_clock_tick() {
    frames++;
    float current_time = SDL_GetTicks() / 1000.0f;

    // If one second has passed, record what the fps and dps was during that second
    if(current_time - last_second_time >= 1.0f) {
        fps = frames;
        dps = deltas;
        frames = 0;
        deltas = 0;
        last_second_time += 1.0;
    }

    // Record the delta time for consistent game logi
    delta = current_time - last_update_time;
    deltas += delta;
    last_update_time = current_time;

    // Delay if there's extra time between frames
    if(current_time - last_frame_time < FRAME_DURATION) {
        unsigned long delay_time = (unsigned long)(1000.0f * (FRAME_DURATION - (current_time - last_frame_time)));
        SDL_Delay(delay_time);
    }
}

// Rendering functions

void engine_render_clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void engine_render_present() {
    SDL_RenderPresent(renderer);
}

void engine_render_text(const char* text, int x, int y) {
    int index = 0;
    while(text[index] != '\0') {
        int letter_index = (int)(text[index] - ' ');
        engine_render_sprite_frame(SPRITE_FONT, letter_index, x + (index * 8), y, false);
        index++;
    }
}

void engine_render_dialog(char* dialog_rows[2], size_t dialog_display_length) {
    const int width = SCREEN_WIDTH / 8;
    const int height = 4;
    const int base_x = 0;
    const int base_y = SCREEN_HEIGHT - (height * 8);
    const int row_length = width - 2;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            int frame = 4;
            if(x == 0) {
                frame--;
            } else if(x == width - 1) {
                frame++;
            }
            if(y == 0) {
                frame -= 3;
            } else if(y == height - 1) {
                frame += 3;
            }
            const int render_x = base_x + (x * 8);
            const int render_y = base_y + (y * 8);

            if(frame != 4) {
                engine_render_sprite_frame(SPRITE_UI_FRAME, frame, render_x, render_y, false);
            } else {
                const int row = y - 1;
                const int col = x - 1;
                const size_t index = (row * row_length) + col;
                if(index < dialog_display_length) {
                    engine_render_sprite_frame(SPRITE_FONT, (int)(dialog_rows[row][col] - ' '), render_x, render_y, false);
                } else {
                    engine_render_sprite_frame(SPRITE_FONT, 0, render_x, render_y, false);
                }
            }
        }
    }
}

void engine_render_sprite(Sprite sprite, int x, int y) {
    engine_render_sprite_frame(sprite, 0, x, y, false);
}

void engine_render_sprite_frame(Sprite sprite, int frame, int x, int y, bool flipped) {
    int frame_long_x = sprite_data[sprite].frame_size[0] * frame;
    SDL_Rect source_rect = (SDL_Rect) {
        .x = frame_long_x % sprite_texture_width[sprite],
        .y = ((int)(frame_long_x / sprite_texture_width[sprite])) * sprite_data[sprite].frame_size[1],
        .w = sprite_data[sprite].frame_size[0],
        .h = sprite_data[sprite].frame_size[1]
    };
    SDL_Rect dest_rect = (SDL_Rect) {
        .x = x,
        .y = y,
        .w = source_rect.w,
        .h = source_rect.h
    };

    SDL_RendererFlip flip;
    if(flipped) {
        flip = SDL_FLIP_HORIZONTAL;
    } else {
        flip = SDL_FLIP_NONE;
    }

    SDL_RenderCopyEx(renderer, sprite_texture[sprite], &source_rect, &dest_rect, 0, NULL, flip);
}

void engine_render_animation(Animation animation, int x, int y) {
    engine_render_sprite_frame(animation.sprite, animation.frame, x, y, false);
}

void engine_render_actor_animation(Animation animation, int direction, int x, int y) {
    int frame = animation.frame;
    if(direction == 0) {
        frame += sprite_frame_count[animation.sprite];
    } else if(direction == 1 || direction == 3) {
        frame += sprite_frame_count[animation.sprite] * 2;
    }

    engine_render_sprite_frame(animation.sprite, frame, x, y, direction == 3);
}
