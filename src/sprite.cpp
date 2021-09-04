#include "sprite.hpp"

const int TILE_SIZE = 16;

SpriteData sprite_data[SPRITE_COUNT] = {
    (SpriteData) {
        .path = "./res/gfx/tiles.png",
        .frame_size = { TILE_SIZE, TILE_SIZE },
        .frame_count = 0,
        .fps = 1
    },
    (SpriteData) {
        .path = "./res/gfx/witch.png",
        .frame_size = { TILE_SIZE, TILE_SIZE },
        .frame_count = 0,
        .fps = 1
    }
};

Animation::Animation(Sprite sprite) {
    this->sprite = sprite;
    frame = 0;
    timer = 0;
    frame_duration = 1.0f / sprite_data[sprite].fps;
}

void Animation::update(float delta) {
    timer += delta;

    if(timer >= frame_duration) {
        frame++;
        timer -= frame_duration;

        if(frame == sprite_data[sprite].frame_count) {
            frame = 0;
        }
    }
}
