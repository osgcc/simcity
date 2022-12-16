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
#include "Traffic.h"

#include "Map.h"
#include "Sprite.h"

#include "s_alloc.h"
#include "w_util.h"

#include <algorithm>
#include <array>
#include <stack>


namespace
{
    constexpr auto MaxDistance = 30;

    std::stack<Point<int>> CoordinatesStack;

    int Zsource;

    const std::array<Vector<int>, 12> ZonePerimeterOffset =
    { {
        { -1, -2 },
        {  0, -2 },
        {  1, -2 },
        {  2, -1 },
        {  2,  0 },
        {  2,  1 },
        {  1,  2 },
        {  0,  2 },
        { -1,  2 },
        { -2,  1 },
        { -2,  0 },
        { -2, -1 }
    } };

    const std::array<Vector<int>, 4> AdjacentVector
    { {
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 }
    } };


    void pushCoordinates(const Point<int> coordinates)
    {
        CoordinatesStack.push(coordinates);
    }

    void popCoordinates()
    {
        CoordinatesStack.pop();
    }

    void resetCoordinatesStack()
    {
        while (!CoordinatesStack.empty())
        {
            CoordinatesStack.pop();
        }
    }

    void updateTrafficDensityMap()
    {
        while (!CoordinatesStack.empty())
        {
            popCoordinates();
            if (CoordinatesValid(SimulationTarget))
            {
                int tile = maskedTileValue(SimulationTarget);
                if ((tile >= ROADBASE) && (tile < POWERBASE))
                {
                    /* check for rail */
                    const Point<int> trafficDensityMapCoordinates = SimulationTarget.skewInverseBy({ 2, 2 });
                    tile = TrafficDensityMap.value(trafficDensityMapCoordinates);
                    tile += 50;

                    if ((tile > ResidentialBase) && (RandomRange(0, 5) == 0))
                    {
                        tile = ResidentialBase;

                        SimSprite* sprite = getSprite(SimSprite::Type::Helicopter);
                        if (sprite)
                        {
                            sprite->destination = SimulationTarget.skewBy({ 16, 16 });
                        }
                    }

                    TrafficDensityMap.value(trafficDensityMapCoordinates) = tile;
                }
            }
        }
    }

    int adjacentTile(size_t i)
    {
        const Point<int> coordinates{ SimulationTarget + AdjacentVector[i] };
        return CoordinatesValid(coordinates) ? maskedTileValue(coordinates) : 0;
    }

    bool tryGo(int distance)
    {
        size_t lastDirection = 5;
        const int startDirection = RandomRange(0, 3);

        for (size_t count = startDirection; count < (startDirection + AdjacentVector.size()); count++)
        {
            const size_t direction = count % AdjacentVector.size();

            // skip last count
            if (direction == lastDirection)
            {
                continue;
            }

            if (tileIsRoad(SimulationTarget + AdjacentVector[direction]))
            {
                MoveSimulationTarget(static_cast<SearchDirection>(direction));
                lastDirection = (direction + 2) % AdjacentVector.size();

                // save coordinates every other move
                if (count % 2)
                {
                    pushCoordinates(SimulationTarget);
                }

                return true;
            }
        }

        return false;
    }

    bool driveDone()
    {
        static int TARGL[3] = { COMBASE, LHTHR, LHTHR };
        static int TARGH[3] = { NUCLEAR, PORT, COMBASE }; // for destinations

        // R>C C>I I>R
        for (int i{}; i < AdjacentVector.size(); ++i)
        {
            const int tile = adjacentTile(i);

            if ((tile >= TARGL[Zsource]) && (tile <= TARGH[Zsource]))
            {
                return true;
            }
        }

        return false;
    }

    bool tryDrive()
    {
        for (int distance{}; distance < MaxDistance; ++distance)
        {
            // if it got a road
            if (tryGo(distance))
            {
                if (driveDone())
                {
                    return true;
                }
            }
            else
            {
                // deadend, backup
                if (!CoordinatesStack.empty())
                {
                    distance += 3;
                }
                else // give up at start
                {
                    return false;
                }
            }
        }

        // MaxDistance reached
        return false;
    }
}


// look for road on edges of zone
bool roadOnZonePerimeter()
{
    for (int i{}; i < ZonePerimeterOffset.size(); ++i)
    {
        const Point<int> coordinates = SimulationTarget + ZonePerimeterOffset[i];
        if (CoordinatesValid(coordinates))
        {
            if (tileIsRoad(coordinates))
            {
                SimulationTarget = coordinates;
                return true;
            }
        }
    }

    return false;
}


TrafficResult makeTraffic(int Zt)
{
    const auto simLocation = SimulationTarget;

    Zsource = Zt;
    resetCoordinatesStack();

    if (roadOnZonePerimeter()) // look for road on zone perimeter
    {
        if (tryDrive()) // attempt to drive somewhere
        {
            updateTrafficDensityMap(); // if sucessful, inc trafdensity
            SimulationTarget = simLocation;
            return TrafficResult::RouteFound; // traffic passed
        }

        SimulationTarget = simLocation;
        return TrafficResult::RouteNotFound; // traffic failed
    }
    else // no road found
    {
        return TrafficResult::NoTransportNearby;
    }
}
