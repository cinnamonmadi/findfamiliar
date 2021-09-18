#pragma once

#include "engine.hpp"
#include "vector.hpp"

class Map {
    public:
        static const int OUT_OF_BOUNDS = -1;

        int width;
        int height;
        vec2 camera_position;

        Map();
        ~Map();

        void render(Engine* engine);

        bool in_bounds(vec2 pos) const;
        int get_tile(vec2 pos) const;
        bool get_wall(vec2 pos) const;

        void set_tile(vec2 pos, int value);
        void set_wall(vec2 pos, bool value);

        void resize(int new_width, int new_height);

        void save_to_file(const char* path);
        void load_from_file(const char* path);
    private:
        int* tiles;
        bool* walls;

        inline int to_index(vec2 pos) const {
            return (pos.y * width) + pos.x;
        }
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
