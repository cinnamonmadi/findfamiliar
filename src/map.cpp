#include "map.hpp"

#include <cmath>
#include <iostream>

const int PLAYER_SPEED = 100;
const vec2 directions[4] = {
    vec2(0, -1),
    vec2(1, 0),
    vec2(0, 1),
    vec2(-1, 0),
};

const vec2 VEC2_ZERO = vec2(0, 0);
const vec2 VEC2_NULL = vec2(-1, -1);

Map::Map() {
    tile_width = 10;
    tile_height = 9;

    tiles = new int*[tile_height];
    walls = new bool*[tile_height];
    for(int y = 0; y < tile_height; y++) {
        tiles[y] = new int[tile_width];
        walls[y] = new bool[tile_width];
        for(int x = 0; x < tile_width; x++) {
            tiles[y][x] = 0;
            walls[y][x] = false;
            if(x == 2) {
                tiles[y][x] = 1;
                walls[y][x] = true;
            }
        }
    }

    actor_init(&player, SPRITE_PLAYER, 5, 2);
}

Map::~Map() {
    for(int y = 0; y < tile_height; y++) {
        delete [] tiles[y];
        delete [] walls[y];
    }
    delete [] tiles;
    delete [] walls;
}

bool Map::is_tile_free(const vec2& tile) const {
    if(tile.x < 0 || tile.x >= tile_width || tile.y < 0 || tile.y >= tile_height) {
        return false;
    }
    return !walls[tile.y][tile.x];
}

void Map::update(float delta, int player_input_direction) {
    actor_move_in_direction(player, player_input_direction, delta);
}

void Map::actor_init(Actor* actor, Sprite sprite, int x, int y) {
    *actor = (Actor) {
        .sprite = sprite,
        .position = position_of(vec2(x, y)),
        .target = vec2(-1, -1)
    };
}

void Map::actor_move_in_direction(Actor& actor, int input_direction, float delta) {
    if(!actor.target.equals(VEC2_NULL)) {
        int move_direction = actor.position.direction_to(actor.target);
        vec2 step = directions[move_direction] * (PLAYER_SPEED * delta);

        bool reached_target = step.manhatten_length() >= actor.position.manhatten_distance_to(actor.target);
        if(!reached_target || input_direction != move_direction) {
            actor.position = actor.position + step;
        } else {
            actor.position = actor.target;
        }

        if(reached_target) {
            if(input_direction == -1) {
                actor.target = VEC2_NULL;
            } else {
                vec2 next_target_tile = tile_at(actor.target) + directions[input_direction];
                if(is_tile_free(next_target_tile)) {
                    actor.target = position_of(next_target_tile);
                }
            }
        }
    } else {
        if(input_direction != -1) {
            vec2 next_target_tile = tile_at(actor.position) + directions[input_direction];
            if(is_tile_free(next_target_tile)) {
                actor.target = position_of(next_target_tile);
            }
        }
    }
}

vec2 tile_at(vec2 point) {
    return vec2((int)(point.x / TILE_SIZE), (int)(point.y / TILE_SIZE));
}

vec2 position_of(vec2 tile) {
    return vec2(tile.x * TILE_SIZE, tile.y * TILE_SIZE);
}
