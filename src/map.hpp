#pragma once

#include "sprite.hpp"
#include "vector.hpp"

typedef struct Actor {
    Animation animation;
    int facing_direction;
    vec2 position;
    vec2 target;
} Actor;

typedef struct NPC {
    int actor;
    int path_index;
    int path_length;
    vec2 path[8];
} NPC;

static const int MAX_ACTORS = 32;
static const int MAX_NPCS = 31;

class Map {
    public:
        int** tiles;
        bool** walls;
        int tile_width;
        int tile_height;

        vec2 camera_position;
        vec2 camera_max_position;

        Actor actors[MAX_ACTORS];
        int actor_count;

        NPC npcs[MAX_NPCS];
        int npc_count;

        Map();
        ~Map();

        bool is_tile_free(const vec2& tile) const;

        void update(int player_input_direction);
        void update_move_player(int player_input_direction);
        void update_camera();

        int actor_init(Sprite sprite, int x, int y);
        void actor_move(Actor& actor);

        int npc_init(Sprite sprite, int x, int y, const vec2* path, int path_length);
        void npc_move(NPC& npc);

    private:
};

vec2 tile_at(vec2 point);
vec2 position_of(vec2 point);
bool intersects_tile(const vec2& point, const vec2& tile);
