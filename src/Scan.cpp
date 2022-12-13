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
#include "Scan.h"

#include "EffectMap.h"
#include "Map.h"

#include "s_alloc.h"
#include "s_sim.h"

#include "Vector.h"
#include "Zone.h"

#include <algorithm>


namespace
{
    bool NewMap{ false };

    Point<int> PollutionMax;
    Point<int> CrimeMax;

    Point<int> CityCenter;

    EffectMap tem({ HalfWorldWidth, HalfWorldHeight });
    EffectMap tem2({ HalfWorldWidth, HalfWorldHeight });

    EffectMap Qtem({ QuarterWorldWidth, QuarterWorldHeight });


    int getPollutionValue(int tileValue)
    {
        if (tileValue < POWERBASE)
        {
            if (tileValue >= HTRFBASE) /* heavy traf  */
            {
                return (/* 25 */ 75);
            }

            if (tileValue >= LTRFBASE) /* light traf  */
            {
                return (/* 10 */ 50);
            }

            if (tileValue < ROADBASE)
            {
                if (tileValue > FIREBASE)
                {
                    return (/* 60 */ 90);
                }

                /* XXX: Why negative pollution from radiation? */
                if (tileValue >= RADTILE) /* radioactivity  */
                {
                    return (/* -40 */ 255);
                }
            }
            return 0;
        }

        if (tileValue <= LASTIND)
        {
            return (0);
        }

        if (tileValue < PORTBASE) /* Ind  */
        {
            return (50);
        }

        if (tileValue <= LASTPOWERPLANT) /* prt, aprt, cpp */
        {
            return (/* 60 */ 100);
        }

        return 0;
    }


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


    int pollutionLevel(const Point<int>& point)
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
                        Qtem.value({ point.x / 2, point.y / 2 }) += 15;
                        continue;
                    }

                    pollutionLevel += getPollutionValue(tile);
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

        PolluteAverage = pollutedTileCount ? pollutionTotal / pollutedTileCount : 0;
    }


    void pollutionScan()
    {
        for (int i{}; i < HalfWorldWidth * HalfWorldHeight; ++i)
        {
            const Point<int> coord{ i % HalfWorldWidth, i / HalfWorldWidth };
            tem.value(coord) = pollutionLevel(coord);
        }
    }


    void landValueScan()
    {
        int LVtot = 0;
        int LVnum = 0;
        for (int x{}; x < SimWidth * SimHeight; ++x)
        {

            const Point<int> coord{ x % SimWidth, x / SimWidth };
            const auto tile = maskedTileValue(coord.x, coord.y);
            if (tile < ROADBASE)
            {
                LandValueMap.value(coord.skewInverseBy({ 2, 2 })) = 0;
                continue;
            }

            int dis = 34 - distanceToCityCenter(coord.x / 2, coord.y / 2);
            dis = dis * 4;
            dis += TerrainMem.value(coord.skewInverseBy({ 4, 4 }));
            dis -= PollutionMap.value(coord.skewInverseBy({ 2, 2 }));

            if (CrimeMap.value(coord.skewInverseBy({ 2, 2 })) > 190)
            {
                dis -= 20;
            }

            dis = std::clamp(dis, 1, 250);

            LandValueMap.value(coord.skewInverseBy({ 2, 2 })) = dis;
            LVtot += dis;
            LVnum++;
        }

        LVAverage = LVnum ? LVtot / LVnum : 0;
    }


    int sumAdjacent(const Point<int>& location, const EffectMap& map)
    {
        int val{};

        if (location.x > 0)
        {
            val += map.value(location + Vector<int>{ -1, 0});
        }

        if (location.x < (map.dimensions().x - 1))
        {
            val += map.value(location + Vector<int>{ 1, 0});
        }

        if (location.y > 0)
        {
            val += map.value(location + Vector<int>{ 0, -1});
        }

        if (location.y < (map.dimensions().y - 1))
        {
            val += map.value(location + Vector<int>{ 0, 1});
        }

        return val;
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


void smoothStationMap(EffectMap& map)
{
    EffectMap temp({ map.dimensions().x , map.dimensions().y});

    for (int x = 0; x < map.dimensions().x; x++)
    {
        for (int y = 0; y < map.dimensions().y; y++)
        {
            int edge = sumAdjacent({ x, y }, map);

            edge = (edge / 4) + map.value({ x, y });
            temp.value({ x, y }) = edge / 2;
        }
    }

    map = temp;
}


void smoothEffectMap(const EffectMap& src, EffectMap& dst)
{
    if (src.dimensions() != dst.dimensions())
    {
        throw std::runtime_error("smoothEffectMap(): Source and Destination array dimensions do not match.");
    }

    for (int x{}; x < src.dimensions().x; ++x)
    {
        for (int y{}; y < src.dimensions().y; ++y)
        {
            const auto val = sumAdjacent({ x, y }, src);
            dst.value({ x, y }) = std::clamp((val + src.value({ x, y })) / 4, 0, 255);
        }
    }
}


void smoothTerrain()
{
    for (int x{}; x < QuarterWorldWidth; ++x)
    {
        for (int y{}; y < QuarterWorldHeight; ++y)
        {
            const int val = Qtem.value({ x, y });
            int z = sumAdjacent({ x, y }, Qtem);
            TerrainMem.value({ x, y }) = (((z / 4) + val) / 2) % 256;
        }
    }
}


/*
 * Make firerate map from firestation map
 */
void fireAnalysis()
{
    smoothStationMap(FireStationMap);
    smoothStationMap(FireStationMap);
    smoothStationMap(FireStationMap);

    FireProtectionMap = FireStationMap;
}


int getPopulationDensity(int tile)
{
    if (tile == FREEZ)
    {
        return DoFreePop();
    }

    if (tile < COMBASE)
    {
        return residentialZonePopulation(tile);
    }

    if (tile < INDBASE)
    {
        return commercialZonePopulation(tile) * 8;
    }

    if (tile < PORTBASE)
    {
        return industrialZonePopulation(tile) * 8;
    }

    return 0;
}


void distIntMarket()
{
    for (int x{}; x < EighthWorldWidth; x++)
    {
        for (int y{}; y < EighthWorldHeight; y++)
        {
            int z{ distanceToCityCenter(x * 4 , y * 4) };
            z *= 4;
            z = 64 - z;
            ComRate.value({ x, y }) = z;
        }
    }
}


/*
 * sets: PopulationDensityMap, , , ComRate 
 */
void scanPopulationDensity()
{
    tem.reset();

    Vector<int> axisTotal{};
    int zoneCount{};

    for (int x{}; x < SimWidth; ++x)
    {
        for (int y{}; y < SimHeight; ++y)
        {
            int tile = tileValue({ x, y });
            if (tile & ZONEBIT)
            {
                tile = tile & LOMASK;
                tile = std::clamp(getPopulationDensity(tile) * 8, 0, 254);
                tem.value({ x / 2, y / 2 }) = tile;
                axisTotal += { x, y };
                zoneCount++;
            }
        }
    }

    smoothEffectMap(tem, tem2);
    smoothEffectMap(tem2, tem);
    smoothEffectMap(tem, tem2);

    PopulationDensityMap = tem2 * 2;

    distIntMarket(); /* set ComRate w/ (/ComMap) */

    // Set center of mass for the city
    zoneCount ? CityCenter = { axisTotal.x / zoneCount, axisTotal.y / zoneCount } : CityCenter = { HalfWorldWidth, HalfWorldHeight };
}


void pollutionAndLandValueScan()
{
    Qtem.fill(0);

    pollutionScan();
    landValueScan();

    smoothEffectMap(tem, tem2);
    smoothEffectMap(tem2, tem);

    setMostPollutedLocation();

    smoothTerrain();
}


void crimeScan()
{
    smoothStationMap(PoliceStationMap);
    smoothStationMap(PoliceStationMap);
    smoothStationMap(PoliceStationMap);

    int totz{};
    int numz{};
    int cmax{};
    for (int x{}; x < HalfWorldWidth; ++x)
    {
        for (int y{}; y < HalfWorldHeight; ++y)
        {
            int landValue = LandValueMap.value({ x, y });
            if (landValue == 0)
            {
                CrimeMap.value({ x, y }) = 0;
                continue;
            }

            ++numz;

            landValue = 128 - landValue;
            landValue = std::clamp(landValue + PopulationDensityMap.value({ x, y }), 0, 300);
            landValue = std::clamp(landValue - PoliceStationMap.value({ x / 4, y / 4 }), 0, 250);

            CrimeMap.value({ x, y }) = landValue;
            totz += landValue;

            if ((landValue > cmax) || ((landValue == cmax) && (!(Rand16() & 3))))
            {
                cmax = landValue;
                CrimeMax = { x * 2, y * 2 };
            }
        }
    }

    (numz > 0) ? CrimeAverage = (totz / numz) : CrimeAverage = 0;

    PoliceProtectionMap = PoliceStationMap;
}
