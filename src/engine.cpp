#include "engine.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;
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

// Fonts
TTF_Font* debug_font;

bool engine_fonts_init();
void engine_fonts_quit();

// Engine init functions

bool engine_init(int argc, char** argv) {
    bool init_fullscreened = false;
    int resolution_width = SCREEN_WIDTH;
    int resolution_height = SCREEN_HEIGHT;

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

    if(TTF_Init() == -1){
        std::cout << "Unable to initialize SDL_ttf! SDL Error: " << TTF_GetError() << std::endl;
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

    if(!engine_fonts_init()) {
        return false;
    }

    return true;
}

bool engine_fonts_init() {
    debug_font = TTF_OpenFont("./res/hack.ttf", 10);
    if(debug_font == NULL) {
        std::cout << "Unable to initialize font! SDL Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

void engine_quit() {
    engine_fonts_quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void engine_fonts_quit() {
    TTF_CloseFont(debug_font);
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

void engine_render_text(const char* text, SDL_Color color, int x, int y) {
    SDL_Surface* text_surface = TTF_RenderText_Solid(debug_font, text, color);
    if(text_surface == NULL) {
        std::cout << "Unable to render text to surface! SDL Error " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if(text_texture == NULL) {
        std::cout << "Unable to create text texture! SDL Error " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect source_rect = (SDL_Rect){ .x = 0, .y = 0, .w = text_surface->w, .h = text_surface->h };
    SDL_Rect dest_rect = (SDL_Rect){ .x = x, .y = y, .w = text_surface->w, .h = text_surface->h };
    if(dest_rect.x == RENDER_POSITION_CENTERED) {
        dest_rect.x = (SCREEN_WIDTH / 2) - (source_rect.w / 2);
    }
    if(dest_rect.y == RENDER_POSITION_CENTERED) {
        dest_rect.y = (SCREEN_HEIGHT / 2) - (source_rect.h / 2);
    }

    SDL_RenderCopy(renderer, text_texture, &source_rect, &dest_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}
