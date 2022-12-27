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
#include "Graphics.h"

#include "EffectMap.h"
#include "Texture.h"


void turnOffBlending(SDL_Renderer& renderer, const Texture& texture)
{
    SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(texture.texture, SDL_BLENDMODE_NONE);
}


void turnOnBlending(SDL_Renderer& renderer, const Texture& texture)
{
    SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(texture.texture, SDL_BLENDMODE_BLEND);
}


void drawPoint(SDL_Renderer& renderer, const Point<int>& point, const SDL_Color& color)
{
    SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(&renderer, point.x, point.y);
}


void drawRect(SDL_Renderer& renderer, const SDL_Rect& rect, const SDL_Color& color)
{
    /*
    SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
    const SDL_Rect rect{ x - size / 2, y - size / 2, size, size };
    SDL_RenderFillRect(&renderer, &rect);
    */
}


void initTexture(SDL_Renderer& renderer, Texture& texture, const Vector<int>& dimensions)
{
    texture.texture = SDL_CreateTexture(&renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dimensions.x, dimensions.y);
    SDL_QueryTexture(texture.texture, nullptr, nullptr, &texture.dimensions.x, &texture.dimensions.y);
    texture.area = { 0, 0, texture.dimensions.x, texture.dimensions.y };
}
