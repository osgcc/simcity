#pragma once

#include <string>

enum class SimulationSpeed
{
	Paused,
	Slow,
	Normal,
	Fast
};


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
