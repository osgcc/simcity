#pragma once

#include "main.h"
#include <array>

extern std::array<std::array<int, SimHeight>, SimWidth> Map; // Main Map 120 x 100

extern Byte* PopDensity[]; // 2X2 Maps  60 x 50
extern Byte* TrfDensity[];
extern Byte* PollutionMem[];
extern Byte* LandValueMem[];
extern Byte* CrimeMem[];
extern Byte* tem[];
extern Byte* tem2[];
extern Byte* TerrainMem[]; // 4X4 Maps  30 x 25
extern Byte* Qtem[];


extern int* ResHis;
extern int* ComHis;
extern int* IndHis;
extern int* MoneyHis;
extern int* CrimeHis;
extern int* PollutionHis;
extern int* MiscHis;
extern int* PowerMap;

extern int ResHisMax;
extern int ComHisMax;
extern int IndHisMax;



void initMapArrays();
