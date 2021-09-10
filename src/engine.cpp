#include "engine.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>

const float FRAME_DURATION = 1.0f / 60.0f;

// Sprite data
typedef struct SpriteData {
    const char* path;
    const int frame_size[2];
} SpriteData;

const SpriteData sprite_data[SPRITE_COUNT] = {
    (SpriteData) {
        .path = "./res/gfx/font.png",
        .frame_size = { 8, 8 },
    },
    (SpriteData) {
        .path = "./res/gfx/frame.png",
        .frame_size = { 8, 8 },
    },
    (SpriteData) {
        .path = "./res/gfx/tiles.png",
        .frame_size = { Engine::TILE_SIZE, Engine::TILE_SIZE },
    },
    (SpriteData) {
        .path = "./res/gfx/witch.png",
        .frame_size = { Engine::TILE_SIZE, Engine::TILE_SIZE },
    }
};

// Engine init functions

bool Engine::init(int resolution_width, int resolution_height, bool init_fullscreened) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Unable to initialize SDL! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Find Familiar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

    set_resolution(resolution_width, resolution_height);
    if(init_fullscreened) {
        toggle_fullscreen();
    }

    if(!textures_init()) {
        return false;
    }

    return true;
}

void Engine::quit() {
    textures_free();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}

bool Engine::textures_init() {
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

void Engine::textures_free() {
    for(int i = 0; i < SPRITE_COUNT; i++) {
        SDL_DestroyTexture(sprite_texture[i]);
    }
}

void Engine::set_resolution(int width, int height) {
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowSize(window, width, height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Engine::toggle_fullscreen() {
    if (is_fullscreen){
        SDL_SetWindowFullscreen(window, 0);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    is_fullscreen = !is_fullscreen;
}

void Engine::clock_tick() {
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

    // Record the delta time for consistent game logic
    delta = current_time - last_update_time;
    deltas += delta;
    last_update_time = current_time;

    // Delay if there's extra time between frames
    if(current_time - last_frame_time < FRAME_DURATION) {
        unsigned long delay_time = (unsigned long)(1000.0f * (FRAME_DURATION - (current_time - last_frame_time)));
        SDL_Delay(delay_time);
    }
}

// Animation functions

Animation Engine::animation_init(Sprite sprite, int frame_duration) const {
    return (Animation) {
        .sprite = sprite,
        .frame = 0,
        .timer = 0,
        .frame_duration = frame_duration
    };
}

void Engine::animation_update(Animation& animation) const {
    animation.timer++;

    if(animation.timer == animation.frame_duration) {
        animation.frame++;
        animation.timer -= animation.frame_duration;

        if(animation.frame == sprite_frame_count[animation.sprite]) {
            animation.frame = 0;
        }
    }
}

void Engine::animation_reset(Animation& animation) const {
    animation.frame = 0;
    animation.timer = 0;
}

// Rendering functions

void Engine::render_clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Engine::render_present() {
    SDL_RenderPresent(renderer);
}

void Engine::render_text(const char* text, int x, int y) {
    int index = 0;
    while(text[index] != '\0') {
        int letter_index = (int)(text[index] - ' ');
        render_sprite_frame(SPRITE_FONT, letter_index, x + (index * 8), y, false);
        index++;
    }
}

void Engine::render_dialog(char* dialog_rows[2], size_t dialog_display_length) {
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
                render_sprite_frame(SPRITE_UI_FRAME, frame, render_x, render_y, false);
            } else {
                const int row = y - 1;
                const int col = x - 1;
                const size_t index = (row * row_length) + col;
                if(index < dialog_display_length) {
                    render_sprite_frame(SPRITE_FONT, (int)(dialog_rows[row][col] - ' '), render_x, render_y, false);
                } else {
                    render_sprite_frame(SPRITE_FONT, 0, render_x, render_y, false);
                }
            }
        }
    }
}

void Engine::render_sprite(Sprite sprite, int x, int y) {
    render_sprite_frame(sprite, 0, x, y, false);
}

void Engine::render_sprite_frame(Sprite sprite, int frame, int x, int y, bool flipped) {
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

void Engine::render_animation(Animation animation, int x, int y) {
    render_sprite_frame(animation.sprite, animation.frame, x, y, false);
}

void Engine::render_actor_animation(Animation animation, int direction, int x, int y) {
    int frame = animation.frame;
    if(direction == 0) {
        frame += sprite_frame_count[animation.sprite];
    } else if(direction == 1 || direction == 3) {
        frame += sprite_frame_count[animation.sprite] * 2;
    }

    render_sprite_frame(animation.sprite, frame, x, y, direction == 3);
}
