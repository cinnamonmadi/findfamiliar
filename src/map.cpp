#include "map.hpp"

#include "global.hpp"
#include <cmath>
#include <iostream>

const int PLAYER_ACTOR = 0;
const vec2 directions[4] = {
    vec2(0, -1),
    vec2(1, 0),
    vec2(0, 1),
    vec2(-1, 0),
};

const int CAMERA_BOUND_TOP = TILE_SIZE * 3;
const int CAMERA_BOUND_BOT = SCREEN_HEIGHT - CAMERA_BOUND_TOP;
const int CAMERA_BOUND_LEFT = TILE_SIZE * 3;
const int CAMERA_BOUND_RIGHT = SCREEN_WIDTH - CAMERA_BOUND_LEFT;

Map::Map() {
    tile_width = 20;
    tile_height = 18;

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

    camera_max_position = vec2((tile_width * TILE_SIZE) - SCREEN_WIDTH, (tile_height * TILE_SIZE) - SCREEN_HEIGHT);

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
    return !walls[tile.y][tile.x];
}

void Map::update(int player_input_direction) {
    update_move_player(player_input_direction);
    update_camera();

    for(int i = 0; i < npc_count; i++) {
        if(npc_being_talked_to == i) {
            continue;
        }
        npc_move(npcs[i]);
    }
}

void Map::update_move_player(int player_input_direction) {
    Actor& player_actor = actors[PLAYER_ACTOR];

    actor_move(player_actor);

    if(player_actor.target.is_null() && player_input_direction != -1) {
        vec2 next_tile = tile_at(player_actor.position) + directions[player_input_direction];
        if(is_tile_free(next_tile)) {
            player_actor.target = position_of(next_tile);
        } else {
            player_actor.facing_direction = player_input_direction;
        }
    }
}

void Map::update_camera() {
    vec2& player_position = actors[PLAYER_ACTOR].position;
    vec2 player_screen_position = player_position - camera_position;
    if(player_screen_position.y < CAMERA_BOUND_TOP) {
        camera_position.y = player_position.y - CAMERA_BOUND_TOP;
    } else if(player_screen_position.y > CAMERA_BOUND_BOT) {
        camera_position.y = player_position.y - CAMERA_BOUND_BOT;
    }
    if(player_screen_position.x < CAMERA_BOUND_LEFT) {
        camera_position.x = player_position.x - CAMERA_BOUND_LEFT;
    } else if(player_screen_position.x > CAMERA_BOUND_RIGHT) {
        camera_position.x = player_position.x - CAMERA_BOUND_RIGHT;
    }

    if(camera_position.x < 0) {
        camera_position.x = 0;
    } else if(camera_position.x > camera_max_position.x) {
        camera_position.x = camera_max_position.x;
    }
    if(camera_position.y < 0) {
        camera_position.y = 0;
    } else if(camera_position.y > camera_max_position.y) {
        camera_position.y = camera_max_position.y;
    }
}

char* Map::player_interact() {
    Actor& player_actor = actors[PLAYER_ACTOR];
    if(!player_actor.target.is_null()) {
        return NULL;
    }

    vec2 interact_target = position_of(tile_at(player_actor.position) + directions[player_actor.facing_direction]);

    if(interact_target.x < 0 || interact_target.x >= tile_width * TILE_SIZE ||
       interact_target.y < 0 || interact_target.y >= tile_height * TILE_SIZE) {
        return NULL;
    }

    for(int i = 0; i < npc_count; i++) {
        if(!actors[npcs[i].actor].target.is_null()) {
            continue;
        }
        if(interact_target.equals(actors[npcs[i].actor].position)) {
            actors[npcs[i].actor].facing_direction = direction_opposite_of(player_actor.facing_direction);
            npc_being_talked_to = i;
            return npcs[i].dialog;
        }
    }

    return NULL;
}

int Map::actor_init(Sprite sprite, int x, int y) {
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
    actors[actor_index].animation = Animation(sprite, 10);
    actor_count++;

    return actor_index;
}

void Map::actor_move(Actor& actor) {
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

int Map::npc_init(Sprite sprite, int x, int y, char* dialog, const PathNode* path, int path_length) {
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

void Map::npc_move(NPC& npc) {
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
