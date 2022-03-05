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
#include <array>


// 2X2 Maps  60 x 50
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> PopDensity;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> TrfDensity;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> PollutionMem;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> LandValueMem;
extern std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> CrimeMem;

// 4X4 Maps  30 x 25
extern std::array<std::array<int, QuarterWorldHeight>, QuarterWorldWidth> TerrainMem;
extern std::array<std::array<int, QuarterWorldHeight>, QuarterWorldWidth> Qtem;


/* 8X8 Maps  15 x 13 */
extern int RateOGMem[SmX][SmY];
extern int FireStMap[SmX][SmY];
extern int PoliceMap[SmX][SmY];

extern std::array<std::array<int, EightWorldHeight>, EightWorldWidth> PoliceMapEffect;
extern std::array<std::array<int, EightWorldHeight>, EightWorldWidth> FireRate;

extern int ComRate[SmX][SmY];
extern int STem[SmX][SmY];


extern std::array<int, HistoryLength> ResHis;
extern std::array<int, HistoryLength> ComHis;
extern std::array<int, HistoryLength> IndHis;
extern std::array<int, HistoryLength> MoneyHis;
extern std::array<int, HistoryLength> PollutionHis;
extern std::array<int, HistoryLength> CrimeHis;
extern std::array<int, HistoryLength> MiscHis;

extern std::array<int, PWRMAPSIZE> PowerMap;

extern std::array<int, HistoryLength> ResHis120Years;
extern std::array<int, HistoryLength> ComHis120Years;
extern std::array<int, HistoryLength> IndHis120Years;
extern std::array<int, HistoryLength> MoneyHis120Years;
extern std::array<int, HistoryLength> PollutionHis120Years;
extern std::array<int, HistoryLength> CrimeHis120Years;
extern std::array<int, HistoryLength> MiscHis120Years;


extern int ResHisMax;
extern int ComHisMax;
extern int IndHisMax;


void initMapArrays();


