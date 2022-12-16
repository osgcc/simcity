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

#include <SDL2/SDL.h>
#include <string>

class Budget;
class CityProperties;

enum class SimulationSpeed
{
	Paused,
	Slow,
	Normal,
	Fast,
	AfricanSwallow
};

const std::string& SpeedString(SimulationSpeed speed);

void DoNewGame();
std::string NumberToDollarDecimal(int value);

void SimSpeed(SimulationSpeed speed);
SimulationSpeed SimSpeed();

void Pause();
void Resume();

bool Paused();

int CurrentYear();
void SetYear(int year);
void SetGameLevelFunds(int level, CityProperties& properties, Budget&);
bool CoordinatesValid(const Point<int>& position);
Point<int> PositionToCell(const Point<int>& position, const Point<int>& offset);
const Vector<int> vectorFromPoints(const Point<int>& start, const Point<int>& end);
bool pointInRect(const Point<int>& point, const SDL_Rect& rect);

int RandomRange(int min, int max);
int Random();
int Rand16();
