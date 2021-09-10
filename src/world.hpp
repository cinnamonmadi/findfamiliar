#pragma once

#include <SDL2/SDL.h>
#include "engine.hpp"
#include "ui.hpp"
#include "vector.hpp"

typedef struct Map {
} Map;

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

class World {
    public:
        World(Engine* engine);
        ~World();

        void handle_input(SDL_Event e);
        void update();
        void render();
    private:
        Engine* engine;

        int input_player_direction;
        bool input_direction_held[4];

        UI ui;

        int** map_tiles;
        bool** map_walls;
        int map_width;
        int map_height;

        vec2 camera_position;

        Actor actors[MAX_ACTORS];
        int actor_count;
        NPC npcs[MAX_NPCS];
        int npc_count;
        int npc_being_talked_to;

        bool is_tile_free(const vec2& tile) const;

        void player_move();
        void player_interact();

        int actor_init(Sprite sprite, int x, int y);
        void actor_move(Actor& actor);

        int npc_init(Sprite sprite, int x, int y, char* dialog, const PathNode* path, int path_length);
        void npc_move(NPC& npc);
};

inline vec2 tile_at(vec2 point) {
    return vec2((int)(point.x / Engine::TILE_SIZE), (int)(point.y / Engine::TILE_SIZE));
}

inline vec2 position_of(vec2 tile) {
    return vec2(tile.x * Engine::TILE_SIZE, tile.y * Engine::TILE_SIZE);
}

inline bool intersects_tile(const vec2& point, const vec2& tile) {
    vec2 rect_size = vec2(Engine::TILE_SIZE, Engine::TILE_SIZE);
    vec2 tile_pos = position_of(tile);
    return !(point.x + rect_size.x <= tile_pos.x ||
             tile_pos.x + rect_size.x <= point.x ||
             point.y + rect_size.y <= tile_pos.y ||
             tile_pos.y + rect_size.y <= point.y);
}

inline int direction_opposite_of(int direction) {
    return (direction + 2) % 4;
}
