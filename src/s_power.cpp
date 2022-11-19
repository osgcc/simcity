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
        if (SMapY > 0)
        {
            SMapY--;
            return true;
        }
        if (SMapY < 0)
        {
            SMapY = 0;
        }
        return false;

    case 1:
        if (SMapX < (SimWidth - 1))
        {
            SMapX++;
            return true;
        }
        if (SMapX > (SimWidth - 1))
        {
            SMapX = SimWidth - 1;
        }
        return false;

    case 2:
        if (SMapY < (SimHeight - 1))
        {
            SMapY++;
            return true;
        }
        if (SMapY > (SimHeight - 1))
        {
            SMapY = SimHeight - 1;
        }
        return false;

    case 3:
        if (SMapX > 0)
        {
            SMapX--;
            return true;
        }
        if (SMapX < 0)
        {
            SMapX = 0;
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
    int PowerWrd = (SMapX >> 4) + (SMapY << 3);
    PowerMap[PowerWrd] |= 1 << (SMapX & 15);
}


bool TestPowerBit(const int x, const int y)
{
    const auto tile = maskedTileValue(x, y);
    if ((tile == NUCLEAR) || (tile == POWERPLANT))
    {
        return true;
    }

    /* XXX: assumes 120x100 */
    //int PowerWrd = (SMapX >> 4) + (SMapY << 3);
    int PowerWrd = (x / 16) + (y * 8);

    if (PowerWrd >= PWRMAPSIZE)
    {
        return false;
    }

    return ((PowerMap[PowerWrd] & (1 << (x & 15))) ? 1 : 0);
}


int TestForCond(int TFDir)
{
    int xsave = SMapX;
    int ysave = SMapY;

    if (MoveMapSim(TFDir))
    {
        if ((Map[SMapX][SMapY] & CONDBIT) && (!TestPowerBit(SMapX, SMapY)))
        {
            SMapX = xsave;
            SMapY = ysave;

            return true;
        }
    }

    SMapX = xsave;
    SMapY = ysave;

    return false;
}


void PushPowerStack()
{
    if (PowerStackNum < (PWRSTKSIZE - 2))
    {
        PowerStackNum++;
        PowerStackX[PowerStackNum] = SMapX;
        PowerStackY[PowerStackNum] = SMapY;
    }
}


void PullPowerStack()
{
    if (PowerStackNum > 0)
    {
        SMapX = PowerStackX[PowerStackNum];
        SMapY = PowerStackY[PowerStackNum];
        PowerStackNum--;
    }
}


void DoPowerScan()
{
    for (int x = 0; x < PWRMAPSIZE; x++) // ClearPowerMem
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

            PowerMap[(SMapX >> 4) + (SMapY << 3)] |= 1 << (SMapX & 15);

            ConNum = 0;
            int Dir = 0;
            while ((Dir < 4) && (ConNum < 2))
            {
                if (TestForCond(Dir))
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
