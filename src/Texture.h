// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 Leeor Dicker
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include "Vector.h"

#include <SDL2/SDL.h>

#include <string>

struct Texture final
{
    SDL_Texture* texture{ nullptr };
    SDL_Rect area{};

    Vector<int> dimensions{};
};


Texture loadTexture(SDL_Renderer* renderer, const std::string& filename);
Texture newTexture(SDL_Renderer* renderer, const Vector<int>& dimensions);
