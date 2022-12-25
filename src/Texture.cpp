// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright � 2022 Leeor Dicker
//
// Portions Copyright � 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#include "Texture.h"

#if defined(__APPLE__)
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include <iostream>

Texture loadTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* temp = IMG_Load(filename.c_str());
    if (!temp)
    {
        std::cout << "loadTexture(): Unable to load '" + filename + "': " + SDL_GetError() << std::endl;
        throw std::runtime_error("loadTexture(): Unable to load '" + filename + "': " + SDL_GetError());
    }

    SDL_Texture* out = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);

    if (!out)
    {
        std::cout << "loadTexture(): Unable to load '" + filename + "': " + SDL_GetError() << std::endl;
        throw std::runtime_error("loadTexture(): Unable to load '" + filename + "': " + SDL_GetError());
    }

    int width = 0, height = 0;
    SDL_QueryTexture(out, nullptr, nullptr, &width, &height);

    return Texture{ out, SDL_Rect{ 0, 0, width, height }, { width, height } };
}
