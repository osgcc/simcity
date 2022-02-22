/* g_map.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */

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


void drawOverlay(Texture& overlay, const std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth>& arr)
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


void drawPopDensity()
{
	drawOverlay(PopulationDensity, PopDensity);
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
	drawOverlay(TrafficDensity, TrfDensity);
}


void drawPollutionMap()
{
	drawOverlay(Pollution, PollutionMem);
}


void drawCrimeMap()
{
	drawOverlay(CrimeOverlay, CrimeMem);
}


void drawLandMap()
{
	drawOverlay(LandValue, LandValueMem);
}


void drawFireRadius()
{
	drawAll();
	for (int x = 0; x < SmY; x++)
	{
		for (int y = 0; y < SmY; y++)
		{
			maybeDrawRect(GetColorIndex(FireRate[x][y]), x * 24, y * 24, 24, 24);
		}
	}
}


void drawPoliceRadius()
{
	drawAll();
	for (int x = 0; x < SmX; x++)
	{
		for (int y = 0; y < SmY; y++)
		{
			maybeDrawRect(GetColorIndex(PoliceMapEffect[x][y]), x * 24, y * 24, 24, 24);
		}
	}
}


void initTexture(Texture& texture)
{
	texture.texture = SDL_CreateTexture(MainWindowRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, HalfWorldWidth, HalfWorldHeight);
	SDL_QueryTexture(texture.texture, nullptr, nullptr, &texture.dimensions.x, &texture.dimensions.y);
	texture.area = { 0, 0, texture.dimensions.x, texture.dimensions.y };
}


void initOverlayTexture()
{
	initTexture(CrimeOverlay);
	initTexture(PopulationDensity);
	initTexture(TrafficDensity);
	initTexture(Pollution);
	initTexture(LandValue);
}


void maybeDrawRect(int val, int x, int y, int w, int h)
{
	throw std::runtime_error("maybeDrawRect(): Replace with call to drawPointToCurrentOverlay()");
}
