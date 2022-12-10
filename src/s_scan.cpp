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
#include "s_scan.h"

#include "EffectMap.h"
#include "Map.h"

#include "s_alloc.h"
#include "s_sim.h"
#include "s_zone.h"

#include "Vector.h"

#include <algorithm>

/* Map Updates */

namespace
{
    bool NewMap{ false };

    Point<int> PollutionMax;
    Point<int> CrimeMax;

    Point<int> CityCenter;

    std::array<int, NMAPS> NewMapFlags;

    EffectMap tem({ HalfWorldWidth, HalfWorldHeight });
    EffectMap tem2({ HalfWorldWidth, HalfWorldHeight });

    std::array<std::array<int, QuarterWorldHeight>, QuarterWorldWidth> Qtem{};
};


const Point<int>& cityCenterOfMass()
{
    return CityCenter;
}


bool newMap()
{
    return NewMap;
}


void newMap(bool value)
{
    NewMap = value;
}


const Point<int>& pollutionMax()
{
    return PollutionMax;
}


std::array<int, NMAPS>& newMapFlags()
{
    return NewMapFlags;
}


void resetNewMapFlags()
{
    NewMapFlags.fill(1);
}


/* comefrom: FireAnalysis */
void SmoothFSMap()
{
    int x, y, edge;

    for (x = 0; x < EighthWorldWidth; x++)
    {
        for (y = 0; y < EighthWorldHeight; y++)
        {
            edge = 0;

            if (x)
            {
                edge += FireStationMap.value({ x - 1, y });
            }

            if (x < (EighthWorldWidth - 1))
            {
                edge += FireStationMap.value({ x + 1, y });
            }

            if (y)
            {
                edge += FireStationMap.value({ x, y - 1 });
            }

            if (y < (EighthWorldHeight - 1))
            {
                edge += FireStationMap.value({ x, y + 1 });
            }

            edge = (edge / 4) + FireStationMap.value({ x, y });
            STem[x][y] = edge / 2;
        }
    }

    for (x = 0; x < EighthWorldWidth; x++)
    {
        for (y = 0; y < EighthWorldHeight; y++)
        {
            FireStationMap.value({ x, y }) = STem[x][y];
        }
    }
}


/* comefrom: Simulate SpecialInit */
void FireAnalysis()		/* Make firerate map from firestation map  */
{
    SmoothFSMap();
    SmoothFSMap();
    SmoothFSMap();

    FireProtectionMap = FireStationMap;

    NewMapFlags[DYMAP] = NewMapFlags[FIMAP] = 1;
}


int getPopulationDensity(int tile)
{
    if (tile == FREEZ)
    {
        return DoFreePop();
    }

    if (tile < COMBASE)
    {
        return RZPop(tile);
    }

    if (tile < INDBASE)
    {
        return CZPop(tile) * 8;
    }

    if (tile < PORTBASE)
    {
        return IZPop(tile) * 8;
    }

    return 0;
}


void SmoothArray(const EffectMap& src, EffectMap& dst)
{
    if (src.dimensions() != dst.dimensions())
    {
        throw std::runtime_error("SmoothArray: Source and Destination array dimensions do not match.");
    }

    for (int x{}; x < src.dimensions().x; ++x)
    {
        for (int y{}; y < src.dimensions().y; ++y)
        {
            int val{ 0 };
            if (x > 0)
            {
                val += src.value({ x - 1, y });
            }

            if (x < (src.dimensions().x - 1))
            {
                val += src.value({ x + 1, y });
            }

            if (y > 0)
            {
                val += src.value({ x, y - 1 });
            }

            if (y < (src.dimensions().y - 1))
            {
                val += src.value({ x, y + 1 });
            }

            dst.value({ x, y }) = std::clamp((val + src.value({ x, y })) / 4, 0, 255);
        }
    }
}


/* comefrom: PTLScan */
int GetPValue(int loc)
{
    if (loc < POWERBASE)
    {
        if (loc >= HTRFBASE) /* heavy traf  */
        {
            return (/* 25 */ 75);
        }
        
        if (loc >= LTRFBASE) /* light traf  */
        {
            return (/* 10 */ 50);
        }

        if (loc < ROADBASE)
        {
            if (loc > FIREBASE)
            {
                return (/* 60 */ 90);
            }

            /* XXX: Why negative pollution from radiation? */
            if (loc >= RADTILE) /* radioactivity  */
            {
                return (/* -40 */ 255);
            }
        }
        return 0;
    }

    if (loc <= LASTIND)
    {
        return (0);
    }

    if (loc < PORTBASE) /* Ind  */
    {
        return (50);
    }

    if (loc <= LASTPOWERPLANT) /* prt, aprt, cpp */
    {
        return (/* 60 */ 100);
    }
    
    return 0;
}


/* comefrom: PTLScan DistIntMarket */
int distanceToCityCenter(int x, int y)
{
    const Vector<int> radius = { CityCenter.x / 2, (CityCenter.y / 2) };

    Vector<int> distance{};
    if (x > radius.x)
    {
        distance.x = x - radius.x;
    }
    else
    {
        distance.x = radius.x - x;
    }

    if (y > radius.y)
    {
        distance.y = y - radius.y;
    }
    else
    {
        distance.y = radius.y - y;
    }

    int totalDistance = (distance.x + distance.y);
    if (totalDistance > 32) // fixme: magic number
    {
        return 32;
    }

    return totalDistance;
}


/* comefrom: PopDenScan */
void DistIntMarket()
{
    int x, y, z;

    for (x = 0; x < EighthWorldWidth; x++)
    {
        for (y = 0; y < EighthWorldHeight; y++)
        {
            z = distanceToCityCenter(x * 4 , y * 4);
            z *= 4;
            z = 64 - z;
            ComRate[x][y] = z;
        }
    }
}


/* comefrom: Simulate SpecialInit */
void PopDenScan()		/*  sets: PopulationDensityMap, , , ComRate  */
{
    int Xtot, Ytot, Ztot;

    tem.reset();

    Xtot = 0;
    Ytot = 0;
    Ztot = 0;

    for (int x{}; x < SimWidth; ++x)
    {
        for (int y{}; y < SimHeight; ++y)
        {
            int z = Map[x][y];
            if (z & ZONEBIT)
            {
                z = z & LOMASK;
                SimulationTarget = { x, y };
                
                z = std::clamp(getPopulationDensity(z) * 8, 0, 254);

                tem.value({ x / 2, y / 2 }) = z;
                
                Xtot += x;
                Ytot += y;
                Ztot++;
            }
        }
    }

    SmoothArray(tem, tem2);
    SmoothArray(tem2, tem);
    SmoothArray(tem, tem2);

    for (int x{}; x < HalfWorldWidth; ++x)
    {
        for (int y{}; y < HalfWorldHeight; ++y)
        {
            PopulationDensityMap.value({ x, y }) = tem2.value({ x, y }) * 2;
        }
    }

    DistIntMarket(); /* set ComRate w/ (/ComMap) */
    
    if (Ztot) /* Find Center of Mass for City */
    {
        CityCenter = { Xtot / Ztot, Ytot / Ztot };
    }
    else /* if pop=0 center of Map is CC */
    {
        CityCenter = { HalfWorldWidth, HalfWorldHeight };
    }
    
    NewMapFlags[DYMAP] = NewMapFlags[PDMAP] = NewMapFlags[RGMAP] = 1;
}


/* comefrom: PTLScan */
void SmoothTerrain()
{
    for (int x{}; x < QuarterWorldWidth; ++x)
    {
        for (int y{}; y < QuarterWorldHeight; ++y)
        {
            int z = 0;
            if (x > 0)
            {
                z += Qtem[x - 1][y];
            }

            if (x < (QuarterWorldWidth - 1))
            {
                z += Qtem[x + 1][y];
            }

            if (y > 0)
            {
                z += Qtem[x][y - 1];
            }

            if (y < (QuarterWorldHeight - 1))
            {
                z += Qtem[x][y + 1];
            }

            TerrainMem[x][y] = (((z / 4) + Qtem[x][y]) / 2) % 256;
        }
    }
}


/* comefrom: CrimeScan */
void SmoothPSMap()
{
    int x, y, edge;

    for (x = 0; x < EighthWorldWidth; x++)
    {
        for (y = 0; y < EighthWorldHeight; y++)
        {
            edge = 0;

            if (x)
            {
                edge += PoliceStationMap.value({ x -  1, y });
            }

            if (x < (EighthWorldWidth - 1))
            {
                edge += PoliceStationMap.value({ x + 1, y });
            }

            if (y)
            {
                edge += PoliceStationMap.value({ x, y - 1 });
            }

            if (y < (EighthWorldHeight - 1))
            {
                edge += PoliceStationMap.value({ x, y + 1});
            }

            edge = (edge / 4) + PoliceStationMap.value({ x, y });
            STem[x][y] = edge / 2;
        }
    }

    for (x = 0; x < EighthWorldWidth; x++)
    {
        for (y = 0; y < EighthWorldHeight; y++)
        {
            PoliceStationMap.value({ x, y }) = STem[x][y];
        }
    }
}


int pollutionLevel(const Point<int>& point, int& LVflag)
{
    int pollutionLevel{};
    for (int xx = (point.x * 2); xx <= (point.x * 2) + 1; ++xx)
    {
        for (int yy = (point.y * 2); yy <= (point.y * 2) + 1; ++yy)
        {
            const int tile = (Map[xx][yy] & LOMASK);
            if (tile)
            {
                if (tile < RUBBLE)
                {
                    /* inc terrainMem */
                    Qtem[point.x / 2][point.y / 2] += 15;
                    continue;
                }

                pollutionLevel += GetPValue(tile);
                if (tile >= ROADBASE)
                {
                    LVflag++;
                }
            }
        }
    }

    return std::clamp(pollutionLevel, 0, 255);
}


void setMostPollutedLocation()
{
    int highestPollution{ 0 };
    int pollutedTileCount{ 0 };
    int pollutionTotal{ 0 };

    for (int x = 0; x < HalfWorldWidth; ++x)
    {
        for (int y = 0; y < HalfWorldHeight; ++y)
        {
            const int pollutionValue = tem.value({ x, y });
            PollutionMap.value({ x, y }) = pollutionValue;

            if (pollutionValue) /*  get pollute average  */
            {
                pollutedTileCount++;
                pollutionTotal += pollutionValue;

                /* find max pol for monster  */
                if ((pollutionValue > highestPollution) || ((pollutionValue == highestPollution) && (!(Rand16() & 3))))
                {
                    highestPollution = pollutionValue;
                    PollutionMax = { x * 2, y * 2 };
                }
            }
        }
    }

    if (pollutedTileCount)
    {
        PolluteAverage = pollutionTotal / pollutedTileCount;
    }
    else
    {
        PolluteAverage = 0;
    }
}

/* comefrom: Simulate SpecialInit */
/* Does pollution, terrain, land value   */
/**
 * fixme: Holy shit, this function does __way__ too much. This
 *        needs to be broken down into smaller chunks.  
 */
void PTLScan()
{
    for (auto& arr : Qtem)
    {
        arr.fill(0);
    }

    int LVtot = 0;
    int LVnum = 0;
    for (int x = 0; x < HalfWorldWidth; ++x)
    {
        for (int y = 0; y < HalfWorldHeight; ++y)
        {
            int LVflag = 0;
            tem.value({ x, y }) = pollutionLevel({ x, y }, LVflag);

            if (LVflag) /* LandValue Equation */
            {
                int dis = 34 - distanceToCityCenter(x, y);
                dis = dis * 4;
                dis += (TerrainMem[x / 2][y / 2]);
                dis -= (PollutionMap.value({ x, y }));

                if (CrimeMap.value({ x, y }) > 190)
                {
                    dis -= 20;
                }

                if (dis > 250)
                {
                    dis = 250;
                }

                if (dis < 1)
                {
                    dis = 1;
                }

                LandValueMap.value({ x, y }) = dis;
                LVtot += dis;
                LVnum++;
            }
            else
            {
                LandValueMap.value({ x, y }) = 0;
            }
        }
    }

    if (LVnum)
    {
        LVAverage = LVtot / LVnum;
    }
    else
    {
        LVAverage = 0;
    }

    SmoothArray(tem, tem2);
    SmoothArray(tem2, tem);

    setMostPollutedLocation();

    SmoothTerrain();

    NewMapFlags[DYMAP] = NewMapFlags[PLMAP] = NewMapFlags[LVMAP] = 1;
}


/* comefrom: Simulate SpecialInit */
void CrimeScan()
{
    int numz;
    int totz;
    int x, y, z;
    int cmax;

    SmoothPSMap();
    SmoothPSMap();
    SmoothPSMap();

    totz = 0;
    numz = 0;
    cmax = 0;
    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            if (z = LandValueMap.value({ x, y }))
            {
                ++numz;

                z = 128 - z;
                z += PopulationDensityMap.value({ x, y });

                if (z > 300)
                {
                    z = 300;
                }

                z -= PoliceStationMap.value({ x / 4, y / 4 });

                if (z > 250)
                {
                    z = 250;
                }

                if (z < 0)
                {
                    z = 0;
                }

                CrimeMap.value({ x, y }) = z;
                totz += z;

                if ((z > cmax) || ((z == cmax) && (!(Rand16() & 3))))
                {
                    cmax = z;
                    CrimeMax = { x * 2, y * 2 };
                }
            }
            else
            {
                CrimeMap.value({ x, y }) = 0;
            }
        }
    }

    if (numz)
    {
        CrimeAverage = totz / numz;
    }
    else
    {
        CrimeAverage = 0;
    }

    PoliceProtectionMap = PoliceStationMap;

    NewMapFlags[DYMAP] = NewMapFlags[CRMAP] = NewMapFlags[POMAP] = 1;
}
