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
#include "w_stubs.h"

#include <array>
#include <vector>


/* Allocate Stuff */


int SMapX, SMapY;

int CChr; // unmasked tile value
int CChr9; // masked tile value

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


int ResHisMax, Res2HisMax;
int ComHisMax, Com2HisMax;
int IndHisMax, Ind2HisMax;

int RoadSpend, PoliceSpend, FireSpend;
int RoadFund, PoliceFund, FireFund;
int RoadEffect, PoliceEffect, FireEffect;

int TaxFlag;

unsigned char tileSynch = 0x01;

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

std::array<int, HistoryLength> ResHis{};
std::array<int, HistoryLength> ComHis{};
std::array<int, HistoryLength> IndHis{};

std::array<int, HistoryLength> MoneyHis{};
std::array<int, HistoryLength> PollutionHis{};
std::array<int, HistoryLength> CrimeHis{};
std::array<int, HistoryLength> MiscHis{};
std::array<int, PWRMAPSIZE> PowerMap{};


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
        for (int i = 0; i < HistoryLength; ++i)
        {
            ResHis[i] = 0;
            ComHis[i] = 0;
            IndHis[i] = 0;
            MoneyHis[i] = 0;
            CrimeHis[i] = 0;
            PollutionHis[i] = 0;
        }
    }
};


void initMapArrays()
{
    resetHalfArrays();
    resetQuarterArrays();
    resetHistoryArrays();

    resetHistoryArrays();

    for (auto& idx : MiscHis) { idx = 0; }
    for (auto& idx : PowerMap) { idx = 0; }
}
