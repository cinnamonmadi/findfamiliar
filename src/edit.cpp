#include "edit.hpp"

#include <vector>
#include <iostream>

Edit::Edit() {
    tool = TOOL_DRAW;
    mouse_pos = vec2(0, 0);
    panning = false;
    drawing = false;

    selected_tile = 0;
    tileset_camera_pos = vec2(0, 0);

    command = "";
    typing = false;
}

Edit::~Edit() {

}

void Edit::handle_input(SDL_Event e) {
    if(e.type == SDL_MOUSEMOTION) {
        mouse_pos = vec2(e.motion.x, e.motion.y);

        vec2 mouse_motion = vec2(e.motion.xrel, e.motion.yrel);
        if(panning) {
            if(tool == TOOL_SELECT_TILE) {
                tileset_camera_pos = tileset_camera_pos - mouse_motion;
            } else {
                map.camera_position = map.camera_position - mouse_motion;
            }
        }
    } else if(e.type == SDL_MOUSEBUTTONDOWN) {
        if(e.button.button == SDL_BUTTON_RIGHT) {
            panning = true;
        } else if(!panning && e.button.button == SDL_BUTTON_LEFT) {
            if(tool == TOOL_SELECT_TILE) {
                handle_select_tile();
            } else if(tool == TOOL_DRAW) {
                drawing = true;
            } else if(tool == TOOL_WALL) {
                handle_toggle_wall();
            }
        }
    } else if(e.type == SDL_MOUSEBUTTONUP) {
        if(e.button.button == SDL_BUTTON_RIGHT) {
            panning = false;
        } else if(e.button.button == SDL_BUTTON_LEFT) {
            drawing = false;
        }
    } else if(e.type == SDL_KEYDOWN) {
        int key = e.key.keysym.sym;

        if(typing &&
            ((key >= SDLK_a && key <= SDLK_z) || key == SDLK_SPACE || (key >= SDLK_0 && key <= SDLK_9) || key == SDLK_PERIOD)) {
            command.push_back((char)key);
            return;
        }

        if(typing && command.length() != 0 && key == SDLK_BACKSPACE) {
            command.pop_back();
            return;
        }

        if(typing && command.length() != 0 && key == SDLK_RETURN) {
            handle_command();
        }

        drawing = false;
        switch(key) {
            case SDLK_b:
                tool = TOOL_DRAW;
                break;
            case SDLK_t:
                tool = TOOL_SELECT_TILE;
                break;
            case SDLK_w:
                tool = TOOL_WALL;
                break;
            case SDLK_BACKQUOTE:
                typing = !typing;
                command = "";
                break;
        }
    }
}

void Edit::handle_command() {
    std::vector<std::string> command_parts;
    while(command.length() != 0) {
        size_t space_index = command.find(' ');
        if(space_index == std::string::npos) {
            command_parts.push_back(command);
            command = "";
        } else {
            command_parts.push_back(command.substr(0, space_index));
            command = command.substr(space_index + 1);
        }
    }

    if(command_parts.at(0) == "resize" && command_parts.size() == 3) {
        int new_width = atoi(command_parts.at(1).c_str());
        int new_height = atoi(command_parts.at(2).c_str());
        map.resize(new_width, new_height);

    } else if(command_parts.at(0) == "save" && command_parts.size() == 2) {
        map.save_to_file(command_parts.at(1).c_str());
    } else if(command_parts.at(0) == "load" && command_parts.size() == 2) {
        map.load_from_file(command_parts.at(1).c_str());
    }

    command = "";
    typing = false;
}

void Edit::update() {
    if(!panning && drawing) {
        handle_draw_tile();
    }
}

void Edit::render(Engine* engine) {
    if(tool == TOOL_SELECT_TILE) {
        engine->render_sprite(SPRITE_TILES, -tileset_camera_pos.x, -tileset_camera_pos.y);

        vec2 selected_tile_pos = (vec2((Engine::TILE_SIZE * selected_tile) % sprite_texture_width[SPRITE_TILES],
                                      (Engine::TILE_SIZE * selected_tile) / sprite_texture_width[SPRITE_TILES]));
        selected_tile_pos = selected_tile_pos - tileset_camera_pos;
        SDL_Rect tile_rect = (SDL_Rect) {
            .x = selected_tile_pos.x,
            .y = selected_tile_pos.y,
            .w = Engine::TILE_SIZE,
            .h = Engine::TILE_SIZE
        };
        SDL_SetRenderDrawColor(engine->renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(engine->renderer, &tile_rect);
    } else {
        map.render_with_walls(engine, tool == TOOL_WALL);

        if(tool == TOOL_DRAW || tool == TOOL_WALL) {
            vec2 mouse_tile = tile_at(mouse_pos + map.camera_position);
            if(map.in_bounds(mouse_tile)) {
                vec2 preview_pos = position_of(mouse_tile) - map.camera_position;
                if(tool == TOOL_DRAW) {
                    engine->render_sprite_frame(SPRITE_TILES, selected_tile, preview_pos.x, preview_pos.y, false);
                } else {
                    SDL_RenderDrawLine(engine->renderer, preview_pos.x, preview_pos.y, preview_pos.x + Engine::TILE_SIZE, preview_pos.y + Engine::TILE_SIZE);
                    SDL_RenderDrawLine(engine->renderer, preview_pos.x, preview_pos.y + Engine::TILE_SIZE, preview_pos.x + Engine::TILE_SIZE, preview_pos.y);
                }
            }
        }
    }

    if(typing) {
        engine->render_text(command.c_str(), 0, Engine::SCREEN_HEIGHT - 8);
    }
}

void Edit::handle_select_tile() {
    vec2 attempt_select_pos = mouse_pos + tileset_camera_pos;
    if(attempt_select_pos.x < 0 || attempt_select_pos.x >= sprite_texture_width[SPRITE_TILES] ||
       attempt_select_pos.y < 0 || attempt_select_pos.y >= sprite_texture_height[SPRITE_TILES]) {
        return;
    }

    vec2 attempt_select_tile = tile_at(attempt_select_pos);
    int attempt_long_x = attempt_select_tile.x + (attempt_select_tile.y * (sprite_texture_height[SPRITE_TILES] / Engine::TILE_SIZE));
    selected_tile = attempt_long_x;
}

void Edit::handle_draw_tile() {
    vec2 attempt_draw_tile = tile_at(mouse_pos + map.camera_position);
    if(map.in_bounds(attempt_draw_tile)) {
        map.set_tile(attempt_draw_tile, selected_tile);
    }
}

void Edit::handle_toggle_wall() {
    vec2 attempt_wall_tile = tile_at(mouse_pos + map.camera_position);
    if(map.in_bounds(attempt_wall_tile)) {
        map.set_wall(attempt_wall_tile, !map.get_wall(attempt_wall_tile));
    }
}
