#pragma once

#include "state.hpp"
#include "engine.hpp"
#include "map.hpp"
#include "ui.hpp"
#include "vector.hpp"
#include <SDL2/SDL.h>

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

class World : public State {
    public:
        World();
        ~World() override;

        void handle_input(SDL_Event e) override;
        void update() override;
        void render(Engine* engine) override;
    private:
        int input_player_direction;
        bool input_direction_held[4];

        UI ui;
        Map map;

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
