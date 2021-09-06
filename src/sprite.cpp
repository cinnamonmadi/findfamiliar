#include "sprite.hpp"

const int TILE_SIZE = 16;

SpriteData sprite_data[SPRITE_COUNT] = {
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
        .frame_size = { TILE_SIZE, TILE_SIZE },
    },
    (SpriteData) {
        .path = "./res/gfx/witch.png",
        .frame_size = { TILE_SIZE, TILE_SIZE },
    }
};
int sprite_frame_count[SPRITE_COUNT];

Animation::Animation(Sprite sprite, int frame_duration) {
    this->sprite = sprite;
    this->frame_duration = frame_duration;
    reset();
}

void Animation::update() {
    timer++;

    if(timer == frame_duration) {
        frame++;
        timer -= frame_duration;

        if(frame == sprite_frame_count[sprite]) {
            frame = 0;
        }
    }
}

void Animation::reset() {
    frame = 0;
    timer = 0;
}
