#include "world.hpp"

#include <cmath>
#include <iostream>

const int INPUT_DIRECTION_KEYMAP[4] = { SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT };

const int PLAYER_ACTOR = 0;
const vec2 directions[4] = {
    vec2(0, -1),
    vec2(1, 0),
    vec2(0, 1),
    vec2(-1, 0),
};

// World init functions

World::World() {
    input_player_direction = -1;
    for(int i = 0; i < 4; i++) {
        input_direction_held[i] = false;
    }

    map.resize(20, 18);

    for(int y = 0; y < map.height; y++) {
        for(int x = 0; x < map.width; x++) {
            if(x == 2) {
                map.set_tile(vec2(x, y), 1);
                map.set_wall(vec2(x, y), true);
            } else {
                map.set_tile(vec2(x, y), 0);
                map.set_wall(vec2(x, y), false);
            }
        }
    }

    map.load_from_file("./world.map");

    actor_count = 0;
    actor_init(SPRITE_PLAYER, 5, 2);

    npc_count = 0;
    PathNode path[2] = {
        (PathNode) {
            .position = vec2(3, 6),
            .wait_time = 120,
            .wait_direction = 2
        },
        (PathNode) {
            .position = vec2(6, 6),
            .wait_time = 120,
            .wait_direction = 2
        }
    };
    npc_init(SPRITE_PLAYER, 6, 6, (char*)"I'm looking for wild mushrooms!", path, 2);

    npc_being_talked_to = -1;
}

World::~World() {
}

// World input functions

void World::handle_input(SDL_Event e) {
    if(e.type == SDL_KEYDOWN) {
        int key = e.key.keysym.sym;

        if(key == SDLK_x) {

            if(ui.dialog_is_open) {
                ui.dialog_progress();

                if(!ui.dialog_is_open) {
                    npc_being_talked_to = -1;
                }
            } else {
                player_interact();
            }

            return;
        }

        for(int i = 0; i < 4; i++) {

            if(key == INPUT_DIRECTION_KEYMAP[i]) {
                input_direction_held[i] = true;
                input_player_direction = i;
                return;
            }
        }
    } else if(e.type == SDL_KEYUP) {
        int key = e.key.keysym.sym;

        for(int i = 0; i < 4; i++) {

            if(key == INPUT_DIRECTION_KEYMAP[i]) {
                input_direction_held[i] = false;
                input_player_direction = -1;

                for(int j = 0; j < 4; j++) {
                    if(input_direction_held[j]) {
                        input_player_direction = j;
                        break;
                    }
                }

                return;
            }
        }
    }
}

// World update functions

void World::update() {
    player_move();

    for(int i = 0; i < npc_count; i++) {
        if(npc_being_talked_to == i) {
            continue;
        }
        npc_move(npcs[i]);
    }

    ui.update();
}

// World render functions

void World::render(Engine* engine) {
    map.render(engine);

    // Render actors
    for(int i = 0; i < actor_count; i++) {
        vec2 render_pos = actors[i].position - map.camera_position;
        engine->render_actor_animation(actors[i].animation, actors[i].facing_direction, render_pos.x, render_pos.y);
    }

    // Render UI
    if(ui.dialog_is_open) {
        engine->render_dialog(ui.dialog_rows, ui.dialog_display_length);
    }
}

// Map functions

bool World::is_tile_free(const vec2& tile) const {
    if(!map.in_bounds(tile)) {
        return false;
    }

    for(int i = 0; i < actor_count; i++) {
        if(actors[i].target.is_null()) {
            vec2 position_tile = tile_at(actors[i].position);
            if(position_tile.equals(tile)) {
                return false;
            }
        } else {
            int actor_direction = actors[i].position.direction_to(actors[i].target);
            vec2 target_tile = tile_at(actors[i].target);
            vec2 last_tile = target_tile - directions[actor_direction];
            if(target_tile.equals(tile) || last_tile.equals(tile)) {
                return false;
            }
        }
    }

    return !map.get_wall(tile);
}

// Player functions

void World::player_move() {
    Actor& player_actor = actors[PLAYER_ACTOR];
    actor_move(player_actor);
    map.camera_position = player_actor.position - vec2(Engine::TILE_SIZE * 4, Engine::TILE_SIZE * 4);

    if(player_actor.target.is_null() && input_player_direction != -1) {
        vec2 next_tile = tile_at(player_actor.position) + directions[input_player_direction];
        if(is_tile_free(next_tile)) {
            player_actor.target = position_of(next_tile);
        } else {
            player_actor.facing_direction = input_player_direction;
        }
    }
}

void World::player_interact() {
    Actor& player_actor = actors[PLAYER_ACTOR];
    if(!player_actor.target.is_null()) {
        return;
    }

    vec2 interact_target = position_of(tile_at(player_actor.position) + directions[player_actor.facing_direction]);

    if(!map.in_bounds(tile_at(interact_target))) {
        return;
    }

    for(int i = 0; i < npc_count; i++) {
        if(!actors[npcs[i].actor].target.is_null()) {
            continue;
        }
        if(interact_target.equals(actors[npcs[i].actor].position)) {
            actors[npcs[i].actor].facing_direction = direction_opposite_of(player_actor.facing_direction);
            npc_being_talked_to = i;
            ui.dialog_open(npcs[i].dialog);
            return;
        }
    }
}

// Actor functions

int World::actor_init(Sprite sprite, int x, int y) {
    if(actor_count == MAX_ACTORS) {
        std::cout << "Cannot create new actor! Max actor count has been reached!" << std::endl;
        return -1;
    }

    int actor_index = actor_count;
    actors[actor_index] = (Actor) {
        .facing_direction = 2,
        .position = position_of(vec2(x, y)),
        .target = vec2_null()
    };
    actors[actor_index].animation.init(sprite, 10);
    actor_count++;

    return actor_index;
}

void World::actor_move(Actor& actor) {
    if(actor.target.is_null()) {
        actor.animation.reset();
        return;
    }

    int move_direction = actor.position.direction_to(actor.target);
    vec2 step = directions[move_direction];
    actor.position = actor.position + step;

    if(actor.position.equals(actor.target)) {
        actor.target = vec2_null();
    }

    actor.facing_direction = move_direction;
    actor.animation.update();
}

// NPC functions

int World::npc_init(Sprite sprite, int x, int y, char* dialog, const PathNode* path, int path_length) {
    if(npc_count == MAX_NPCS) {
        std::cout << "Cannot create new NPC! Max NPC count has been reached!" << std::endl;
        return -1;
    }

    int npc_index = npc_count;
    npcs[npc_index].actor = actor_init(sprite, x, y);
    npcs[npc_index].dialog = dialog;

    npcs[npc_index].path_index = 0;
    npcs[npc_index].path_length = path_length;
    npcs[npc_index].path_timer = 0;

    for(int i = 0; i < path_length; i++) {
        npcs[npc_index].path[i] = path[i];
        npcs[npc_index].path[i].position = position_of(npcs[npc_index].path[i].position);
    }
    npc_count++;

    return npc_index;
}

void World::npc_move(NPC& npc) {
    // Don't move if this npc never had a path
    if(npc.path_length == 0) {
        return;
    }

    Actor& npc_actor = actors[npc.actor];

    // Don't move if this npc is waiting on the current path node
    if(npc.path_timer > 0) {
        npc_actor.facing_direction = npc.path[npc.path_index].wait_direction;
        npc.path_timer--;
        return;
    }

    // If we've decided to move, check to make sure we have a target to move towards
    if(npc_actor.target.is_null()) {
        int target_direction = npc_actor.position.direction_to(npc.path[npc.path_index].position);
        vec2 next_tile = tile_at(npc_actor.position) + directions[target_direction];
        if(is_tile_free(next_tile)) {
            npc_actor.target = position_of(next_tile);
        }
    }

    // Now move the actor towards that arget
    actor_move(npc_actor);

    // If after movement we've reached our target path node, increment the path and start the wait timer if there is one
    if(npc_actor.target.is_null() && npc_actor.position.equals(npc.path[npc.path_index].position)) {
        npc.path_timer = npc.path[npc.path_index].wait_time;
        npc_actor.animation.reset();

        npc.path_index++;
        if(npc.path_index == npc.path_length) {
            npc.path_index -= 2;
        }
    }
}
