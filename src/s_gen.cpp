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

#include "CityProperties.h"
#include "Map.h"

#include "s_alloc.h"
#include "s_sim.h"

#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"


#include <algorithm>
#include <limits>

/* Generate Map */


#define WATER_LOW	RIVER /* 2 */
#define WATER_HIGH	LASTRIVEDGE /* 20 */
#define WOODS_LOW	TREEBASE /* 21 */
#define WOODS_HIGH	UNUSED_TRASH2 /* 39 */


constexpr auto RADIUS = 18;


int XStart, YStart, MapX, MapY;
int Dir, LastDir;
int TreeLevel = -1;		/* level for tree creation */
int LakeLevel = -1;		/* level for lake creation */
int CurveLevel = -1;		/* level for river curviness */
bool CreateIsland = false;


int ERand(int limit)
{
  int x, z;

  z = RandomRange(0, limit);
  x = RandomRange(0, limit);
  
  return std::min(x, z);
}


bool IsTree(int cell)
{
    return (((cell & LOMASK) >= WOODS_LOW) && ((cell & LOMASK) <= WOODS_HIGH));
}


void ClearMap()
{
    for (int x = 0; x < SimWidth; x++)
    {
        for (int y = 0; y < SimHeight; y++)
        {
            Map[x][y] = DIRT;
        }
    }
}


void ClearUnnatural()
{
    for (int x = 0; x < SimWidth; x++)
    {
        for (int y = 0; y < SimHeight; y++)
        {
            if (Map[x][y] > WOODS)
            {
                Map[x][y] = DIRT;
            }
        }
    }
}


void MoveMap(int dir)
{
    static int DirTab[2][8] =
    {
        { 0, 1, 1, 1, 0, -1, -1, -1},
        {-1,-1, 0, 1, 1,  1,  0, -1}
    };

    MapX += DirTab[0][dir & 7];
    MapY += DirTab[1][dir & 7];
}


void PutOnMap(int Mchar, int Xoff, int Yoff)
{
    if (Mchar == 0)
    {
        return;
    }

    int Xloc = MapX + Xoff;
    int Yloc = MapY + Yoff;

    if (!CoordinatesValid({ Xloc, Yloc }))
    {
        return;
    }

    int temp = 0;
    if (temp = Map[Xloc][Yloc])
    {
        temp = temp & LOMASK;
        if (temp == RIVER)
        {
            if (Mchar != CHANNEL)
            {
                return;
            }
        }
        if (temp == CHANNEL)
        {
            return;
        }
    }

    Map[Xloc][Yloc] = Mchar;
}


void SmoothTrees()
{
    static int DX[4] = { -1, 0, 1, 0 };
    static int DY[4] = { 0, 1, 0,-1 };

    static int TEdTab[16] =
    {
        0, 0, 0, 34,
        0, 0, 36, 35,
        0, 32, 0, 33,
        30, 31, 29, 37
    };

    for (int MapX = 0; MapX < SimWidth; MapX++)
    {
        for (int MapY = 0; MapY < SimHeight; MapY++)
        {
            if (IsTree(Map[MapX][MapY]))
            {
                int bitindex = 0;
                for (int z = 0; z < 4; z++)
                {
                    bitindex = bitindex << 1;

                    int Xtem = MapX + DX[z];
                    int Ytem = MapY + DY[z];

                    if (CoordinatesValid({ Xtem, Ytem }) && IsTree(Map[Xtem][Ytem]))
                    {
                        bitindex++;
                    }
                }

                int temp = TEdTab[bitindex & 15];

                if (temp)
                {
                    if (temp != WOODS)
                    {
                        if ((MapX + MapY) & 1)
                        {
                            temp = temp - 8;
                        }
                    }
                    Map[MapX][MapY] = temp + BLBNBIT;
                }
                else
                {
                    Map[MapX][MapY] = temp;
                }
            }
        }
    }
}


void SmoothRiver()
{
    static int DX[4] = { -1, 0, 1, 0 };
    static int DY[4] = { 0, 1, 0,-1 };

    static int REdTab[16] =
    {
        13 + BULLBIT, 13 + BULLBIT, 17 + BULLBIT, 15 + BULLBIT,
        5 + BULLBIT, 2, 19 + BULLBIT, 17 + BULLBIT,
        9 + BULLBIT, 11 + BULLBIT, 2, 13 + BULLBIT,
        7 + BULLBIT, 9 + BULLBIT, 5 + BULLBIT, 2
    };

    for (int MapX = 0; MapX < SimWidth; MapX++)
    {
        for (int MapY = 0; MapY < SimHeight; MapY++)
        {
            if (Map[MapX][MapY] == REDGE)
            {
                int bitindex = 0;

                for (int z = 0; z < 4; z++)
                {
                    bitindex = bitindex << 1;
                    int Xtem = MapX + DX[z];
                    int Ytem = MapY + DY[z];
                    if (CoordinatesValid({ Xtem, Ytem }) &&
                        ((Map[Xtem][Ytem] & LOMASK) != DIRT) &&
                        (((Map[Xtem][Ytem] & LOMASK) < WOODS_LOW) ||
                            ((Map[Xtem][Ytem] & LOMASK) > WOODS_HIGH)))
                    {
                        bitindex++;
                    }

                }

                int temp = REdTab[bitindex & 15];

                if ((temp != RIVER) && (RandomRange(0, 1)))
                {
                    temp++;
                }

                Map[MapX][MapY] = temp;
            }
        }
    }
}



void TreeSplash(int xloc, int yloc)
{
    int dis = TreeLevel < 0 ? dis = RandomRange(0, 150) + 50 : RandomRange(0, 100 + (TreeLevel * 2)) + 50;

    MapX = xloc;
    MapY = yloc;

    for (int z = 0; z < dis; z++)
    {
        int dir = RandomRange(0, 7);

        MoveMap(dir);

        if (!(CoordinatesValid({ MapX, MapY })))
        {
            return;
        }

        if ((Map[MapX][MapY] & LOMASK) == DIRT)
        {
            Map[MapX][MapY] = WOODS + BLBNBIT;
        }
    }
}


void DoTrees()
{
    int Amount, x, xloc, yloc;

    if (TreeLevel < 0)
    {
        Amount = RandomRange(0, 100) + 50;
    }
    else
    {
        Amount = TreeLevel + 3;
    }

    for (x = 0; x < Amount; x++)
    {
        xloc = RandomRange(0, SimWidth - 1);
        yloc = RandomRange(0, SimHeight - 1);
        TreeSplash(xloc, yloc);
    }

    SmoothTrees();
    SmoothTrees();
}


void PlopLargeRiver()
{
    static int BRMatrix[9][9] =
    {
        { 0, 0, 0, 3, 3, 3, 0, 0, 0 },
        { 0, 0, 3, 2, 2, 2, 3, 0, 0 },
        { 0, 3, 2, 2, 2, 2, 2, 3, 0 },
        { 3, 2, 2, 2, 2, 2, 2, 2, 3 },
        { 3, 2, 2, 2, 4, 2, 2, 2, 3 },
        { 3, 2, 2, 2, 2, 2, 2, 2, 3 },
        { 0, 3, 2, 2, 2, 2, 2, 3, 0 },
        { 0, 0, 3, 2, 2, 2, 3, 0, 0 },
        { 0, 0, 0, 3, 3, 3, 0, 0, 0 }
    };

    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            PutOnMap(BRMatrix[y][x], x, y);
        }
    }
}


void PlopSmallRiver(void)
{
    static int SRMatrix[6][6] =
    {
        { 0, 0, 3, 3, 0, 0 },
        { 0, 3, 2, 2, 3, 0 },
        { 3, 2, 2, 2, 2, 3 },
        { 3, 2, 2, 2, 2, 3 },
        { 0, 3, 2, 2, 3, 0 },
        { 0, 0, 3, 3, 0, 0 }
    };

    for (int x = 0; x < 6; x++)
    {
        for (int y = 0; y < 6; y++)
        {
            PutOnMap(SRMatrix[y][x], x, y);
        }
    }
}


void DoLargeRiver()
{
    int r1, r2;

    if (CurveLevel < 0)
    {
        r1 = 100;
        r2 = 200;
    }
    else
    {
        r1 = CurveLevel + 10;
        r2 = CurveLevel + 100;
    }

    while (CoordinatesValid({ MapX + 4, MapY + 4 }))
    {
        PlopLargeRiver();
        if (RandomRange(0, r1) < 10)
        {
            Dir = LastDir;
        }
        else
        {
            if (RandomRange(0, r2) > 90) Dir++;
            if (RandomRange(0, r2) > 90) Dir--;
        }
        MoveMap(Dir);
    }
}


void DoSmallRiver()
{
    int r1, r2;

    if (CurveLevel < 0)
    {
        r1 = 100;
        r2 = 200;
    }
    else
    {
        r1 = CurveLevel + 10;
        r2 = CurveLevel + 100;
    }

    while (CoordinatesValid({ MapX + 3, MapY + 3 }))
    {
        PlopSmallRiver();
        if (RandomRange(0, r1) < 10)
        {
            Dir = LastDir;
        }
        else
        {
            if (RandomRange(0, r2) > 90) Dir++;
            if (RandomRange(0, r2) > 90) Dir--;
        }
        MoveMap(Dir);
    }
}


void DoRivers()
{
    LastDir = RandomRange(0, 3);
    Dir = LastDir;   
    DoLargeRiver();
    
    MapX = XStart;
    MapY = YStart;
    LastDir = LastDir ^ 4;
    Dir = LastDir;
    DoLargeRiver();

    MapX = XStart;
    MapY = YStart;
    LastDir = RandomRange(0, 3);
    DoSmallRiver();
}


void MakeNakedIsland()
{
    for (int x = 0; x < SimWidth; x++)
    {
        for (int y = 0; y < SimHeight; y++)
        {
            Map[x][y] = RIVER;
        }
    }
    
    for (int x = 5; x < SimWidth - 5; x++)
    {
        for (int y = 5; y < SimHeight - 5; y++)
        {
            Map[x][y] = DIRT;
        }
    }
   
    for (int x = 0; x < SimWidth - 5; x += 2)
    {
        MapX = x;
        MapY = ERand(RADIUS);
        PlopLargeRiver();

        MapY = (SimHeight - 10) - ERand(RADIUS);
        PlopLargeRiver();

        MapY = ERand(RADIUS);
        PlopSmallRiver();

        MapY = (SimHeight - 6) - ERand(RADIUS);
        PlopSmallRiver();
    }

    for (int y = 0; y < SimHeight - 5; y += 2)
    {
        MapY = y;
        MapX = ERand(RADIUS);
        PlopLargeRiver();

        MapX = (SimWidth - 10) - ERand(RADIUS);
        PlopLargeRiver();

        MapX = ERand(RADIUS);
        PlopSmallRiver();

        MapX = (SimWidth - 6) - ERand(RADIUS);
        PlopSmallRiver();
    }
}


void MakeIsland()
{
    MakeNakedIsland();
    SmoothRiver();
    DoTrees();
}


void MakeLakes()
{
    int Lim1 = 0;
    int Lim2 = 0;

    if (LakeLevel < 0)
    {
        Lim1 = RandomRange(0, 10);
    }
    else
    {
        Lim1 = LakeLevel / 2;
    }

    for (int t = 0; t < Lim1; t++)
    {
        int  x = RandomRange(0, SimWidth - 21) + 10;
        int y = RandomRange(0, SimHeight - 20) + 10;

        Lim2 = RandomRange(0, 12) + 2;

        for (int z = 0; z < Lim2; z++)
        {
            MapX = x - 6 + RandomRange(0, 12);
            MapY = y - 6 + RandomRange(0, 12);

            if (RandomRange(0, 4))
            {
                PlopSmallRiver();
            }
            else
            {
                PlopLargeRiver();
            }
        }
    }
}


void GetRandStart()
{
    XStart = 40 + RandomRange(0, SimWidth - 80);
    YStart = 33 + RandomRange(0, SimHeight - 67);
    MapX = XStart;
    MapY = YStart;
}


void GenerateMap(int r)
{
    if (RandomRange(0, 100) < 10) // chance that island is generated
    {
        MakeIsland();
        return;
    }

    if (CreateIsland)
    {
        MakeNakedIsland();
    }

    GetRandStart();

    if (CurveLevel != 0)
    {
        DoRivers();
    }

    if (LakeLevel != 0)
    {
        MakeLakes();
    }

    SmoothRiver();

    if (TreeLevel != 0)
    {
        DoTrees();
    }
}

#include <iostream>
#include "s_alloc.h"

void GenerateSomeCity(int seed, CityProperties& properties, Budget& budget)
{
    ScenarioID = 0;
    CityTime = 0;
    InitSimLoad = 2;
    DoInitialEval = 0;

    initWillStuff();
    UpdateFunds(budget);
    DoSimInit(properties, budget);
    Eval("UIDidGenerateNewCity");

    GenerateMap(seed);
}


void GenerateNewCity(CityProperties& properties, Budget& budget)
{
    GenerateSomeCity(RandomRange(0, std::numeric_limits<int>::max()), properties, budget);
}
