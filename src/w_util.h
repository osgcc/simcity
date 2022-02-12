#pragma once

#include "Point.h"

#include <string>

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
void SetGameLevelFunds(int level);
void setSkips(int skips);
void SetGameLevel(int level);
bool TestBounds(int x, int y, int width, int height);
Point<int> PositionToCell(const Point<int>& position, const Point<int>& offset);
