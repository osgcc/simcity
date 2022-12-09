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

    std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> tem;
    std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> tem2;

    void resetTempArrays()
    {
        for (int x = 0; x < HalfWorldWidth; x++)
        {
            for (int y = 0; y < HalfWorldHeight; y++)
            {
                tem[x][y] = 0;
                tem2[x][y] = 0;
            }
        }
    }
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


/* comefrom: PopDenScan */
void ClrTemArray()
{
    int x, y, z;

    z = 0;
    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            tem[x][y] = z;
        }
    }
}


/* comefrom: PopDenScan */
int GetPDen(int Ch9)
{
    int pop;

    if (Ch9 == FREEZ)
    {
        pop = DoFreePop(/*Ch9*/);
        return (pop);
    }

    if (Ch9 < COMBASE)
    {
        pop = RZPop(Ch9);
        return (pop);
    }

    if (Ch9 < INDBASE)
    {
        pop = (CZPop(Ch9) << 3);
        return (pop);
    }

    if (Ch9 < PORTBASE)
    {
        pop = (IZPop(Ch9) << 3);
        return (pop);
    }

    return 0;
}


/* comefrom: PopDenScan */
/* smooths data in tem[x][y] into tem2[x][y]  */
void DoSmooth()
{
    int x, y, z;

    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            z = 0;
            if (x > 0)
            {
                z += tem[x - 1][y];
            }

            if (x < (HalfWorldWidth - 1))
            {
                z += tem[x + 1][y];
            }

            if (y > 0)
            {
                z += tem[x][y - 1];
            }

            if (y < (HalfWorldHeight - 1))
            {
                z += tem[x][y + 1];
            }

            tem2[x][y] = std::clamp((z + tem[x][y]) / 4, 0, 255);
        }
    }
}


/* comefrom: PopDenScan */
void DoSmooth2()        /* smooths data in tem2[x][y] into tem[x][y]  */
{
    int x, y, z;

    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            z = 0;
            if (x > 0)
            {
                z += tem2[x - 1][y];
            }

            if (x < (HalfWorldWidth - 1))
            {
                z += tem2[x + 1][y];
            }

            if (y > 0)
            {
                z += tem2[x][y - 1];
            }

            if (y < (HalfWorldHeight - 1))
            {
                z += tem2[x][y + 1];
            }

            tem[x][y] = std::clamp((z + tem2[x][y]) / 4, 0, 255);
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
    int x, y, z;

    ClrTemArray();

    Xtot = 0;
    Ytot = 0;
    Ztot = 0;
    for (x = 0; x < SimWidth; x++)
    {
        for (y = 0; y < SimHeight; y++)
        {
            z = Map[x][y];
            if (z & ZONEBIT)
            {
                z = z & LOMASK;
                SimulationTarget = { x, y };
                z = GetPDen(z) << 3;
                if (z > 254)
                {
                    z = 254;
                }

                tem[x >> 1][y >> 1] = z;
                
                Xtot += x;
                Ytot += y;
                Ztot++;
            }
        }
    }

    DoSmooth(); /* T1 -> T2 */
    DoSmooth2(); /* T2 -> T1 */
    DoSmooth(); /* T1 -> T2 */

    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            PopulationDensityMap.value({ x, y }) = tem2[x][y] << 1;
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
    int x, y, z;

    for (x = 0; x < QuarterWorldWidth; x++)
    {
        for (y = 0; y < QuarterWorldHeight; y++)
        {
            z = 0;
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


/* comefrom: Simulate SpecialInit */
/* Does pollution, terrain, land value   */
/**
 * fixme: Holy shit, this function does __way__ too much. This
 *        needs to be broken down into smaller chunks.  
 */
void PTLScan()
{
    int ptot, LVtot;
    int x, y, z, dis;
    int Plevel, LVflag, loc, Mx, My, pnum, LVnum, pmax;

    for (x = 0; x < QuarterWorldWidth; x++)
    {
        for (y = 0; y < QuarterWorldHeight; y++)
        {
            Qtem[x][y] = 0;
        }
    }

    LVtot = 0;
    LVnum = 0;
    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            Plevel = 0;
            LVflag = 0;
            int zx = x * 2;
            int zy = y * 2;
            for (Mx = zx; Mx <= zx + 1; Mx++)
            {
                for (My = zy; My <= zy + 1; My++)
                {
                    if (loc = (Map[Mx][My] & LOMASK))
                    {
                        if (loc < RUBBLE)
                        {
                            /* inc terrainMem */
                            Qtem[x >> 1][y >> 1] += 15;
                            continue;
                        }

                        Plevel += GetPValue(loc);
                        if (loc >= ROADBASE)
                        {
                            LVflag++;
                        }
                    }
                }
            }

            if (Plevel > 255)
            {
                Plevel = 255;
            }

            tem[x][y] = Plevel;

            if (LVflag) /* LandValue Equation */
            {
                dis = 34 - distanceToCityCenter(x, y);
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

    DoSmooth();
    DoSmooth2();

    pmax = 0;
    pnum = 0;
    ptot = 0;
    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            z = tem[x][y];
            PollutionMap.value({ x, y }) = z;

            if (z) /*  get pollute average  */
            {
                pnum++;
                ptot += z;

                /* find max pol for monster  */
                if ((z > pmax) || ((z == pmax) && (!(Rand16() & 3))))
                {
                    pmax = z;
                    PollutionMax = { x * 2, y * 2 };
                }
            }
        }
    }

    if (pnum)
    {
        PolluteAverage = ptot / pnum;
    }
    else
    {
        PolluteAverage = 0;
    }

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
