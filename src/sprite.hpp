#pragma once

typedef enum Sprite {
    SPRITE_TILES,
    SPRITE_PLAYER,
    SPRITE_COUNT
} Sprite;

typedef struct SpriteData {
    const char* path;
    const int frame_size[2];
    int frame_count;
    const int fps;
} SpriteData;

typedef struct Animation {
    Sprite sprite;
    int frame;
    float timer;
    float frame_duration;

    Animation(Sprite sprite);
    void update(float delta);
} Animation;

extern SpriteData sprite_data[SPRITE_COUNT];
extern const int TILE_SIZE;
