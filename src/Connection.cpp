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
#include "Map.h"

#include "Budget.h"
#include "Connection.h"

#include "Tool.h"

#include "w_util.h"


namespace
{
    constexpr auto WaterConstructionMultiplier = 5;

    int _RoadTable[16]
    {
      66, 67, 66, 68,
      67, 67, 69, 73,
      66, 71, 66, 72,
      70, 75, 74, 76
    };

    int _RailTable[16]
    {
      226, 227, 226, 228,
      227, 227, 229, 233,
      226, 231, 226, 232,
      230, 235, 234, 236
    };

    int _WireTable[16]
    {
      210, 211, 210, 212,
      211, 211, 213, 217,
      210, 215, 210, 216,
      214, 219, 218, 220
    };

    unsigned int NeutralizeRoad(int tile)
    {
        unsigned int outValue = tile & LOMASK;
        if ((outValue >= 64) && (outValue <= 207))
        {
            outValue = (tile & 0x000F) + 64;
        }

        return outValue;
    }
}


ToolResult _LayDoze(int x, int y, int* TileAdrPtr, Budget& budget)
{
    int Tile;

    if (budget.Broke())
    {
        return ToolResult::InsufficientFunds; // no mas dinero.
    }

    Tile = (*TileAdrPtr);

    if (!(Tile & BULLBIT))
    {
        return ToolResult::CannotBulldoze; // Check dozeable bit.
    }

    Tile = NeutralizeRoad(Tile);

    switch (Tile)
    {
    case HBRIDGE:
    case VBRIDGE:
    case BRWV:
    case BRWH:
    case HBRDG0:
    case HBRDG1:
    case HBRDG2:
    case HBRDG3:
    case VBRDG0:
    case VBRDG1:
    case VBRDG2:
    case VBRDG3:
    case HPOWER:
    case VPOWER:
    case HRAIL:
    case VRAIL: // Dozing over water, replace with water.
        (*TileAdrPtr) = RIVER;
        break;

    default: // Dozing on land, replace with land.  Simple, eh?
        (*TileAdrPtr) = DIRT;
        break;
    }

    budget.Spend(1); // Costs $1.00...
    return ToolResult::Success;
}


ToolResult _LayRoad(int x, int y, int* TileAdrPtr, Budget& budget)
{
    int cost = 10;

    if (budget.CurrentFunds() < 10)
    {
        return ToolResult::InsufficientFunds;
    }

    switch (Map[x][y] & LOMASK)
    {
    case DIRT:
        Map[x][y] = ROADS | BULLBIT | BURNBIT;
        break;

    case RIVER: // Road on Water
    case REDGE:
    case CHANNEL: // Check how to build bridges, if possible.
        if (budget.CurrentFunds() < 50)
        {
            return ToolResult::InsufficientFunds;
        }

        cost = 50;

        if (x < (SimWidth - 1))
        {
            const int adjTile = NeutralizeRoad(Map[x + 1][y]);
            if ((adjTile == VRAILROAD) || (adjTile == HBRIDGE) || ((adjTile >= ROADS) && (adjTile <= HROADPOWER)))
            {
                Map[x][y] = HBRIDGE | BULLBIT;
                break;
            }
        }

        if (x > 0)
        {
            const int adjTile = NeutralizeRoad(Map[x - 1][y]);
            if ((adjTile == VRAILROAD) || (adjTile == HBRIDGE) || ((adjTile >= ROADS) && (adjTile <= INTERSECTION)))
            {
                Map[x][y] = HBRIDGE | BULLBIT;
                break;
            }
        }

        if (y < (SimHeight - 1))
        {
            const int adjTile = NeutralizeRoad(Map[x][y + 1]);
            if ((adjTile == HRAILROAD) || (adjTile == VROADPOWER) || ((adjTile >= VBRIDGE) && (adjTile <= INTERSECTION)))
            {
                Map[x][y] = VBRIDGE | BULLBIT;
                break;
            }
        }

        if (y > 0)
        {
            const int adjTile = NeutralizeRoad(Map[x][y - 1]);
            if ((adjTile == HRAILROAD) || (adjTile == VROADPOWER) || ((adjTile >= VBRIDGE) && (adjTile <= INTERSECTION)))
            {
                Map[x][y] = VBRIDGE | BULLBIT;
                break;
            }
        }

        // Can't do road...
        return ToolResult::InvalidOperation;

    case LHPOWER: // Road on power
        Map[x][y] = VROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVPOWER: // Road on power #2
        Map[x][y] = HROADPOWER | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LHRAIL: // Road on rail
        Map[x][y] = HRAILROAD | BURNBIT | BULLBIT;
        break;

    case LVRAIL: // Road on rail #2
        Map[x][y] = VRAILROAD | BURNBIT | BULLBIT;
        break;

    default: // Can't do road
        return ToolResult::InvalidOperation;
    }

    budget.Spend(cost);
    return ToolResult::Success;
}


ToolResult _LayRail(int x, int y, int*, Budget& budget)
{
    int cost = 20;

    if (budget.CurrentFunds() < 20)
    {
        return ToolResult::InsufficientFunds;
    }

    switch (NeutralizeRoad(Map[x][y] & LOMASK))
    {
    case DIRT: // Rail on Dirt
        Map[x][y] = LHRAIL | BULLBIT | BURNBIT;
        break;

    case RIVER: // Rail on Water
    case REDGE:
    case CHANNEL: // Check how to build underwater tunnel, if possible.
        if (budget.CurrentFunds() < 100)
        {
            return ToolResult::InsufficientFunds;
        }
        cost = 100;

        if (x < (SimWidth - 1))
        {
            const int adjTile = NeutralizeRoad(Map[x + 1][y]);
            if ((adjTile == RAILHPOWERV) || (adjTile == RAILBASE) || ((adjTile >= LHRAIL) && (adjTile <= HRAILROAD)))
            {
                Map[x][y] = HRAIL | BULLBIT;
                break;
            }
        }

        if (x > 0)
        {
            const int adjTile = NeutralizeRoad(Map[x - 1][y]);
            if ((adjTile == RAILHPOWERV) || (adjTile == RAILBASE) || ((adjTile > VRAIL) && (adjTile < VRAILROAD)))
            {
                Map[x][y] = HRAIL | BULLBIT;
                break;
            }
        }

        if (y < (SimHeight - 1))
        {
            const int adjTile = NeutralizeRoad(Map[x][y + 1]);
            if ((adjTile == RAILVPOWERH) || (adjTile == VRAILROAD) || ((adjTile > HRAIL) && (adjTile < HRAILROAD)))
            {
                Map[x][y] = VRAIL | BULLBIT;
                break;
            }
        }

        if (y > 0)
        {
            const int adjTile = NeutralizeRoad(Map[x][y - 1]);
            if ((adjTile == RAILVPOWERH) || (adjTile == VRAILROAD) || ((adjTile > HRAIL) && (adjTile < HRAILROAD)))
            {
                Map[x][y] = VRAIL | BULLBIT;
                break;
            }
        }

        // Can't do rail...
        return ToolResult::InvalidOperation;

    case LHPOWER: // Rail on power
        Map[x][y] = RAILVPOWERH | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVPOWER: // Rail on power #2 
        Map[x][y] = RAILHPOWERV | CONDBIT | BURNBIT | BULLBIT;
        break;

    case ROADS: // Rail on road
        Map[x][y] = VRAILROAD | BURNBIT | BULLBIT;
        break;

    case ROADSV: // Rail on road #2
        Map[x][y] = HRAILROAD | BURNBIT | BULLBIT;
        break;

    default: // Can't do rail
        return ToolResult::InvalidOperation;
    }

    budget.Spend(cost);
    return ToolResult::Success;
}


ToolResult _LayWire(int x, int y, int*, Budget& budget)
{
    int cost = 5;

    if (budget.CurrentFunds() < 5)
    {
        return ToolResult::InsufficientFunds;
    }

    switch (NeutralizeRoad(Map[x][y] & LOMASK))
    {
    case DIRT: // Wire on Dirt
        Map[x][y] = 210 | CONDBIT | BURNBIT | BULLBIT;
        break;

    case RIVER: // Wire on Water
    case REDGE:
    case CHANNEL: // Check how to lay underwater wire, if possible.
        if (budget.CurrentFunds() < 25)
        {
            return ToolResult::InsufficientFunds;
        }

        cost = 25;

        if (x < (SimWidth - 1))
        {
            int adjTile = Map[x + 1][y];
            if (adjTile & CONDBIT)
            {
                adjTile = NeutralizeRoad(adjTile);
                if ((adjTile != 77) && (adjTile != 221) && (adjTile != 208))
                {
                    Map[x][y] = 209 | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (x > 0)
        {
            int adjTile = Map[x - 1][y];
            if (adjTile & CONDBIT)
            {
                adjTile = NeutralizeRoad(adjTile);
                if ((adjTile != 77) && (adjTile != 221) && (adjTile != 208))
                {
                    Map[x][y] = 209 | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (y < (SimHeight - 1))
        {
            int adjTile = Map[x][y + 1];
            if (adjTile & CONDBIT)
            {
                adjTile = NeutralizeRoad(adjTile);
                if ((adjTile != 78) && (adjTile != 222) && (adjTile != 209))
                {
                    Map[x][y] = 208 | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        if (y > 0)
        {
            int adjTile = Map[x][y - 1];
            if (adjTile & CONDBIT)
            {
                adjTile = NeutralizeRoad(adjTile);
                if ((adjTile != 78) && (adjTile != 222) && (adjTile != 209))
                {
                    Map[x][y] = 208 | CONDBIT | BULLBIT;
                    break;
                }
            }
        }

        // Can't do wire...
        return ToolResult::InvalidOperation;

    case ROADS: // Wire on Road
        Map[x][y] = 77 | CONDBIT | BURNBIT | BULLBIT;
        break;

    case ROADSV: // Wire on Road #2
        Map[x][y] = 78 | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LHRAIL: // Wire on rail
        Map[x][y] = 221 | CONDBIT | BURNBIT | BULLBIT;
        break;

    case LVRAIL: // Wire on rail #2
        Map[x][y] = 222 | CONDBIT | BURNBIT | BULLBIT;
        break;

    default: // Can't do wire
        return ToolResult::InvalidOperation;
    }

    budget.Spend(cost);
    return ToolResult::Success;
}


void _FixSingle(int x, int y, int* TileAdrPtr)
{
    int Tile = NeutralizeRoad((*TileAdrPtr) & LOMASK);
    int adjTile = 0;

    // Cleanup Road
    if ((Tile >= 66) && (Tile <= 76))
    {
        if (y > 0)
        {
            Tile = NeutralizeRoad(TileAdrPtr[-1]);
            if (((Tile == 237) || ((Tile >= 64) && (Tile <= 78))) && (Tile != 77) && (Tile != 238) && (Tile != 64))
            {
                adjTile |= 0x0001;
            }
        }

        if (x < (SimWidth - 1))
        {
            Tile = NeutralizeRoad(TileAdrPtr[SimHeight]);
            if (((Tile == 238) || ((Tile >= 64) && (Tile <= 78))) && (Tile != 78) && (Tile != 237) && (Tile != 65))
            {
                adjTile |= 0x0002;
            }
        }

        if (y < (SimHeight - 1))
        {
            Tile = NeutralizeRoad(TileAdrPtr[1]);
            if (((Tile == 237) || ((Tile >= 64) && (Tile <= 78))) && (Tile != 77) && (Tile != 238) && (Tile != 64))
            {
                adjTile |= 0x0004;
            }
        }

        if (x > 0)
        {
            Tile = NeutralizeRoad(TileAdrPtr[-SimHeight]);
            if (((Tile == 238) || ((Tile >= 64) && (Tile <= 78))) && (Tile != 78) && (Tile != 237) && (Tile != 65))
            {
                adjTile |= 0x0008;
            }
        }

        (*TileAdrPtr) = _RoadTable[adjTile] | BULLBIT | BURNBIT;
        return;
    }

    // Cleanup Rail
    if ((Tile >= 226) && (Tile <= 236))
    {

        if (y > 0)
        {
            Tile = NeutralizeRoad(TileAdrPtr[-1]);
            if ((Tile >= 221) && (Tile <= 238) && (Tile != 221) && (Tile != 237) && (Tile != 224))
            {
                adjTile |= 0x0001;
            }
        }

        if (x < (SimWidth - 1))
        {
            Tile = NeutralizeRoad(TileAdrPtr[SimHeight]);
            if ((Tile >= 221) && (Tile <= 238) && (Tile != 222) && (Tile != 238) && (Tile != 225))
            {
                adjTile |= 0x0002;
            }
        }

        if (y < (SimHeight - 1))
        {
            Tile = NeutralizeRoad(TileAdrPtr[1]);
            if ((Tile >= 221) && (Tile <= 238) && (Tile != 221) && (Tile != 237) && (Tile != 224))
            {
                adjTile |= 0x0004;
            }
        }

        if (x > 0)
        {
            Tile = NeutralizeRoad(TileAdrPtr[-SimHeight]);
            if ((Tile >= 221) && (Tile <= 238) && (Tile != 222) && (Tile != 238) && (Tile != 225))
            {
                adjTile |= 0x0008;
            }
        }

        (*TileAdrPtr) = _RailTable[adjTile] | BULLBIT | BURNBIT;
        return;
    }

    // Cleanup Wire
    if ((Tile >= 210) && (Tile <= 220))
    {

        if (y > 0)
        {
            Tile = TileAdrPtr[-1];
            if (Tile & CONDBIT)
            {
                Tile = NeutralizeRoad(Tile);
                if ((Tile != 209) && (Tile != 78) && (Tile != 222))
                {
                    adjTile |= 0x0001;
                }
            }
        }

        if (x < (SimWidth - 1))
        {
            Tile = TileAdrPtr[SimHeight];
            if (Tile & CONDBIT)
            {
                Tile = NeutralizeRoad(Tile);
                if ((Tile != 208) && (Tile != 77) && (Tile != 221))
                {
                    adjTile |= 0x0002;
                }
            }
        }

        if (y < (SimHeight - 1))
        {
            Tile = TileAdrPtr[1];
            if (Tile & CONDBIT)
            {
                Tile = NeutralizeRoad(Tile);
                if ((Tile != 209) && (Tile != 78) && (Tile != 222))
                {
                    adjTile |= 0x0004;
                }
            }
        }

        if (x > 0)
        {
            Tile = TileAdrPtr[-SimHeight];
            if (Tile & CONDBIT)
            {
                Tile = NeutralizeRoad(Tile);
                if ((Tile != 208) && (Tile != 77) && (Tile != 221))
                {
                    adjTile |= 0x0008;
                }
            }
        }

        (*TileAdrPtr) = _WireTable[adjTile] | BULLBIT | BURNBIT | CONDBIT;
        return;
    }
}


void _FixZone(int x, int y, int* TileAdrPtr)
{
    _FixSingle(x, y, &TileAdrPtr[0]);

    if (y > 0)
    {
        _FixSingle(x, y - 1, &TileAdrPtr[-1]);
    }

    if (x < (SimWidth - 1))
    {
        _FixSingle(x + 1, y, &TileAdrPtr[SimHeight]);
    }

    if (y < (SimHeight - 1))
    {
        _FixSingle(x, y + 1, &TileAdrPtr[1]);
    }

    if (x > 0)
    {
        _FixSingle(x - 1, y, &TileAdrPtr[-SimHeight]);
    }
}


ToolResult CanConnectTile(int x, int y, Tool tool, Budget& budget)
{
    const int cost = toolProperties(tool).cost;
    const int waterCost = cost * WaterConstructionMultiplier;

    if (budget.CurrentFunds() < cost)
    {
        return ToolResult::InsufficientFunds;
    }

    switch (Map[x][y] & LOMASK)
    {
    case DIRT:
        break;

    case RIVER: // Road on Water
    case REDGE:
    case CHANNEL: // Check how to build bridges, if possible.
        if (budget.CurrentFunds() < waterCost)
        {
            return ToolResult::InsufficientFunds;
        }

        if (x < (SimWidth - 1))
        {
            int adjTile = NeutralizeRoad(Map[x + 1][y]);
            if ((adjTile == VRAILROAD) || (adjTile == HBRIDGE) || ((adjTile >= ROADS) && (adjTile <= HROADPOWER)))
            {
                break;
            }
        }

        if (x > 0)
        {
            int adjTile = NeutralizeRoad(Map[x - 1][y]);
            if ((adjTile == VRAILROAD) || (adjTile == HBRIDGE) || ((adjTile >= ROADS) && (adjTile <= INTERSECTION)))
            {
                break;
            }
        }

        if (y < (SimHeight - 1))
        {
            int adjTile = NeutralizeRoad(Map[x][y + 1]);
            if ((adjTile == HRAILROAD) || (adjTile == VROADPOWER) || ((adjTile >= VBRIDGE) && (adjTile <= INTERSECTION)))
            {
                break;
            }
        }

        if (y > 0)
        {
            int adjTile = NeutralizeRoad(Map[x][y - 1]);
            if ((adjTile == HRAILROAD) || (adjTile == VROADPOWER) || ((adjTile >= VBRIDGE) && (adjTile <= INTERSECTION)))
            {
                break;
            }
        }

        return ToolResult::InvalidOperation;

    case LHPOWER:
    case LVPOWER:
    case LHRAIL:
    case LVRAIL:
        break;

    default:
        return ToolResult::InvalidOperation;
    }

    return ToolResult::Success;
}


ToolResult ConnectTile(int x, int y, int* TileAdrPtr, int Command, Budget& budget)
{
    int Tile{};

    // AutoDoze
    if ((Command >= 2) && (Command <= 4))
    {
        if ((AutoBulldoze) && (budget.CurrentFunds() > 0) && ((Tile = (*TileAdrPtr)) & BULLBIT))
        {
            Tile = NeutralizeRoad(Tile);
            // Maybe this should check BULLBIT instead of checking tile values?
            if (((Tile >= TINYEXP) && (Tile <= LASTTINYEXP)) || ((Tile < 64) && (Tile != 0)))
            {
                budget.Spend(1);
                (*TileAdrPtr) = 0;
            }
        }
    }

    ToolResult result = ToolResult::Success;
    switch (Command)
    {
    case 0:	// Fix zone
        _FixZone(x, y, TileAdrPtr);
        break;

    case 1:	// Doze zone
        result = _LayDoze(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    case 2:	// Lay Road
        result = _LayRoad(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    case 3:	// Lay Rail
        result = _LayRail(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    case 4:	// Lay Wire
        result = _LayWire(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    default:
        throw std::runtime_error("Unsupported flag");
        break;
    }

    return result;
}
