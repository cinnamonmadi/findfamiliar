#pragma once

#include "state.hpp"
#include "engine.hpp"
#include "map.hpp"
#include <SDL2/SDL.h>
#include <string>

typedef enum EditTool {
    TOOL_DRAW,
    TOOL_SELECT_TILE
} EditTool;

class Edit : public State {
    public:
        Edit();
        ~Edit() override;

        void handle_input(SDL_Event e) override;
        void update() override;
        void render(Engine* engine) override;

        void handle_select_tile();
        void handle_draw_tile();
    private:
        Map map;

        EditTool tool;
        vec2 mouse_pos;
        bool panning;
        bool drawing;

        int selected_tile;
        vec2 tileset_camera_pos;

        std::string command;
        bool typing;

        void handle_command();
};
