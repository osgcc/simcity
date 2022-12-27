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

#include "Point.h"
#include "Texture.h"
#include "Vector.h"

#include <SDL2/SDL.h>

void turnOffBlending(SDL_Renderer& renderer, const Texture& texture);
void turnOnBlending(SDL_Renderer& renderer, const Texture& texture);
void drawPoint(SDL_Renderer& renderer, const Point<int>& point, const SDL_Color& color);
void drawRect(SDL_Renderer& renderer, const SDL_Rect& rect, const SDL_Color& color);
void initTexture(SDL_Renderer& renderer, Texture& texture, const Vector<int>& dimensions);
