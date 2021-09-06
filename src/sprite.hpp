#pragma once

typedef enum Sprite {
    SPRITE_FONT,
    SPRITE_UI_FRAME,
    SPRITE_TILES,
    SPRITE_PLAYER,
    SPRITE_COUNT
} Sprite;

typedef struct SpriteData {
    const char* path;
    const int frame_size[2];
} SpriteData;

typedef struct Animation {
    Sprite sprite;
    int frame;
    int timer;
    int frame_duration;

    Animation() {
        sprite = SPRITE_COUNT;
        frame = 0;
        timer = 0;
        frame_duration = 0;
    };
    Animation(Sprite sprite, int frame_duration);
    void update();
    void reset();
} Animation;

extern SpriteData sprite_data[SPRITE_COUNT];
extern int sprite_frame_count[SPRITE_COUNT];
extern const int TILE_SIZE;
