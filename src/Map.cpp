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
#include "Map.h"

#include "s_alloc.h"

#include "Point.h"

#include "Texture.h"
#include "SmallMaps.h"
#include "Sprite.h"

#include <array>
#include <SDL2/SDL.h>

extern SDL_Renderer* MainWindowRenderer;

extern Texture BigTileset;
extern Texture MainMapTexture;


std::array<std::array<int, SimHeight>, SimWidth> Map;


namespace
{
	SDL_Rect tileRect{ 0, 0, 16, 16 };
	bool flagBlink{ false };
};


void toggleBlinkFlag()
{
	flagBlink = !flagBlink;
}


void ResetMap()
{
	for (int row = 0; row < SimWidth; ++row)
	{
		for (int col = 0; col < SimHeight; ++col)
		{
			Map[row][col] = 0;
		}
	}
}


int& tileValue(const Point<int>& location)
{
	return tileValue(location.x, location.y);
}


int& tileValue(const int x, const int y)
{
	return Map[x][y];
}


void tileValue(const Point<int>& location, unsigned int value)
{
	Map[location.x][location.y] = static_cast<int>(value);
}


unsigned int maskedTileValue(const Point<int>& location)
{
	return maskedTileValue(location.x, location.y);
}


unsigned int maskedTileValue(const int x, const int y)
{
	return tileValue(x, y) & LOMASK;
}


unsigned int maskedTileValue(unsigned int tile)
{
	return tile & LOMASK;
}


bool tilePowered(const unsigned int tile)
{
	return tile & PWRBIT;
}

bool tileIsZoned(const unsigned int tile)
{
	return tile & ZONEBIT;
}


bool blink()
{
	return flagBlink;
}


/**
 * Assumes \c begin and \c end are in a valid range
 */
void DrawBigMapSegment(const Point<int>& begin, const Point<int>& end)
{
	SDL_SetRenderTarget(MainWindowRenderer, MainMapTexture.texture);

	SDL_Rect drawRect{ 0, 0, 16, 16 };
	unsigned int tile = 0;

	for (int row = begin.x; row < end.x; row++)
	{
		for (int col = begin.y; col < end.y; col++)
		{
			tile = tileValue(row, col);
			// Blink lightning bolt in unpowered zone center
			if (blink() && tileIsZoned(tile) && !tilePowered(tile))
			{
				tile = LIGHTNINGBOLT;
			}

			drawRect = { row * drawRect.w, col * drawRect.h, drawRect.w, drawRect.h };

			const unsigned int masked = maskedTileValue(tile);
			tileRect =
			{
				(static_cast<int>(masked) % 32) * 16,
				(static_cast<int>(masked) / 32) * 16,
				16, 16
			};

			SDL_RenderCopy(MainWindowRenderer, BigTileset.texture, &tileRect, &drawRect);
		}
	}

	SDL_RenderPresent(MainWindowRenderer);
	SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


void DrawBigMap()
{
	DrawBigMapSegment(Point<int>{0, 0}, Point<int>{SimWidth, SimHeight});
}
