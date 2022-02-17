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
extern std::array<std::array<int, QuarterWorldWidth>, QuarterWorldWidth> TerrainMem;
extern std::array<std::array<int, QuarterWorldWidth>, QuarterWorldWidth> Qtem;


/* 8X8 Maps  15 x 13 */
extern int RateOGMem[SmX][SmY];
extern int FireStMap[SmX][SmY];
extern int PoliceMap[SmX][SmY];
extern int PoliceMapEffect[SmX][SmY];
extern int ComRate[SmX][SmY];
extern int FireRate[SmX][SmY];
extern int STem[SmX][SmY];


extern std::array<int, HistoryLength> ResHis;
extern std::array<int, HistoryLength> ComHis;
extern std::array<int, HistoryLength> IndHis;

extern std::array<int, HistoryLength> MoneyHis;
extern std::array<int, HistoryLength> PollutionHis;
extern std::array<int, HistoryLength> CrimeHis;
extern std::array<int, HistoryLength> MiscHis;
extern std::array<int, PWRMAPSIZE> PowerMap;


extern int ResHisMax;
extern int ComHisMax;
extern int IndHisMax;


void initMapArrays();


