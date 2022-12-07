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

#include "EffectMap.h"

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
	Texture RateOfGrowth;
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


const Texture& rateOfGrowthTexture()
{
	return RateOfGrowth;
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


void drawOverlayPoints(Texture& overlay, const EffectMap& effectMap)
{
	SDL_SetRenderTarget(MainWindowRenderer, overlay.texture);
	turnOffBlending(overlay);
	clearOverlay();

	for (int x = 0; x < effectMap.dimensions().x; x++)
	{
		for (int y = 0; y < effectMap.dimensions().y; y++)
		{
			drawPointToCurrentOverlay(x, y, GetColorIndex(effectMap.value({ x, y })));
		}
	}

	turnOnBlending(overlay);
	SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


void drawPopDensity()
{
	drawOverlayPoints(PopulationDensity, PopulationDensityMap);
}


int rateOfGrowthColorIndex(int value)
{
	if (value > 100)
	{
		return VAL_VERYPLUS;
	}

	if (value > 20)
	{
		return VAL_PLUS;
	}

	if (value < -100)
	{
		return VAL_VERYMINUS;
	}

	if (value < -20)
	{
		return VAL_MINUS;
	}

	return VAL_NONE;
}


void drawRateOfGrowth()
{
	SDL_SetRenderTarget(MainWindowRenderer, RateOfGrowth.texture);
	turnOffBlending(RateOfGrowth);
	clearOverlay();

	for (int x = 0; x < EighthWorldWidth; x++)
	{
		for (int y = 0; y < EighthWorldHeight; y++)
		{
			const int index = rateOfGrowthColorIndex(RateOfGrowthMap[x][y]);
			drawPointToCurrentOverlay(x, y, index);
		}
	}

	turnOnBlending(RateOfGrowth);
	SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


void drawTrafficMap()
{
	//drawOverlayPoints(TrafficDensity, TrafficDensityMap);
}


void drawPollutionMap()
{
	//drawOverlayPoints(Pollution, PollutionMap);
}


void drawCrimeMap()
{
	//drawOverlayPoints(CrimeOverlay, CrimeMap);
}


void drawLandMap()
{
	//drawOverlayPoints(LandValue, LandValueMap);
}


void drawFireRadius()
{
	//drawOverlayPointsSm(FireRadius, FireRate);
}


void drawPoliceRadius()
{
	//drawOverlayPointsSm(PoliceRadius, PoliceMapEffect);
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

	initTexture(PoliceRadius, EighthWorldWidth, EighthWorldHeight);
	initTexture(FireRadius, EighthWorldWidth, EighthWorldHeight);
	initTexture(RateOfGrowth, EighthWorldWidth, EighthWorldHeight);
}
