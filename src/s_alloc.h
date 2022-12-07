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

#include "main.h"
#include "Point.h"

#include <array>

enum class SearchDirection
{
	Left, Down, Right, Up, Undefined
};

extern Point<int> SimulationTarget;

// 2X2 Maps  60 x 50
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> PopulationDensityMap;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> TrafficDensityMap;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> PollutionMap;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> LandValueMap;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> CrimeMap;

// 4X4 Maps  30 x 25
extern std::array<std::array<int, QuarterWorldHeight>, QuarterWorldWidth> TerrainMem;
extern std::array<std::array<int, QuarterWorldHeight>, QuarterWorldWidth> Qtem;


/* 8X8 Maps  15 x 13 */
extern int RateOfGrowthMap[EighthWorldWidth][EighthWorldHeight];
extern int FireStationMap[EighthWorldWidth][EighthWorldHeight];
extern int PoliceStationMap[EighthWorldWidth][EighthWorldHeight];

extern std::array<std::array<int, EighthWorldHeight>, EighthWorldWidth> PoliceMapEffect;
extern std::array<std::array<int, EighthWorldHeight>, EighthWorldWidth> FireRate;

extern int ComRate[EighthWorldWidth][EighthWorldHeight];
extern int STem[EighthWorldWidth][EighthWorldHeight];

using GraphHistory = std::array<int, HistoryLength>;

extern GraphHistory ResHis;
extern GraphHistory ComHis;
extern GraphHistory IndHis;
extern GraphHistory MoneyHis;
extern GraphHistory PollutionHis;
extern GraphHistory CrimeHis;
extern GraphHistory MiscHis;

extern GraphHistory ResHis120Years;
extern GraphHistory ComHis120Years;
extern GraphHistory IndHis120Years;
extern GraphHistory MoneyHis120Years;
extern GraphHistory PollutionHis120Years;
extern GraphHistory CrimeHis120Years;
extern GraphHistory MiscHis120Years;

extern int ResHisMax;
extern int ComHisMax;
extern int IndHisMax;

void initMapArrays();
bool MoveSimulationTarget(SearchDirection direction);
