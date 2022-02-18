/* w_tool.c
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
#include "Map.h"

#include "w_tool.h"

#include "s_alloc.h"
#include "s_msg.h"
#include "s_sim.h"

#include "w_con.h"
#include "w_resrc.h"
#include "w_sound.h"
#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <stdexcept>

int specialBase = CHURCH;
int OverRide = 0;
int Expensive = 1000;
int Players = 1;
int Votes = 0;

Tool PendingTool{ Tool::None };


std::map<Tool, ToolProperties> Tools =
{
    { Tool::Residential, { 100, 3, 1, "Residential" }},
    { Tool::Commercial, { 100, 3, 1, "Commercial" }},
    { Tool::Industrial, { 100, 3, 1, "Industrial" }},
    { Tool::Fire, { 500, 3, 1, "Fire Department" }},
    { Tool::Query, { 0, 1, 0, "Query" }},
    { Tool::Police, { 500, 3, 1, "Police Department" }},
    { Tool::Wire, { 5, 1, 0, "Power Line" }},
    { Tool::Bulldoze, { 1, 1, 0, "Bulldoze" }},
    { Tool::Rail, { 20, 1, 0, "Rail" }},
    { Tool::Road, { 10, 1, 0, "Roads" }},
    { Tool::Stadium, { 5000, 4, 1, "Stadium" }},
    { Tool::Park, { 10, 1, 0, "Park" }},
    { Tool::Seaport, { 3000, 4, 1, "Seaport" }},
    { Tool::Coal, { 3000, 4, 1, "Coal Power" }},
    { Tool::Nuclear, { 5000, 4, 1, "Nuclear Power" }},
    { Tool::Airport, { 10000, 6, 1, "Airport" }},
    { Tool::Network, { 100, 1, 0, "Network" }},
    { Tool::None, { 0, 0, 0, "No Tool" } }
};


std::map<ToolResult, std::string> ToolResultStringTable =
{
    { ToolResult::CannotBulldoze, "Cannot Bulldoze" },
    { ToolResult::InsufficientFunds, "Insufficient Funds" },
    { ToolResult::InvalidLocation, "Invalid Location" },
    { ToolResult::InvalidOperation, "Invalid Operation" },
    { ToolResult::NetworkVotedNo, "Other players vetoed action" },
    { ToolResult::OutOfBounds, "Out of Bounds" },
    { ToolResult::RequiresBulldozing, "Bulldozing Required" },
    { ToolResult::Success, "Success!" }
};


ToolResult putDownPark(int mapH, int mapV)
{
    int tile{};

    if (TotalFunds() - Tools.at(Tool::Park).cost >= 0)
    {
        int value = RandomRange(0, 4);

        if (value == 4)
        {
            tile = FOUNTAIN | BURNBIT | BULLBIT | ANIMBIT;
        }
        else
        {
            tile = (value + WOODS2) | BURNBIT | BULLBIT;
        }

        if (Map[mapH][mapV] == 0)
        {
            Spend(Tools.at(Tool::Park).cost);
            UpdateFunds();
            Map[mapH][mapV] = tile;
            return ToolResult::Success;
        }

        return ToolResult::InvalidLocation;
    }

    return ToolResult::InsufficientFunds;
}


// Radar?
ToolResult putDownNetwork(int mapH, int mapV)
{
    int tile = Map[mapH][mapV] & LOMASK;

    if ((TotalFunds() > 0) && tally(tile))
    {
        Map[mapH][mapV] = tile = 0;
        Spend(1);
    }

    if (tile == 0)
    {
        if ((TotalFunds() - Tools.at(Tool::Network).cost) >= 0)
        {
            Map[mapH][mapV] = TELEBASE | CONDBIT | BURNBIT | BULLBIT | ANIMBIT;
            Spend(Tools.at(Tool::Network).cost);
            return ToolResult::Success;
        }
        else
        {
            return ToolResult::InsufficientFunds;
        }
    }
    else
    {
        return ToolResult::InvalidLocation;
    }
}


int checkBigZone(int id, int* deltaHPtr, int* deltaVPtr)
{
    switch (id)
    {
    case POWERPLANT: // check coal plant
    case PORT: // check sea port
    case NUCLEAR: // check nuc plant
    case STADIUM: // check stadium
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return (4);

    case POWERPLANT + 1: // check coal plant
    case COALSMOKE3: // check coal plant, smoke
    case COALSMOKE3 + 1: // check coal plant, smoke
    case COALSMOKE3 + 2: // check coal plant, smoke
    case PORT + 1: // check sea port
    case NUCLEAR + 1: // check nuc plant
    case STADIUM + 1: // check stadium
        *deltaHPtr = -1;
        *deltaVPtr = 0;
        return (4);

    case POWERPLANT + 4: // check coal plant
    case PORT + 4: // check sea port
    case NUCLEAR + 4: // check nuc plant
    case STADIUM + 4: // check stadium
        *deltaHPtr = 0;
        *deltaVPtr = -1;
        return (4);

    case POWERPLANT + 5: // check coal plant
    case PORT + 5: // check sea port
    case NUCLEAR + 5: // check nuc plant
    case STADIUM + 5: // check stadium
        *deltaHPtr = -1;
        *deltaVPtr = -1;
        return (4);

        // check airport
        //*** first row ***
    case AIRPORT:
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return (6);

    case AIRPORT + 1:
        *deltaHPtr = -1;
        *deltaVPtr = 0;
        return (6);

    case AIRPORT + 2:
        *deltaHPtr = -2;
        *deltaVPtr = 0;
        return (6);

    case AIRPORT + 3:
        *deltaHPtr = -3;
        *deltaVPtr = 0;
        return (6);

        //*** second row ***
    case AIRPORT + 6:
        *deltaHPtr = 0;
        *deltaVPtr = -1;
        return (6);

    case AIRPORT + 7:
        *deltaHPtr = -1;
        *deltaVPtr = -1;
        return (6);

    case AIRPORT + 8:
        *deltaHPtr = -2;
        *deltaVPtr = -1;
        return (6);

    case AIRPORT + 9:
        *deltaHPtr = -3;
        *deltaVPtr = -1;
        return (6);

        //*** third row ***
    case AIRPORT + 12:
        *deltaHPtr = 0;
        *deltaVPtr = -2;
        return (6);

    case AIRPORT + 13:
        *deltaHPtr = -1;
        *deltaVPtr = -2;
        return (6);

    case AIRPORT + 14:
        *deltaHPtr = -2;
        *deltaVPtr = -2;
        return (6);

    case AIRPORT + 15:
        *deltaHPtr = -3;
        *deltaVPtr = -2;
        return (6);

        //*** fourth row ***
    case AIRPORT + 18:
        *deltaHPtr = 0;
        *deltaVPtr = -3;
        return (6);

    case AIRPORT + 19:
        *deltaHPtr = -1;
        *deltaVPtr = -3;
        return (6);

    case AIRPORT + 20:
        *deltaHPtr = -2;
        *deltaVPtr = -3;
        return (6);

    case AIRPORT + 21:
        *deltaHPtr = -3;
        *deltaVPtr = -3;
        return (6);

    default:
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return (0);
    }
}


bool tally(int tileValue)
{
    /* can we autobulldoze this tile? */
    return (((tileValue >= FIRSTRIVEDGE) && (tileValue <= LASTRUBBLE)) ||
        ((tileValue >= (POWERBASE + 2)) && (tileValue <= (POWERBASE + 12))) ||
        ((tileValue >= TINYEXP) && (tileValue <= (LASTTINYEXP + 2))));/* ??? */
}


int checkSize(int temp)
{
    /* check for the normal com, resl, ind 3x3 zones & the fireDept & PoliceDept */
    if (((temp >= (RESBASE - 1)) && (temp <= (PORTBASE - 1))) ||
        ((temp >= (LASTPOWERPLANT + 1)) && (temp <= (POLICESTATION + 4))))
    {
        return (3);
    }
    else if (((temp >= PORTBASE) && (temp <= LASTPORT)) ||
        ((temp >= COALBASE) && (temp <= LASTPOWERPLANT)) ||
        ((temp >= STADIUMBASE) && (temp <= LASTZONE)))
    {
        return (4);
    }
    return (0);
}


void doConnectTile(const int x, const int y, const int w, const int h)
{
    if (CoordinatesValid(x, y, w, h))
    {
        ConnecTile(x, y, &Map[x][y], 0);
    }
}


void checkBorder(const int mapX, const int mapY, const int count)
{
    int xPos{}, yPos{};

    xPos = mapX; yPos = mapY - 1;
    for (int cnt = 0; cnt < count; cnt++)
    {
        /*** this will do the upper bordering row ***/
        doConnectTile(xPos, yPos, SimWidth, SimHeight);
        xPos++;
    }

    xPos = mapX - 1; yPos = mapY;
    for (int cnt = 0; cnt < count; cnt++)
    {
        /*** this will do the left bordering row ***/
        doConnectTile(xPos, yPos, SimWidth, SimHeight);
        yPos++;
    }

    xPos = mapX; yPos = mapY + count;
    for (int cnt = 0; cnt < count; cnt++)
    {
        /*** this will do the bottom bordering row ***/
        doConnectTile(xPos, yPos, SimWidth, SimHeight);
        xPos++;
    }

    xPos = mapX + count; yPos = mapY;
    for (int cnt = 0; cnt < count; cnt++)
    {
        /*** this will do the right bordering row ***/
        doConnectTile(xPos, yPos, SimWidth, SimHeight);
        yPos++;
    }
}


ToolResult checkArea(const int mapH, const int mapV, const int base, const int size, const bool animate, const Tool tool)
{
    if (!CoordinatesValid(mapH - 1, mapV - 1, SimWidth - size, SimHeight - size))
    {
        return ToolResult::OutOfBounds;
    }

    bool needsBulldozing{ false };

    int totalCost{ Tools.at(tool).cost };

    int mapY{ mapV - 1 };
    for (int row = 0; row < size; ++row)
    {
        int mapX{ mapH - 1 };

        for (int col = 0; col < size; ++col)
        {
            const unsigned int tileValue{ maskedTileValue(mapX, mapY) };

            if (autoBulldoze)
            {
                // if autoDoze is enabled, add up the cost of bulldozed tiles
                if (tileValue != 0)
                {
                    if (tally(tileValue))
                    {
                        ++totalCost;
                    }
                    else
                    {
                        needsBulldozing = true;
                    }
                }
            }
            else
            {
                // check and see if the tile is clear or not
                if (tileValue != 0)
                {
                    needsBulldozing = true;
                    break;
                }
            }
            ++mapX;
        }
        ++mapY;
    }

    if (needsBulldozing)
    {
        return ToolResult::RequiresBulldozing;
    }

    if ((TotalFunds() - totalCost) < 0)
    {
        return ToolResult::InsufficientFunds;
    }

    if ((Players > 1) && (OverRide == 0) && (totalCost >= Expensive))
    {
        return ToolResult::NetworkVotedNo;
    }

    Spend(totalCost);
    UpdateFunds();

    int tileBase = base;
    mapY = mapV - 1;
    for (int row = 0; row < size; ++row)
    {
        int mapX = mapH - 1;

        for (int col = 0; col < size; ++col)
        {
            if (col == 1 && row == 1)
            {
                Map[mapX][mapY] = tileBase + BNCNBIT + ZONEBIT;
            }
            // special case to get nuclear plant animation working
            else if (animate && col == 1 && row == 2)
            {
                Map[mapX][mapY] = tileBase + BNCNBIT + ANIMBIT;
            }
            else
            {
                Map[mapX][mapY] = tileBase + BNCNBIT;
            }
            ++mapX;
            ++tileBase;
        }
        ++mapY;
    }

    checkBorder(mapH - 1, mapV - 1, size);
    return ToolResult::Success;
}


/* QUERY */
/* search table for zone status string match */
static int idArray[28] =
{
    DIRT,
    RIVER,
    TREEBASE,
    RUBBLE,
    FLOOD,
    RADTILE,
    FIRE,
    ROADBASE,
    POWERBASE,
    RAILBASE,
    RESBASE,
    COMBASE,
    INDBASE,
    PORTBASE,
    AIRPORTBASE,
    COALBASE,
    FIRESTBASE,
    POLICESTBASE,
    STADIUMBASE,
    NUCLEARBASE,
    827, // crossed out lightning bolt
    832, // radar dish first frame
    FOUNTAIN,
    INDBASE2,
    FOOTBALLGAME1,
    VBRDG0,
    952, // radiation icon first frame
    956 // white tile?
};

/*
  0, 2, 21, 44, 
  48, 52, 53, 64,
  208, 224, 240, 423, 
  612, 693, 709, 745,
  761, 770, 779, 811, 
  827, 832, 840, 844,
  932, 948, 952, 956

  Clear, Water, Trees, Rubble, 
  Flood, Radioactive Waste, Fire, Road,
  Power, Rail, Residential, Commercial,
  Industrial, Port, AirPort, Coal Power,
  Fire Department, Police Department, Stadium, Nuclear Power, 
  Draw Bridge, Radar Dish, Fountain, Industrial,
  49er's 38  Bears 3, Draw Bridge, Ur 238
*/


int getDensityStr(int catNo, int mapH, int mapV)
{
    int z;

    switch (catNo)
    {
    case 0:
        z = PopDensity[mapH >> 1][mapV >> 1];
        z = z >> 6;
        z = z & 3;
        return (z);

    case 1:
        z = LandValueMem[mapH >> 1][mapV >> 1];
        if (z < 30) return (4);
        if (z < 80) return (5);
        if (z < 150) return (6);
        return (7);

    case 2:
        z = CrimeMem[mapH >> 1][mapV >> 1];
        z = z >> 6;
        z = z & 3;
        return (z + 8);

    case 3:
        z = PollutionMem[mapH >> 1][mapV >> 1];
        if ((z < 64) && (z > 0)) return (13);
        z = z >> 6;
        z = z & 3;
        return (z + 12);

    case 4:
        z = RateOGMem[mapH >> 3][mapV >> 3];
        if (z < 0) return (16);
        if (z == 0) return (17);
        if (z > 100) return (19);
        return (18);

    default:
        throw std::runtime_error("");
    }
}


struct ZoneStatsStrings
{
    const std::string str;
    const std::string s0;
    const std::string s1;
    const std::string s2;
    const std::string s3;
    const std::string s4;
};


void DoShowZoneStatus(const ZoneStatsStrings zoneStats, int x, int y)
{
    const std::string msg{
        "UIShowZoneStatus {" +
        zoneStats.str + "} {" +
        zoneStats.s0 + "} {" +
        zoneStats.s1 + "} {" +
        zoneStats.s2 + "} {" +
        zoneStats.s3 + "} {" +
        zoneStats.s4 + "} {" +
        std::to_string(x) + "} {" +
        std::to_string(y) + "}"
    };

    Eval(msg);
}


const std::string& queryString(int tileValue)
{
    for (int i = 1; i < 29; ++i)
    {
        if (tileValue < idArray[i])
        {
            /*
            int queryId = i - 1;
            if (queryId < 1 || queryId > 28)
            {
                queryId = 28;
            }
            */

            int queryId = std::clamp(i - 1, 0, 28);

            // \fixme ugly cast
            return QueryStatsString(static_cast<QueryStatsId>(queryId));

            break;
        }
    }

    return QueryStatsString(QueryStatsId::Padding2);
}


void doZoneStatus(int x, int y)
{
    int tileNum = Map[x][y] & LOMASK;
    if (tileNum >= COALSMOKE1 && tileNum < FOOTBALLGAME1)
    {
        tileNum = COALBASE;
    }

    std::string localStr = queryString(tileNum);
    std::array<std::string, 5> statusStr;

    for (int _x = 0; _x < 5; ++_x)
    {
        int id = getDensityStr(_x, x, y);
        id++;
        
        // \fixme ugly cast
        statusStr[_x] = ZoneStatsString(static_cast<ZoneStatsId>(std::clamp(id, 1, 19)));
    }

    DoShowZoneStatus({ localStr, statusStr[0], statusStr[1], statusStr[2], statusStr[3], statusStr[4] }, x, y);
}


void putRubble(const int mapX, const int mapY, const int size)
{
    for (int x = mapX - 1; x < mapX + size - 1; x++)
    {
        for (int y = mapY - 1; y < mapY + size - 1; y++)
        {
            if (CoordinatesValid(x, y, SimWidth, SimHeight))
            {
                int cellValue = Map[x][y] & LOMASK;
                if ((cellValue != RADTILE) && (cellValue != 0))
                {
                    Map[x][y] = (DoAnimation ? (TINYEXP + RandomRange(0, 2)) : SOMETINYEXP) | ANIMBIT | BULLBIT;
                }
            }
        }
    }
}


/************************************************************************/
/* TOOLS */


ToolResult query_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    doZoneStatus(x, y);
    return ToolResult::Success;
}


ToolResult bulldozer_tool(int x, int y)
{
    unsigned int currTile, temp;
    int zoneSize, deltaH, deltaV;

    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    currTile = Map[x][y];
    temp = currTile & LOMASK;

    ToolResult result = ToolResult::Success;

    if (currTile & ZONEBIT)
    { /* zone center bit is set */
        if (TotalFunds() > 0)
        {
            Spend(1);
            switch (checkSize(temp))
            {
            case 3:
                MakeSound("city", "Explosion-High");
                putRubble(x, y, 3);
                break;

            case 4:
                putRubble(x, y, 4);
                MakeSound("city", "Explosion-Low");
                break;

            case 6:
                MakeSound("city", "Explosion-High");
                MakeSound("city", "Explosion-Low");
                putRubble(x, y, 6);
                break;

            default:
                break;
            }
        }
    }
    else if ((zoneSize = checkBigZone(temp, &deltaH, &deltaV)))
    {
        if (TotalFunds() > 0)
        {
            Spend(1);
            switch (zoneSize)
            {
            case 3:
                MakeSound("city", "Explosion-High");
                break;

            case 4:
                MakeSound("city", "Explosion-Low");
                putRubble(x + deltaH, y + deltaV, 4);
                break;

            case 6:
                MakeSound("city", "Explosion-High");
                MakeSound("city", "Explosion-Low");
                putRubble(x + deltaH, y + deltaV, 6);
                break;
            }
        }
    }
    else
    {
        if (temp == RIVER || temp == REDGE || temp == CHANNEL)
        {
            if (TotalFunds() >= 6)
            {
                result = ConnecTile(x, y, &Map[x][y], 1);
                if (temp != (Map[x][y] & LOMASK))
                {
                    Spend(5);
                }
            }
            else
            {
                result = ToolResult::InsufficientFunds;
            }
        }
        else
        {
            result = ConnecTile(x, y, &Map[x][y], 1);
        }
    }
    UpdateFunds();
    return result;
}


ToolResult road_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    ToolResult result = ConnecTile(x, y, &Map[x][y], 2);
    UpdateFunds();
    return result;
}


ToolResult rail_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    ToolResult result = ConnecTile(x, y, &Map[x][y], 3);
    UpdateFunds();
    return result;
}


ToolResult wire_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    ToolResult result = ConnecTile(x, y, &Map[x][y], 4);
    UpdateFunds();
    return result;
}


ToolResult park_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return putDownPark(x, y);
}


ToolResult residential_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, RESBASE, 3, false, Tool::Residential);
}


ToolResult commercial_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, COMBASE, 3, false, Tool::Commercial);
}


ToolResult industrial_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, INDBASE, 3, false, Tool::Industrial);
}


ToolResult police_dept_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, POLICESTBASE, 3, false, Tool::Police);
}


ToolResult fire_dept_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, FIRESTBASE, 3, false , Tool::Fire);
}


ToolResult stadium_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, STADIUMBASE, 4, false, Tool::Stadium);
}


ToolResult coal_power_plant_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, COALBASE, 4, false, Tool::Coal);
}


ToolResult nuclear_power_plant_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, NUCLEARBASE, 4, true, Tool::Nuclear);
}


ToolResult seaport_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, PORTBASE, 4, false, Tool::Seaport);
}


ToolResult airport_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return checkArea(x, y, AIRPORTBASE, 6, false, Tool::Airport);
}


ToolResult network_tool(int x, int y)
{
    if (!CoordinatesValid(x, y, SimWidth - 1, SimHeight - 1))
    {
        return ToolResult::OutOfBounds;
    }

    return putDownNetwork(x, y);
}


ToolResult do_tool(Tool state, int mapX, int mapY)
{
    switch (state)
    {
    case Tool::Residential:
        return residential_tool(mapX, mapY);
        break;

    case Tool::Commercial:
        return commercial_tool(mapX, mapY);
        break;

    case Tool::Industrial:
        return industrial_tool(mapX, mapY);
        break;

    case Tool::Fire:
        return fire_dept_tool(mapX, mapY);
        break;

    case Tool::Query:
        return query_tool(mapX, mapY);
        break;

    case Tool::Police:
        return police_dept_tool(mapX, mapY);
        break;

    case Tool::Wire:
        return wire_tool(mapX, mapY);
        break;

    case Tool::Bulldoze:
        return bulldozer_tool(mapX, mapY);
        break;

    case Tool::Rail:
        return rail_tool(mapX, mapY);
        break;

    case Tool::Road:
        return road_tool(mapX, mapY);
        break;

    case Tool::Stadium:
        return stadium_tool(mapX, mapY);
        break;

    case Tool::Park:
        return park_tool(mapX, mapY);
        break;

    case Tool::Seaport:
        return seaport_tool(mapX, mapY);
        break;

    case Tool::Coal:
        return coal_power_plant_tool(mapX, mapY);
        break;

    case Tool::Nuclear:
        return nuclear_power_plant_tool(mapX, mapY);
        break;

    case Tool::Airport:
        return airport_tool(mapX, mapY);
        break;

    case Tool::Network:
        return network_tool(mapX, mapY);
        break;

    default:
        return ToolResult::InvalidOperation;
        break;
    }

    return ToolResult::InvalidOperation;
}


ToolResult current_tool(int x, int y)
{
    return do_tool(PendingTool, x, y);
}


void DoPendTool(Tool tool, int x, int y)
{
    Eval(std::string("DoPendTool: ") + "'winId' " + Tools.at(tool).name + " " + std::to_string(x) + " " + std::to_string(y));
}


/**
 * Performs tool action
 * 
 * Coordinates expected to be in tile coords,
 * not screen coords.
 */
void ToolDown(int mapX, int mapY)
{
    if (PendingTool == Tool::None)
    {
        return;
    }

    ToolResult result = current_tool(mapX, mapY);

    if (result == ToolResult::RequiresBulldozing)
    {
        ClearMes();
        SendMes(NotificationId::MustBulldoze);
        MakeSoundOn(nullptr, "edit", "UhUh");
    }
    else if (result == ToolResult::InsufficientFunds)
    {
        ClearMes();
        SendMes(NotificationId::InsufficientFunds);
        MakeSoundOn(nullptr, "edit", "Sorry");
    }
    else
    {
        DoPendTool(PendingTool, mapX, mapY);
    }

    sim_skip = 0;
}


void ToolDrag(int px, int py)
{
    /*
    int x, y, dx, dy, adx, ady, lx, ly, dist;
    float i, step, tx, ty, dtx, dty, rx, ry;

    ViewToPixelCoords(view, px, py, x, y);
    view->tool_x = x; view->tool_y = y;

    current_tool(view, x, y, 0);
    view->last_x = x; view->last_y = y;

    dist = ToolSize[view->tool_state];

    x >>= 4; y >>= 4;
    lx = view->last_x >> 4;
    ly = view->last_y >> 4;

    dx = x - lx;
    dy = y - ly;

    if (dx == 0 && dy == 0)
    {
        return;
    }

    adx = std::abs(dx); ady = std::abs(dy);

    if (adx > ady)
    {
        step = 0.3f / adx;
    }
    else
    {
        step = 0.3f / ady;
    }

    rx = (dx < 0 ? 1.0f : 0.0f);
    ry = (dy < 0 ? 1.0f : 0.0f);

    if (dist == 1)
    {
        for (i = 0.0; i <= 1 + step; i += step)
        {
            tx = (view->last_x >> 4) + i * dx;
            ty = (view->last_y >> 4) + i * dy;
            dtx = std::abs(tx - lx);
            dty = std::abs(ty - ly);
            if (dtx >= 1 || dty >= 1)
            {
                /* fill in corners
                if ((dtx >= 1) && (dty >= 1))
                {
                    if (dtx > dty)
                    {
                        current_tool(view, ((int)(tx + rx)) << 4, ly << 4, 0);
                    }
                    else
                    {
                        current_tool(view, lx << 4, ((int)(ty + ry)) << 4, 0);
                    }
                }
                lx = (int)(tx + rx);
                ly = (int)(ty + ry);
                current_tool(view, lx << 4, ly << 4, 0);
            }
        }
    }
    else
    {
        for (i = 0.0; i <= 1 + step; i += step)
        {
            tx = (view->last_x >> 4) + i * dx;
            ty = (view->last_y >> 4) + i * dy;
            dtx = std::abs(tx - lx);
            dty = std::abs(ty - ly);
            lx = (int)(tx + rx);
            ly = (int)(ty + ry);
            current_tool(view, lx << 4, ly << 4, 0);
        }

        view->last_x = (lx << 4) + 8;
        view->last_y = (ly << 4) + 8;
    }

    sim_skip = 0; /* update editors overlapping this one 
    view->skip = 0;
    view->invalid = 1;
    */
}


int ToolUp(int x, int y)
{
    //ToolDrag(view, x, y);
    return 0;
}
