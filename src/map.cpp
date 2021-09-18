#include "map.hpp"

#include <iostream>
#include <fstream>

Map::Map() {
    width = 10;
    height = 9;

    tiles = new int[width * height];
    walls = new bool[width * height];
    for(int i = 0; i < width * height; i++) {
        tiles[i] = 0;
        walls[i] = false;
    }

    camera_position = vec2(0, 0);
}

Map::~Map() {
    delete [] tiles;
    delete [] walls;
}

void Map::render(Engine* engine) {
    // Render map
    vec2 start_tile = tile_at(camera_position);
    vec2 base_render_pos = position_of(start_tile) - camera_position;
    vec2 draw_size = vec2(Engine::SCREEN_WIDTH / Engine::TILE_SIZE, Engine::SCREEN_HEIGHT / Engine::TILE_SIZE);
    if(camera_position.x % Engine::TILE_SIZE != 0) {
        draw_size.x++;
    }
    if(camera_position.y % Engine::TILE_SIZE != 0) {
        draw_size.y++;
    }
    for(int y = 0; y < draw_size.y; y++) {
        for(int x = 0; x < draw_size.x; x++) {
            vec2 tile = start_tile + vec2(x, y);
            if(tile.x < 0 || tile.x >= width || tile.y < 0 || tile.y >= height) {
                continue;
            }
            vec2 render_pos = base_render_pos + vec2(x * Engine::TILE_SIZE, y * Engine::TILE_SIZE);
            engine->render_sprite_frame(SPRITE_TILES, get_tile(tile), render_pos.x, render_pos.y, false);
        }
    }
}

bool Map::in_bounds(vec2 pos) const {
    int index = to_index(pos);
    return index >= 0 && index < width * height;
}

int Map::get_tile(vec2 pos) const {
    return tiles[to_index(pos)];
}

bool Map::get_wall(vec2 pos) const {
    return walls[to_index(pos)];
}

void Map::set_tile(vec2 pos, int value) {
    tiles[to_index(pos)] = value;
}

void Map::set_wall(vec2 pos, bool value) {
    walls[to_index(pos)] = value;
}

void Map::resize(int new_width, int new_height) {
    int* new_tiles = new int[new_width * new_height];
    bool* new_walls = new bool[new_width * new_height];

    for(int i = 0; i < new_width * new_height; i++) {
        new_tiles[i] = 0;
        new_walls[i] = false;
    }
    for(int y = 0; y < (int)fmin(height, new_height); y++) {
        for(int x = 0; x < (int)fmin(width, new_width); x++) {
            int old_index = (y * width) + x;
            int new_index = (y * new_width) + x;

            new_tiles[new_index] = tiles[old_index];
            new_walls[new_index] = walls[old_index];
        }
    }

    delete [] tiles;
    delete [] walls;

    tiles = new_tiles;
    walls = new_walls;
    width = new_width;
    height = new_height;
}

void Map::save_to_file(const char* path) {
    std::ofstream outfile(path, std::ios::out);

    if(!outfile.is_open()) {
        std::cout << "Unable to open file!" << std::endl;
        return;
    }

    outfile << width << "," << height << std::endl;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            outfile << get_tile(vec2(x, y));
            if(x != width - 1) {
                outfile << ",";
            }
        }
        outfile << "\n";
    }
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            outfile << (int)get_wall(vec2(x, y));
            if(x != width - 1) {
                outfile << ",";
            }
        }
        outfile << "\n";
    }

    outfile.close();
}

void Map::load_from_file(const char* path) {
    std::ifstream infile(path);

    if(!infile.is_open()) {
        std::cout << "Unable to open file!" << std::endl;
        return;
    }

    std::string line;
    getline(infile, line);

    width = atoi(line.substr(0, line.find(',')).c_str());
    height = atoi(line.substr(line.find(',') + 1).c_str());

    delete [] tiles;
    delete [] walls;

    tiles = new int[width * height];
    walls = new bool[width * height];

    for(int y = 0; y < height; y++) {
        getline(infile, line);
        for(int x = 0; x < width; x++) {
            size_t comma_index = line.find(',');
            int tile;
            if(comma_index == std::string::npos) {
                tile = atoi(line.c_str());
            } else {
                tile = atoi(line.substr(0, comma_index).c_str());
                line = line.substr(comma_index + 1);
            }
            set_tile(vec2(x, y), tile);
        }
    }

    for(int y = 0; y < height; y++) {
        getline(infile, line);
        for(int x = 0; x < width; x++) {
            size_t comma_index = line.find(',');
            bool wall;
            if(comma_index == std::string::npos) {
                wall = (bool)atoi(line.c_str());
            } else {
                wall = (bool)atoi(line.substr(0, comma_index).c_str());
                line = line.substr(comma_index + 1);
            }
            set_wall(vec2(x, y), wall);
        }
    }

    infile.close();
}
