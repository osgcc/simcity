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
#include "MiniMapWindow.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdexcept>
#include <iostream>


MiniMapWindow::MiniMapWindow(const Point<int>& position, const Vector<int>& size)
{
    if (!SDL_WasInit(SDL_INIT_VIDEO))
    {
        std::cout << "MiniMapWindow::c'tor: SDL Video subsystem was not initialized. Initializing now." << std::endl;
        SDL_Init(SDL_INIT_VIDEO);
    }

    mWindow = SDL_CreateWindow("Mini Map", position.x, position.y, size.x, size.y, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_UTILITY);
    if (!mWindow)
    {
        throw std::runtime_error("MiniMapWindow::MiniMapWindow(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mRenderer)
    {
        throw std::runtime_error("MiniMapWindow::MiniMapWindow(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
}


MiniMapWindow::~MiniMapWindow()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
}


void MiniMapWindow::draw()
{

}
