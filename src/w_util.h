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

#include <string>

class Budget;

enum class SimulationSpeed
{
	Paused,
	Slow,
	Normal,
	Fast
};

const std::string& SpeedString(SimulationSpeed speed);

void setAnyCityName(const char* name);
void DoNewGame();
std::string NumberToDollarDecimal(int value);

void SimSpeed(SimulationSpeed speed);
SimulationSpeed SimSpeed();

void Pause();
void Resume();

bool Paused();

int CurrentYear();
void SetYear(int year);
void SetGameLevelFunds(int level, Budget&);
bool CoordinatesValid(int x, int y, int width, int height);
Point<int> PositionToCell(const Point<int>& position, const Point<int>& offset);
