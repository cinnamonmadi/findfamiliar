#pragma once

#include <SDL2/SDL.h>
#include "engine.hpp"

class State {
    public:
        virtual ~State() {};
        virtual void handle_input(SDL_Event e) = 0;
        virtual void update() = 0;
        virtual void render(Engine* engine) = 0;
};
