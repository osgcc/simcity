#pragma once

extern short* Map[]; // Main Map 120 x 100

extern Byte* PopDensity[]; // 2X2 Maps  60 x 50
extern Byte* TrfDensity[];
extern Byte* PollutionMem[];
extern Byte* LandValueMem[];
extern Byte* CrimeMem[];
extern Byte* tem[];
extern Byte* tem2[];
extern Byte* TerrainMem[]; // 4X4 Maps  30 x 25
extern Byte* Qtem[];

void initMapArrays();
