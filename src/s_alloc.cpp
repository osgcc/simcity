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

#include "EffectMap.h"
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


EffectMap PopulationDensityMap({HalfWorldWidth, HalfWorldHeight });
EffectMap TrafficDensityMap({ HalfWorldWidth, HalfWorldHeight });
EffectMap PollutionMap({ HalfWorldWidth, HalfWorldHeight });
EffectMap LandValueMap({ HalfWorldWidth, HalfWorldHeight });
EffectMap CrimeMap({ HalfWorldWidth, HalfWorldHeight });

EffectMap TerrainMem({ QuarterWorldWidth, QuarterWorldHeight });

EffectMap RateOfGrowthMap({ EighthWorldWidth, EighthWorldHeight });
EffectMap FireStationMap({ EighthWorldWidth, EighthWorldHeight });
EffectMap PoliceStationMap({ EighthWorldWidth, EighthWorldHeight });

EffectMap PoliceProtectionMap({ EighthWorldWidth, EighthWorldHeight });
EffectMap FireProtectionMap({ EighthWorldWidth, EighthWorldHeight });

EffectMap ComRate({ EighthWorldWidth, EighthWorldHeight });

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
        PopulationDensityMap.reset();
        TrafficDensityMap.reset();
        PollutionMap.reset();
        LandValueMap.reset();
        CrimeMap.reset();
    }

    void resetQuarterArrays()
    {
        TerrainMem.reset();
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
         resetPowerMap();
    }
};


void initMapArrays()
{
    resetHalfArrays();
    resetQuarterArrays();
    resetHistoryArrays();
}


bool MoveSimulationTarget(SearchDirection direction)
{
    switch (direction)
    {
    case SearchDirection::Left:
        if (SimulationTarget.y > 0)
        {
            SimulationTarget.y--;
            return true;
        }
        if (SimulationTarget.y < 0)
        {
            SimulationTarget.y = 0;
        }
        return false;

    case SearchDirection::Down:
        if (SimulationTarget.x < (SimWidth - 1))
        {
            SimulationTarget.x++;
            return true;
        }
        if (SimulationTarget.x > (SimWidth - 1))
        {
            SimulationTarget.x = SimWidth - 1;
        }
        return false;

    case SearchDirection::Right:
        if (SimulationTarget.y < (SimHeight - 1))
        {
            SimulationTarget.y++;
            return true;
        }
        if (SimulationTarget.y > (SimHeight - 1))
        {
            SimulationTarget.y = SimHeight - 1;
        }
        return false;

    case SearchDirection::Up:
        if (SimulationTarget.x > 0)
        {
            SimulationTarget.x--;
            return true;
        }
        if (SimulationTarget.x < 0)
        {
            SimulationTarget.x = 0;
        }
        return false;

    case SearchDirection::Undefined:
        return true;
    }

    return false;
}
