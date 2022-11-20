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
#include "s_alloc.h"

#include "main.h"
#include "Power.h"

#include <array>
#include <vector>


/* Allocate Stuff */

Point<int> SimulationTarget{};

int CurrentTile; // unmasked tile value
int CurrentTileMasked; // masked tile value

int RoadTotal, RailTotal, FirePop;

int ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
int ResZPop, ComZPop, IndZPop, TotalZPop; // zone counts
int HospPop, ChurchPop, StadiumPop;
int PolicePop, FireStPop;
int CoalPop, NuclearPop, PortPop, APortPop;

int NeedHosp, NeedChurch;
int CrimeAverage, PolluteAverage, LVAverage;

int CityTime;
int StartingYear;

int ResHisMax;
int ComHisMax;
int IndHisMax;

int RoadEffect, PoliceEffect, FireEffect;

std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> PopDensity{};
std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> TrfDensity{};
std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> PollutionMem{};
std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> LandValueMem{};
std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> CrimeMem{};

std::array<std::array<int, QuarterWorldHeight>, QuarterWorldWidth> TerrainMem{};
std::array<std::array<int, QuarterWorldHeight>, QuarterWorldWidth> Qtem{};


int RateOGMem[SmX][SmY];
int FireStMap[SmX][SmY];
int PoliceMap[SmX][SmY];


//int PoliceMapEffect[SmX][SmY];
std::array<std::array<int, EightWorldHeight>, EightWorldWidth> PoliceMapEffect{};
std::array<std::array<int, EightWorldHeight>, EightWorldWidth> FireRate{};

int ComRate[SmX][SmY];
int STem[SmX][SmY];


GraphHistory ResHis{};
GraphHistory ComHis{};
GraphHistory IndHis{};

GraphHistory MoneyHis{};
GraphHistory PollutionHis{};
GraphHistory CrimeHis{};
GraphHistory MiscHis{};

GraphHistory ResHis120Years{};
GraphHistory ComHis120Years{};
GraphHistory IndHis120Years{};

GraphHistory MoneyHis120Years{};
GraphHistory PollutionHis120Years{};
GraphHistory CrimeHis120Years{};
GraphHistory MiscHis120Years{};

namespace
{
    void resetHalfArrays()
    {
        for (int x = 0; x < HalfWorldWidth; x++)
        {
            for (int y = 0; y < HalfWorldHeight; y++)
            {
                PopDensity[x][y] = 0;
                TrfDensity[x][y] = 0;
                PollutionMem[x][y] = 0;
                LandValueMem[x][y] = 0;
                CrimeMem[x][y] = 0;
            }
        }
    }

    void resetQuarterArrays()
    {
        for (int row = 0; row < QuarterWorldWidth; ++row)
        {
            for (int col = 0; col < QuarterWorldHeight; ++col)
            {
                TerrainMem[row][col] = 0;
                Qtem[row][col] = 0;
            }
        }
    }

    void resetHistoryArrays()
    {
         ResHis.fill(0);
         ComHis.fill(0);
         IndHis.fill(0);

         MoneyHis.fill(0);
         PollutionHis.fill(0);
         CrimeHis.fill(0);
         MiscHis.fill(0);

         ResHis120Years.fill(0);
         ComHis120Years.fill(0);
         IndHis120Years.fill(0);

         MoneyHis120Years.fill(0);
         PollutionHis120Years.fill(0);
         CrimeHis120Years.fill(0);
         MiscHis120Years.fill(0);

         MiscHis.fill(0);
         ResetPowerMap();
    }
};


void initMapArrays()
{
    resetHalfArrays();
    resetQuarterArrays();
    resetHistoryArrays();

    resetHistoryArrays();
}
