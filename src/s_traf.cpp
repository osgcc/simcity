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
#include "s_power.h"
#include "s_sim.h"

#include "w_util.h"

#include "Sprite.h"


/* Traffic Generation */


#define MAXDIS 30

int PosStackN, SMapXStack[MAXDIS+1], SMapYStack[MAXDIS+1];
int LDir;
int Zsource;
int TrafMaxX, TrafMaxY;


/* comefrom: TryGo */
void PushPos()
{
  PosStackN++;
  SMapXStack[PosStackN] = SimulationLocation.x;
  SMapYStack[PosStackN] = SimulationLocation.y;
}


/* comefrom: SetTrafMem */
void PullPos()
{
  SimulationLocation.x = SMapXStack[PosStackN];
  SimulationLocation.y = SMapYStack[PosStackN];
  PosStackN--;
}


/* comefrom: MakeTraf */
void SetTrafMem()
{
    for (int x = PosStackN; x > 0; x--)
    {
        PullPos();
        if (CoordinatesValid(SimulationLocation.x, SimulationLocation.y, SimWidth, SimHeight))
        {
            int z = maskedTileValue(SimulationLocation.x, SimulationLocation.y);
            if ((z >= ROADBASE) && (z < POWERBASE))
            {
                /* check for rail */
                z = TrfDensity[SimulationLocation.x / 2][SimulationLocation.y / 2];
                z += 50;
                if ((z > 240) && (RandomRange(0, 5) == 0))
                {
                    z = 240;
                    TrafMaxX = SimulationLocation.x * 16;
                    TrafMaxY = SimulationLocation.y * 16;

                    SimSprite* sprite = GetSprite(SimSprite::Type::Helicopter);
                    if (sprite)
                    {
                        sprite->dest_x = TrafMaxX;
                        sprite->dest_y = TrafMaxY;
                    }
                }
                TrfDensity[SimulationLocation.x >> 1][SimulationLocation.y >> 1] = z;
            }
        }
    }
}


/* comefrom: TryGo FindPRoad */
bool RoadTest(const int x)
{
    int tile = x & LOMASK;
    if (tile < ROADBASE)
    {
        return false;
    }
    if (tile > LASTRAIL)
    {
        return false;
    }
    if ((tile >= POWERBASE) && (tile < RAILHPOWERV))
    {
        return false;
    }
    return true;
}


/* comefrom: DoSPZone MakeTraf */
bool FindPRoad()		/* look for road on edges of zone   */
{
  static int PerimX[12] = {-1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2};
  static int PerimY[12] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1};
  int tx, ty, z;

  for (z = 0; z < 12; z++) {
	  tx = SimulationLocation.x + PerimX[z];
	  ty = SimulationLocation.y + PerimY[z];
	  if (CoordinatesValid(tx, ty, SimWidth, SimHeight)) {
		  if (RoadTest(Map[tx][ty])) {
			  SimulationLocation.x = tx;
			  SimulationLocation.y = ty;
			  return true;
		  }
	  }
  }
  return false;
}


bool FindPTele()		/* look for telecommunication on edges of zone */
{
  static int PerimX[12] = {-1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2};
  static int PerimY[12] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1};
  int tx, ty, z, tile;

  for (z = 0; z < 12; z++) {
	  tx = SimulationLocation.x + PerimX[z];
	  ty = SimulationLocation.y + PerimY[z];
	  if (CoordinatesValid(tx, ty, SimWidth, SimHeight)) {
	  	  tile = Map[tx][ty] & LOMASK;
		  if ((tile >= TELEBASE) && (tile <= TELELAST)) {
			  return true;
		  }
	  }
  }
  return false;
}


/* comefrom: TryGo DriveDone */
int GetFromMap(int x)
{
    switch (x)
    {
    case 0:
        if (SimulationLocation.y > 0)
        {
            return (Map[SimulationLocation.x][SimulationLocation.y - 1] & LOMASK);
        }
        return 0;
    case 1:
        if (SimulationLocation.x < (SimWidth - 1))
        {
            return (Map[SimulationLocation.x + 1][SimulationLocation.y] & LOMASK);
        }
        return 0;

    case 2:
        if (SimulationLocation.y < (SimHeight - 1))
        {
            return (Map[SimulationLocation.x][SimulationLocation.y + 1] & LOMASK);
        }
        return 0;

    case 3:
        if (SimulationLocation.x > 0)
        {
            return (Map[SimulationLocation.x - 1][SimulationLocation.y] & LOMASK);
        }
        return 0;

    default:
        return 0;
    }
}


/* comefrom: TryDrive */
bool TryGo(int z)
{
    int rdir = RandomRange(0, 4);
    for (int x = rdir; x < (rdir + 4); x++) // for the 4 directions
    {
        int realdir = x % 4;
        if (realdir == LDir) // skip last direction
        {
            continue;
        }
        if (RoadTest(GetFromMap(realdir)))
        {
            MoveMapSim(realdir);
            LDir = (realdir + 2) % 4;
            if (z & 1) // save pos every other move
            {
                PushPos();
            }
            return true;
        }
    }
    return false;
}


/* comefrom: TryDrive */
bool DriveDone()
{
    static int TARGL[3] = { COMBASE, LHTHR, LHTHR };
    static int TARGH[3] = { NUCLEAR, PORT, COMBASE };	/* for destinations */
    //int l, h;

    for (int x = 0; x < 4; x++) /* R>C C>I I>R  */
    {
        int z = GetFromMap(x);
        if ((z >= TARGL[Zsource]) && (z <= TARGH[Zsource]))
        {
            return true;
        }
    }

    return false;
}


bool TryDrive()
{
    LDir = 5;
    for (int z = 0; z < MAXDIS; ++z) // Maximum distance to try
    {
        if (TryGo(z)) // if it got a road
        {
            if (DriveDone()) // if destination is reached
            {
                return true; // pass
            }
        }
        else
        {
            if (PosStackN) // deadend , backup
            {
                PosStackN--;
                z += 3;
            }
            else // give up at start
            {
                return false;
            }
        }
    }

    return false; // gone maxdis
}


/* comefrom: DoIndustrial DoCommercial DoResidential */
int MakeTraf(int Zt)
{
    int xtem = SimulationLocation.x;
    int ytem = SimulationLocation.y;
    Zsource = Zt;
    PosStackN = 0;

    if (FindPRoad()) // look for road on zone perimeter
    {
        if (TryDrive()) // attempt to drive somewhere
        {
            SetTrafMem(); // if sucessful, inc trafdensity
            SimulationLocation.x = xtem;
            SimulationLocation.y = ytem;
            return 1; // traffic passed
        }
        SimulationLocation.x = xtem;
        SimulationLocation.y = ytem;
        return 0; // traffic failed
    }
    else // no road found
    {
        return -1;
    }
}
