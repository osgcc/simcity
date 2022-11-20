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
#include "main.h"

#include "Map.h"

#include "s_alloc.h"
#include "s_msg.h"

/* Power Scan */


int PowerStackNum;
char PowerStackX[PWRSTKSIZE], PowerStackY[PWRSTKSIZE];
int MaxPower, NumPower;


bool MoveMapSim(int MDir)
{
    switch (MDir)
    {
    case 0:
        if (SimulationLocation.y > 0)
        {
            SimulationLocation.y--;
            return true;
        }
        if (SimulationLocation.y < 0)
        {
            SimulationLocation.y = 0;
        }
        return false;

    case 1:
        if (SimulationLocation.x < (SimWidth - 1))
        {
            SimulationLocation.x++;
            return true;
        }
        if (SimulationLocation.x > (SimWidth - 1))
        {
            SimulationLocation.x = SimWidth - 1;
        }
        return false;

    case 2:
        if (SimulationLocation.y < (SimHeight - 1))
        {
            SimulationLocation.y++;
            return true;
        }
        if (SimulationLocation.y > (SimHeight - 1))
        {
            SimulationLocation.y = SimHeight - 1;
        }
        return false;

    case 3:
        if (SimulationLocation.x > 0)
        {
            SimulationLocation.x--;
            return true;
        }
        if (SimulationLocation.x < 0)
        {
            SimulationLocation.x = 0;
        }
        return false;

    case 4:
        return true;
    }

    return false;
}


void SetPowerBit()
{
    /* XXX: assumes 120x100 */
    int PowerWrd = (SimulationLocation.x / 16) + (SimulationLocation.y * 8);
    PowerMap[PowerWrd] |= 1 << (SimulationLocation.x & 15);
}


bool TestPowerBit(const int x, const int y)
{
    const auto tile = maskedTileValue(x, y);
    if ((tile == NUCLEAR) || (tile == POWERPLANT))
    {
        return true;
    }

    /* XXX: assumes 120x100 */
    int PowerWrd = (x / 16) + (y * 8);

    if (PowerWrd >= PowerMapSize)
    {
        return false;
    }

    return ((PowerMap[PowerWrd] & (1 << (x & 15))) ? true : false);
}


bool TileIsConductive(int TFDir)
{
    Point<int> saved = SimulationLocation;

    if (MoveMapSim(TFDir))
    {
        if ((Map[SimulationLocation.x][SimulationLocation.y] & CONDBIT) && (!TestPowerBit(SimulationLocation.x, SimulationLocation.y)))
        {
            SimulationLocation = saved;
            return true;
        }
    }

    SimulationLocation = saved;

    return false;
}


void PushPowerStack()
{
    if (PowerStackNum < (PWRSTKSIZE - 2))
    {
        PowerStackNum++;
        PowerStackX[PowerStackNum] = SimulationLocation.x;
        PowerStackY[PowerStackNum] = SimulationLocation.y;
    }
}


void PullPowerStack()
{
    if (PowerStackNum > 0)
    {
        SimulationLocation = { PowerStackX[PowerStackNum], PowerStackY[PowerStackNum] };
        PowerStackNum--;
    }
}


void DoPowerScan()
{
    for (int x = 0; x < PowerMapSize; x++) // ClearPowerMem
    {
        PowerMap[x] = 0;
    }

    MaxPower = (CoalPop * 700L) + (NuclearPop * 2000L); // post release
    NumPower = 0;

    int ConNum{};
    while (PowerStackNum)
    {
        PullPowerStack();
        
        int ADir = 4;
        do
        {
            if (++NumPower > MaxPower)
            {
                SendMes(NotificationId::BrownoutsReported);
                return;
            }
            
            MoveMapSim(ADir);
            SetPowerBit();

            ConNum = 0;
            int Dir = 0;
            while ((Dir < 4) && (ConNum < 2))
            {
                if (TileIsConductive(Dir))
                {
                    ConNum++;
                    ADir = Dir;
                }
                Dir++;
            }
            if (ConNum > 1)
            {
                PushPowerStack();
            }
        } while (ConNum);
    }
}
