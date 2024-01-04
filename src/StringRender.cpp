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

#include "StringRender.h"

#include <algorithm>


StringRender::StringRender(SDL_Renderer* renderer) :
    mRenderer(renderer)
{}


void StringRender::drawString(Font& font, std::string_view text, const Point<int>& position) const
{
    if (text.empty()) { return; }

    const auto& gml = font.metrics();
    if (gml.empty()) { return; }

    int offset = 0;
    for (auto character : text)
    {
        const auto& gm = gml[std::clamp<std::size_t>((uint8_t)(character), 0, 255)];

        const auto glyphCellSize = font.glyphCellSize().to<float>();
        const auto adjustX = (gm.minX < 0) ? gm.minX : 0;

        const SDL_Rect srcRect
        {
            static_cast<int>(gm.uvRect.x),
            static_cast<int>(gm.uvRect.y),
            static_cast<int>(glyphCellSize.x),
            static_cast<int>(glyphCellSize.y)
        };

        const SDL_Rect dstRect
        {
            position.x + offset + adjustX,
            position.y,
            static_cast<int>(glyphCellSize.x),
            static_cast<int>(glyphCellSize.y)
        };

        SDL_RenderCopy(mRenderer, font.texture(), &srcRect, &dstRect);

        offset += gm.advance;
    }
}
