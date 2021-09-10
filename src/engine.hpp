#pragma once

#include <SDL2/SDL.h>

typedef enum Sprite {
    SPRITE_FONT,
    SPRITE_UI_FRAME,
    SPRITE_TILES,
    SPRITE_PLAYER,
    SPRITE_COUNT
} Sprite;

typedef struct Animation {
    Sprite sprite;
    int frame;
    int timer;
    int frame_duration;
} Animation;

class Engine {
    public:
        static const int SCREEN_WIDTH = 160;
        static const int SCREEN_HEIGHT = 144;
        static const int TILE_SIZE = 16;

        int fps = 0;

        SDL_Window* window;
        SDL_Renderer* renderer;

        bool init(int resolution_width, int resolution_height, bool init_fullscreened);
        void quit();
        void set_resolution(int width, int height);
        void toggle_fullscreen();
        void clock_tick();

        Animation animation_init(Sprite sprite, int frame_duration) const;
        void animation_update(Animation& animation) const;
        void animation_reset(Animation& animation) const;

        void render_clear();
        void render_present();

        void render_text(const char* text, int x, int y);
        void render_dialog(char* dialog_rows[2], size_t dialog_display_length);

        void render_sprite(Sprite sprite, int x, int y);
        void render_sprite_frame(Sprite sprite, int frame, int x, int y, bool flipped);
        void render_animation(Animation animation, int x, int y);
        void render_actor_animation(Animation animation, int direction, int x, int y);
    private:
        bool is_fullscreen = false;

        float last_frame_time = 0.0f;
        float last_update_time = 0.0f;
        float last_second_time = 0.0f;
        int frames = 0;
        float deltas = 0.0f;
        float delta = 0.0f;
        float dps = 0.0f;

        SDL_Texture* sprite_texture[SPRITE_COUNT];
        int sprite_texture_width[SPRITE_COUNT];
        int sprite_texture_height[SPRITE_COUNT];
        int sprite_frame_count[SPRITE_COUNT];

        bool textures_init();
        void textures_free();
};
