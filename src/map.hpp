#pragma once

#include "sprite.hpp"
#include "vector.hpp"

typedef struct Actor {
    Animation animation;
    int facing_direction;
    vec2 position;
    vec2 target;
} Actor;

typedef struct PathNode {
    vec2 position;
    int wait_time;
    int wait_direction;
} PathNode;

typedef struct NPC {
    int actor;
    int path_index;
    int path_length;
    int path_timer;
    PathNode path[8];
    char* dialog;
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

        int npc_being_talked_to;

        Map();
        ~Map();

        bool is_tile_free(const vec2& tile) const;

        void update(int player_input_direction);
        void update_move_player(int player_input_direction);
        void update_camera();

        char* player_interact();

        int actor_init(Sprite sprite, int x, int y);
        void actor_move(Actor& actor);

        int npc_init(Sprite sprite, int x, int y, char* dialog, const PathNode* path, int path_length);
        void npc_move(NPC& npc);

    private:
};

inline vec2 tile_at(vec2 point) {
    return vec2((int)(point.x / TILE_SIZE), (int)(point.y / TILE_SIZE));
}

inline vec2 position_of(vec2 tile) {
    return vec2(tile.x * TILE_SIZE, tile.y * TILE_SIZE);
}

inline bool intersects_tile(const vec2& point, const vec2& tile) {
    vec2 rect_size = vec2(TILE_SIZE, TILE_SIZE);
    vec2 tile_pos = position_of(tile);
    return !(point.x + rect_size.x <= tile_pos.x ||
             tile_pos.x + rect_size.x <= point.x ||
             point.y + rect_size.y <= tile_pos.y ||
             tile_pos.y + rect_size.y <= point.y);
}

inline int direction_opposite_of(int direction) {
    return (direction + 2) % 4;
}
