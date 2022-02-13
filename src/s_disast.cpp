/* s_disast.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
#include "main.h"

#include "s_alloc.h"
#include "s_sim.h"

#include "s_msg.h"

#include "w_stubs.h"
#include "w_util.h"

#include <algorithm>



#define TILE_IS_NUCLEAR(tile) \
	((tile & LOMASK) == NUCLEAR)

#define TILE_IS_VULNERABLE(tile) \
	(!(tile & ZONEBIT) && \
	 ((tile & LOMASK) >= RBRDR) && \
	 ((tile & LOMASK) <= LASTZONE))

#define TILE_IS_ARSONABLE(tile) \
	(!(tile & ZONEBIT) && \
	 ((tile & LOMASK) >= RBRDR) && \
	 ((tile & LOMASK) <= LASTZONE))

#define TILE_IS_RIVER_EDGE(tile) \
	(((tile & LOMASK) >= FIRSTRIVEDGE) && \
	 ((tile & LOMASK) <= LASTRIVEDGE))

#define TILE_IS_FLOODABLE(tile) \
	((tile == DIRT) || \
	 ((tile & BULLBIT) && \
	  (tile & BURNBIT)))

#define TILE_IS_RUBBLE(tile) \
	 (((tile & LOMASK) >= RUBBLE) && \
	  ((tile & LOMASK) <= LASTRUBBLE)))

#define TILE_IS_FLOODABLE2(tile) \
	((tile == 0) || \
	 (tile & BURNBIT) || \
         TILE_IS_RUBBLE(tile))





int ShakeNow;
int FloodCount;
int FloodX, FloodY;

// fordward declare from w_stubs
void DropFireBombs();

// forward declare from s_sim
void DoMeltdown(int, int);

/// Forward declares from w_sprite
void MakeMonster();
void MakeTornado();
void MakeExplosion(int, int);

// forward declare from w_tk
void DoEarthQuake();


void MakeMeltdown()
{
    for (int x = 0; x < (SimWidth - 1); x++)
    {
        for (int y = 0; y < (SimHeight - 1); y++)
        {
            /* TILE_IS_NUCLEAR(Map[x][y]) */
            if ((Map[x][y] & LOMASK) == NUCLEAR)
            {
                DoMeltdown(x, y);
                return;
            }
        }
    }
}


void FireBomb()
{
    CrashX = RandomRange(0, SimWidth - 1);
    CrashY = RandomRange(0, SimHeight - 1);
    MakeExplosion(CrashX, CrashY);
    ClearMes();
    SendMesAt(MessageEnumerator::FirebombingReported, CrashX, CrashY);
}


bool Vulnerable(int tem)
{
    int tem2 = tem & LOMASK;
    return !(tem2 < RESBASE) || (tem2 > LASTZONE) || (tem & ZONEBIT);
}


void MakeEarthquake()
{
    DoEarthQuake();
    SendMesAt(MessageEnumerator::EarthquakeReported, CCx, CCy);

    int time = RandomRange(0, 700) + 300;

    for (int z = 0; z < time; z++)
    {
        int x = RandomRange(0, SimWidth - 1);
        int y = RandomRange(0, SimHeight - 1);
        if ((x < 0) || (x > (SimWidth - 1)) || (y < 0) || (y > (SimHeight - 1)))
        {
            continue;
        }
        /* TILE_IS_VULNERABLE(Map[x][y]) */
        if (Vulnerable(Map[x][y]))
        {
            if (z & 0x3)
            {
                Map[x][y] = (RUBBLE + BULLBIT) + (Rand16() & 3);
            }
            else
            {
                Map[x][y] = (FIRE + ANIMBIT) + (Rand16() & 7);
            }
        }
    }
}


void SetFire()
{
    int x = RandomRange(0, SimWidth - 1);
    int y = RandomRange(0, SimHeight - 1);
    int z = Map[x][y];

    /* TILE_IS_ARSONABLE(z) */
    if (!(z & ZONEBIT))
    {
        z = z & LOMASK;
        if ((z > LHTHR) && (z < LASTZONE))
        {
            Map[x][y] = FIRE + ANIMBIT + (Rand16() & 7);
            CrashX = x;
            CrashY = y;
            SendMesAt(MessageEnumerator::FireReported, x, y);
        }
    }
}


void MakeFire()
{
    for (int t = 0; t < 40; t++)
    {
        int x = RandomRange(0, SimWidth - 1);
        int y = RandomRange(0, SimHeight - 1);
        int z = Map[x][y];
        /* !(z & BURNBIT) && TILE_IS_ARSONABLE(z) */
        if ((!(z & ZONEBIT)) && (z & BURNBIT))
        {
            z = z & LOMASK;
            if ((z > 21) && (z < LASTZONE))
            {
                Map[x][y] = FIRE + ANIMBIT + (Rand16() & 7);
                SendMesAt(MessageEnumerator::FireReported, x, y);
                return;
            }
        }
    }
}


void MakeFlood()
{
    static int Dx[4] = { 0, 1, 0,-1 };
    static int Dy[4] = { -1, 0, 1, 0 };

    for (int z = 0; z < 300; z++)
    {
        int x = RandomRange(0, SimWidth - 1);
        int y = RandomRange(0, SimHeight - 1);
        int cell = Map[x][y] & LOMASK; /* XXX: & LOMASK */

        /* TILE_IS_RIVER_EDGE(c) */
        if ((cell > 4) && (cell < 21))		/* if riveredge  */
        {
            for (int t = 0; t < 4; t++)
            {
                int xx = x + Dx[t];
                int yy = y + Dy[t];
                if (TestBounds(xx, yy, SimWidth, SimHeight))
                {
                    cell = Map[xx][yy];
                    /* TILE_IS_FLOODABLE(c) */
                    if ((cell == 0) || ((cell & BULLBIT) && (cell & BURNBIT)))
                    {
                        Map[xx][yy] = FLOOD;
                        FloodCount = 30;
                        SendMesAt(MessageEnumerator::FloodingReported, xx, yy);
                        FloodX = xx;
                        FloodY = yy;
                        return;
                    }
                }
            }
        }
    }
}


void DoFlood()
{
    static int Dx[4] = { 0, 1, 0,-1 };
    static int Dy[4] = { -1, 0, 1, 0 };

    if (FloodCount)
    {
        for (int z = 0; z < 4; z++)
        {
            if (!(Rand16() & 7))
            {
                int xx = SMapX + Dx[z];
                int yy = SMapY + Dy[z];
                if (TestBounds(xx, yy, SimWidth, SimHeight))
                {
                    int cell = Map[xx][yy];
                    int terrain = cell & LOMASK;

                    /* TILE_IS_FLOODABLE2(c) */
                    if ((cell & BURNBIT) || (cell == 0) || ((terrain >= WOODS5 /* XXX */) && (terrain < FLOOD)))
                    {
                        if (cell & ZONEBIT)
                        {
                            FireZone(xx, yy, cell);
                        }
                        Map[xx][yy] = FLOOD + RandomRange(0, 2);
                    }
                }
            }
        }
    }
    else
    {
        if (!(Rand16() & 15))
        {
            Map[SMapX][SMapY] = 0;
        }
    }
}


void ScenarioDisaster()
{
    switch (DisasterEvent)
    {
    case 1:	// Dullsville
        break;
    case 2: // San Francisco
        if (DisasterWait == 1)
        {
            MakeEarthquake();
        }
        break;
    case 3: // Hamburg
        DropFireBombs();
        break;
    case 4: // Bern
        break;
    case 5: // Tokyo
        if (DisasterWait == 1)
        {
            MakeMonster();
        }
        break;
    case 6:	// Detroit
        break;
    case 7: // Boston
        if (DisasterWait == 1)
        {
            MakeMeltdown();
        }
        break;
    case 8:	// Rio
        if ((DisasterWait % 24) == 0)
        {
            MakeFlood();
        }
        break;
    }

    if (DisasterWait)
    {
        DisasterWait--;
    }
    else
    {
        DisasterEvent = 0;
    }
}


void DoDisasters()
{
    // Chance of disasters at lev 0 1 2
    static int DisChance[3] = { 10 * 48, 5 * 48, 60 };

    if (FloodCount)
    {
        FloodCount--;
    }

    if (DisasterEvent)
    {
        ScenarioDisaster();
    }

    if (NoDisasters)
    {
        return;
    }
    
    int disasterChance = DisChance[std::clamp(GameLevel(), 0, 2)];
    if (disasterChance == 0)
    {
        int disasterType = RandomRange(0, 8);
        switch (disasterType)
        {
        case 0:
        case 1:
            SetFire();
            break;
        case 2:
        case 3:
            MakeFlood();
            break;
        case 4:
            break;
        case 5:
            MakeTornado();
            break;
        case 6:
            MakeEarthquake();
            break;
        case 7:
        case 8:
            if (PolluteAverage > /* 80 */ 60)
            {
                MakeMonster();
            }
            break;
        }
    }
}
