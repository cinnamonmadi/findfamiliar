#pragma once

#include "sprite.hpp"
#include <cmath>

typedef struct vec2 {
    int x;
    int y;
    vec2() {
        vec2(0, 0);
    }
    vec2(int x, int y) {
        this->x = x;
        this->y = y;
    }
    vec2 operator+(const vec2& other) const {
        return vec2(x + other.x, y + other.y);
    }
    vec2 operator-(const vec2& other) const {
        return vec2(x - other.x, y - other.y);
    }
    vec2 operator*(const float& scaler) const {
        return vec2((int)(x * scaler), (int)(y * scaler));
    }
    int direction_to(const vec2& target) const {
        if(y > target.y) {
            return 0;
        } else if(x < target.x) {
            return 1;
        } else if(y < target.y) {
            return 2;
        } else if(x > target.x) {
            return 3;
        }
        return -1;
    }
    int manhatten_distance_to(const vec2& target) const {
        return abs(target.x - x) + abs(target.y - y);
    }
    int manhatten_length() const {
        return abs(x) + abs(y);
    }
    bool equals(const vec2& other) const {
        return x == other.x && y == other.y;
    }
} vec2;

typedef struct Actor {
    Sprite sprite;
    vec2 position;
    vec2 target;
} Actor;

class Map {
    public:
        int** tiles;
        bool** walls;
        int tile_width;
        int tile_height;

        Actor player;

        Map();
        ~Map();

        bool is_tile_free(const vec2& tile) const;

        void update(float delta, int player_input_direction);

        void actor_init(Actor* actor, Sprite sprite, int x, int y);
        void actor_move_in_direction(Actor& actor, int input_direction, float delta);

    private:
};

vec2 tile_at(vec2 point);
vec2 position_of(vec2 point);
