#pragma once

#include "Vector.h"

#include <SDL2/SDL.h>

#include <string>

struct Texture
{
    SDL_Texture* texture{ nullptr };
    SDL_Rect area{};

    Vector<int> dimensions{};
};


Texture loadTexture(SDL_Renderer* renderer, const std::string& filename);
