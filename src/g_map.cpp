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
#include "g_map.h"

#include "SmallMaps.h"
#include "s_alloc.h"

#include "Texture.h"

#include <array>
#include <stdexcept>

#include <SDL2/SDL.h>


namespace
{
	constexpr auto VAL_NONE = 0;
	constexpr auto VAL_LOW = 1;
	constexpr auto VAL_MEDIUM = 2;
	constexpr auto VAL_HIGH = 3;
	constexpr auto VAL_VERYHIGH = 4;
	constexpr auto VAL_PLUS = 5;
	constexpr auto VAL_VERYPLUS = 6;
	constexpr auto VAL_MINUS = 7;
	constexpr auto VAL_VERYMINUS = 8;

	constexpr auto UNPOWERED = 0;
	constexpr auto POWERED = 1;
	constexpr auto CONDUCTIVE = 2;

	Texture CrimeOverlay;
	Texture PopulationDensity;
	Texture TrafficDensity;
	Texture Pollution;
	Texture LandValue;
	Texture PoliceRadius;
	Texture FireRadius;
};


namespace Colors
{
	constexpr SDL_Color LightGrey{ 211, 211, 211, 255 };
	constexpr SDL_Color Yellow{ 255, 255, 0, 255 };
	constexpr SDL_Color Orange{ 255, 165, 0, 255 };
	constexpr SDL_Color Red{ 255, 0, 0, 255 };
	constexpr SDL_Color DarkGreen{ 0, 100, 0, 255 };
	constexpr SDL_Color LightGreen{ 50, 205, 50, 255 };
	constexpr SDL_Color LightBlue{ 0, 191, 255, 255 };
	constexpr SDL_Color Clear{ 0, 0, 0, 0 };
};


std::array<SDL_Color, 3> PowerColorTable =
{ {
	Colors::LightGrey,
	Colors::Red,
	Colors::LightGrey
} };


std::array<SDL_Color, 9> OverlayColorTable =
{ {
	Colors::Clear,
	Colors::LightGrey,
	Colors::Yellow,
	Colors::Orange,
	Colors::Red,
	Colors::DarkGreen,
	Colors::LightGreen,
	Colors::Orange,
	Colors::Yellow
} };


const Texture& crimeOverlayTexture()
{
	return CrimeOverlay;
}


const Texture& populationDensityTexture()
{
	return PopulationDensity;
}


const Texture& trafficDensityTexture()
{
	return TrafficDensity;
}


const Texture& pollutionTexture()
{
	return Pollution;
}


const Texture& landValueTexture()
{
	return LandValue;
}


const Texture& policeRadiusTexture()
{
	return PoliceRadius;
}


const Texture& fireRadiusTexture()
{
	return FireRadius;
}


void turnOffBlending(const Texture& texture)
{
	SDL_SetRenderDrawBlendMode(MainWindowRenderer, SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(texture.texture, SDL_BLENDMODE_NONE);
}


void turnOnBlending(const Texture& texture)
{
	SDL_SetRenderDrawBlendMode(MainWindowRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(texture.texture, SDL_BLENDMODE_BLEND);
}


void clearOverlay()
{
	SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 0);
	SDL_RenderClear(MainWindowRenderer);
}


void drawPointToCurrentOverlay(const int x, const int y, const int colorIndex)
{
	const auto& color = OverlayColorTable[colorIndex];
	SDL_SetRenderDrawColor(MainWindowRenderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoint(MainWindowRenderer, x, y);
}


void drawRectToCurrentOverlay(const int x, const int y, const int size, const int colorIndex)
{
	const auto& color = OverlayColorTable[colorIndex];
	SDL_SetRenderDrawColor(MainWindowRenderer, color.r, color.g, color.b, color.a);
	const SDL_Rect rect{ x - size / 2, y - size / 2, size, size };
	SDL_RenderFillRect(MainWindowRenderer, &rect);
}


int GetColorIndex(int x)
{
	if (x < 50)
	{
		return VAL_NONE;
	}
	if (x < 100)
	{
		return VAL_LOW;
	}
	if (x < 150)
	{
		return VAL_MEDIUM;
	}
	if (x < 200)
	{
		return VAL_HIGH;
	}

	return VAL_VERYHIGH;
}


void drawOverlayPoints(Texture& overlay, const std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth>& arr)
{
	SDL_SetRenderTarget(MainWindowRenderer, overlay.texture);
	turnOffBlending(overlay);
	clearOverlay();

	for (int x = 0; x < HalfWorldWidth; x++)
	{
		for (int y = 0; y < HalfWorldHeight; y++)
		{
			drawPointToCurrentOverlay(x, y, GetColorIndex(arr[x][y]));
		}
	}

	turnOnBlending(overlay);
	SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


void drawOverlayPointsSm(Texture& overlay, const std::array<std::array<int, EightWorldHeight>, EightWorldWidth>& arr)
{
	SDL_SetRenderTarget(MainWindowRenderer, overlay.texture);
	turnOffBlending(overlay);
	clearOverlay();

	for (int x = 0; x < EightWorldWidth; x++)
	{
		for (int y = 0; y < EightWorldHeight; y++)
		{
			drawPointToCurrentOverlay(x, y, GetColorIndex(arr[x][y]));
		}
	}

	turnOnBlending(overlay);
	SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


void drawPopDensity()
{
	drawOverlayPoints(PopulationDensity, PopDensity);
}


void drawRateOfGrowth()
{
	drawAll();
	for (int x = 0; x < SmX; x++)
	{
		for (int y = 0; y < SmY; y++)
		{
			int val, z = RateOGMem[x][y];

			if (z > 100)
			{
				val = VAL_VERYPLUS;
			}
			else
			{
				if (z > 20)
				{
					val = VAL_PLUS;
				}
				else
				{
					if (z < -100)
					{
						val = VAL_VERYMINUS;
					}
					else
					{
						if (z < -20)
						{
							val = VAL_MINUS;
						}
						else
						{
							val = VAL_NONE;
						}
					}
				}
			}
			maybeDrawRect(val, x * 24, y * 24, 24, 24);
		}
	}
}


void drawTrafficMap()
{
	drawLilTransMap();
	drawOverlayPoints(TrafficDensity, TrfDensity);
}


void drawPollutionMap()
{
	drawOverlayPoints(Pollution, PollutionMem);
}


void drawCrimeMap()
{
	drawOverlayPoints(CrimeOverlay, CrimeMem);
}


void drawLandMap()
{
	drawOverlayPoints(LandValue, LandValueMem);
}


void drawFireRadius()
{
	drawOverlayPointsSm(FireRadius, FireRate);
}


void drawPoliceRadius()
{
	drawOverlayPointsSm(PoliceRadius, PoliceMapEffect);
}


void initTexture(Texture& texture, const int width, const int height)
{
	texture.texture = SDL_CreateTexture(MainWindowRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_QueryTexture(texture.texture, nullptr, nullptr, &texture.dimensions.x, &texture.dimensions.y);
	texture.area = { 0, 0, texture.dimensions.x, texture.dimensions.y };
}


void initOverlayTexture()
{
	initTexture(CrimeOverlay, HalfWorldWidth, HalfWorldHeight);
	initTexture(PopulationDensity, HalfWorldWidth, HalfWorldHeight);
	initTexture(TrafficDensity, HalfWorldWidth, HalfWorldHeight);
	initTexture(Pollution, HalfWorldWidth, HalfWorldHeight);
	initTexture(LandValue, HalfWorldWidth, HalfWorldHeight);

	initTexture(PoliceRadius, EightWorldWidth, EightWorldHeight);
	initTexture(FireRadius, EightWorldWidth, EightWorldHeight);
}


void maybeDrawRect(int val, int x, int y, int w, int h)
{
	throw std::runtime_error("maybeDrawRect(): Replace with call to drawPointToCurrentOverlay()");
}
