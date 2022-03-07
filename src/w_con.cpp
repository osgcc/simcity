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

#include "w_stubs.h"
#include "w_tool.h"
#include "w_util.h"


int _RoadTable[16] = {
  66, 67, 66, 68,
  67, 67, 69, 73,
  66, 71, 66, 72,
  70, 75, 74, 76
};

int _RailTable[16] = {
  226, 227, 226, 228,
  227, 227, 229, 233,
  226, 231, 226, 232,
  230, 235, 234, 236
};

int _WireTable[16] = {
  210, 211, 210, 212,
  211, 211, 213, 217,
  210, 215, 210, 216,
  214, 219, 218, 220
};


#define NeutralizeRoad(Tile) \
  if (((Tile &= LOMASK) >= 64) && \
      ((Tile & LOMASK) <= 207)) { \
    Tile = (Tile & 0x000F) + 64; \
  }


/* comefrom: ConnecTile */
ToolResult
_LayDoze(int x, int y, int *TileAdrPtr, Budget& budget)
{
  int Tile;

  if (budget.Broke()) {
    return ToolResult::InsufficientFunds;			/* no mas dinero. */
  }

  Tile = (*TileAdrPtr);

  if (!(Tile & BULLBIT)) {
    return ToolResult::CannotBulldoze;			/* Check dozeable bit. */
  }

  NeutralizeRoad(Tile);

  switch (Tile) {
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
  case VRAIL:		/* Dozing over water, replace with water. */
    (*TileAdrPtr) = RIVER;
    break;

  default:		/* Dozing on land, replace with land.  Simple, eh? */
    (*TileAdrPtr) = DIRT;
    break;
  }

  budget.Spend(1);			/* Costs $1.00....*/
  return ToolResult::Success;
}


/* comefrom: ConnecTile */
ToolResult
_LayRoad(int x, int y, int *TileAdrPtr, Budget& budget)
{
  int Tile;
  int cost = 10;

  if (budget.CurrentFunds() < 10) {
    return ToolResult::InsufficientFunds;
  }

  Tile = (*TileAdrPtr) & LOMASK;

  switch (Tile) {

  case DIRT:
    (*TileAdrPtr) = ROADS | BULLBIT | BURNBIT;
    break;
			
  case RIVER:			/* Road on Water */
  case REDGE:
  case CHANNEL:			/* Check how to build bridges, if possible. */
    if (budget.CurrentFunds() < 50) {
      return ToolResult::InsufficientFunds;
    }

    cost = 50;

    if (x < (SimWidth - 1)) {
      Tile = TileAdrPtr[SimHeight];
      NeutralizeRoad(Tile);
      if ((Tile == VRAILROAD) ||
	  (Tile == HBRIDGE) ||
	  ((Tile >= ROADS) &&
	   (Tile <= HROADPOWER))) {
	(*TileAdrPtr) = HBRIDGE | BULLBIT;
	break;
      }
    }

    if (x > 0) {
      Tile = TileAdrPtr[-SimHeight];
      NeutralizeRoad(Tile);
      if ((Tile == VRAILROAD) ||
	  (Tile == HBRIDGE) ||
	  ((Tile >= ROADS) &&
	   (Tile <= INTERSECTION))) {
	(*TileAdrPtr) = HBRIDGE | BULLBIT;
	break;
      }
    }

    if (y < (SimHeight - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if ((Tile == HRAILROAD) ||
	  (Tile == VROADPOWER) ||
	  ((Tile >= VBRIDGE) &&
	   (Tile <= INTERSECTION))) {
	(*TileAdrPtr) = VBRIDGE | BULLBIT;
	break;
      }
    }

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if ((Tile == HRAILROAD) ||
	  (Tile == VROADPOWER) ||
	  ((Tile >= VBRIDGE) &&
	   (Tile <= INTERSECTION))) {
	(*TileAdrPtr) = VBRIDGE | BULLBIT;
	break;
      }
    }

    /* Can't do road... */
    return ToolResult::InvalidOperation;

  case LHPOWER:		/* Road on power */
    (*TileAdrPtr) = VROADPOWER | CONDBIT | BURNBIT | BULLBIT;
    break;

  case LVPOWER:		/* Road on power #2 */
    (*TileAdrPtr) = HROADPOWER | CONDBIT | BURNBIT | BULLBIT;
    break;

  case LHRAIL:		/* Road on rail */
    (*TileAdrPtr) = HRAILROAD | BURNBIT | BULLBIT;
    break;

  case LVRAIL:		/* Road on rail #2 */
    (*TileAdrPtr) = VRAILROAD | BURNBIT | BULLBIT;
    break;

  default:		/* Can't do road */
    return ToolResult::InvalidOperation;

  }

  budget.Spend(cost);

  return ToolResult::Success;
}


/* comefrom: ConnecTile */
ToolResult
_LayRail(int x, int y, int *TileAdrPtr, Budget& budget)
{
  int Tile;
  int cost = 20;

  if (budget.CurrentFunds() < 20) {
    return ToolResult::InsufficientFunds;
  }

  Tile = (*TileAdrPtr) & LOMASK;
  NeutralizeRoad(Tile);

  switch (Tile) {
  case 0:			/* Rail on Dirt */
    (*TileAdrPtr) = 226 | BULLBIT | BURNBIT;
    break;

  case 2:			/* Rail on Water */
  case 3:
  case 4:			/* Check how to build underwater tunnel, if possible. */
    if (budget.CurrentFunds() < 100) {
      return ToolResult::InsufficientFunds;
    }
    cost = 100;

    if (x < (SimWidth - 1)) {
      Tile = TileAdrPtr[SimHeight];
      NeutralizeRoad(Tile);
      if ((Tile == 221) || (Tile == 224) || ((Tile >= 226) && (Tile <= 237))) {
	(*TileAdrPtr) = 224 | BULLBIT;
	break;
      }
    }
    
    if (x > 0) {
      Tile = TileAdrPtr[-SimHeight];
      NeutralizeRoad(Tile);
      if ((Tile == 221) || (Tile == 224) || ((Tile > 225) && (Tile < 238))) {
	(*TileAdrPtr) = 224 | BULLBIT;
	break;
      }
    }

    if (y < (SimHeight - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if ((Tile == 222) || (Tile == 238) || ((Tile > 224) && (Tile < 237))) {
	(*TileAdrPtr) = 225 | BULLBIT;
	break;
      }
    }

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if ((Tile == 222) || (Tile == 238) || ((Tile > 224) && (Tile < 237))) {
	(*TileAdrPtr) = 225 | BULLBIT;
	break;
      }
    }

    /* Can't do rail... */
    return ToolResult::InvalidOperation;

  case 210:		/* Rail on power */
    (*TileAdrPtr) = 222 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 211:		/* Rail on power #2 */
    (*TileAdrPtr) = 221 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 66:		/* Rail on road */
    (*TileAdrPtr) = 238 | BURNBIT | BULLBIT;
    break;

  case 67:		/* Rail on road #2 */
    (*TileAdrPtr) = 237 | BURNBIT | BULLBIT;
    break;

  default:		/* Can't do rail */
    return ToolResult::InvalidOperation;
  }

  budget.Spend(cost);
  return ToolResult::Success;
}


/* comefrom: ConnecTile */
ToolResult
_LayWire(int x, int y, int *TileAdrPtr, Budget& budget)
{
  int Tile;
  int cost = 5;

  if (budget.CurrentFunds() < 5) {
    return ToolResult::InsufficientFunds;
  }

  Tile = (*TileAdrPtr) & LOMASK;
  NeutralizeRoad(Tile);

  switch (Tile) {
  case 0:			/* Wire on Dirt */
    (*TileAdrPtr) = 210 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 2:			/* Wire on Water */
  case 3:
  case 4:			/* Check how to lay underwater wire, if possible. */
    if (budget.CurrentFunds() < 25)
      return ToolResult::InsufficientFunds;
    cost = 25;

    if (x < (SimWidth - 1)) {
      Tile = TileAdrPtr[SimHeight];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 77) && (Tile != 221) && (Tile != 208)) {
	  (*TileAdrPtr) = 209 | CONDBIT | BULLBIT;
	  break;		
	}
      }
    }

    if (x > 0) {
      Tile = TileAdrPtr[-SimHeight];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);		
	if ((Tile != 77) && (Tile != 221) && (Tile != 208)) {
	  (*TileAdrPtr) = 209 | CONDBIT | BULLBIT;
	  break;		
	}
      }
    }

    if (y < (SimHeight - 1)) {
      Tile = TileAdrPtr[1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);		
	if ((Tile != 78) && (Tile != 222) && (Tile != 209)) {
	  (*TileAdrPtr) = 208 | CONDBIT | BULLBIT;
	  break;
	}
      }
    }

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);		
	if ((Tile != 78) && (Tile != 222) && (Tile != 209)) {
	  (*TileAdrPtr) = 208 | CONDBIT | BULLBIT;
	  break;		
	}
      }
    }

    /* Can't do wire... */
    return ToolResult::InvalidOperation;

  case 66:		/* Wire on Road */
    (*TileAdrPtr) = 77 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 67:		/* Wire on Road #2 */
    (*TileAdrPtr) = 78 | CONDBIT | BURNBIT | BULLBIT;
			break;

  case 226:		/* Wire on rail */
    (*TileAdrPtr) = 221 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 227:		/* Wire on rail #2 */
    (*TileAdrPtr) = 222 | CONDBIT | BURNBIT | BULLBIT;
    break;

  default:		/* Can't do wire */
    return ToolResult::InvalidOperation;
  }

  budget.Spend(cost);
  return ToolResult::Success;
}


/* comefrom: _FixZone */
void _FixSingle(int x, int y, int *TileAdrPtr)
{
  int Tile;
  int adjTile = 0;

  Tile = (*TileAdrPtr) & LOMASK;
  NeutralizeRoad(Tile);
  if ((Tile >= 66) && (Tile <= 76)) {		/* Cleanup Road */

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if (((Tile == 237) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 77) && (Tile != 238) && (Tile != 64))
	adjTile |= 0x0001;
    }

    if (x < (SimWidth - 1)) {
      Tile = TileAdrPtr[SimHeight];
      NeutralizeRoad(Tile);
      if (((Tile == 238) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 78) && (Tile != 237) && (Tile != 65))
	adjTile |= 0x0002;
    }

    if (y < (SimHeight - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if (((Tile == 237) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 77) && (Tile != 238) && (Tile != 64))
	adjTile |= 0x0004;
    }

    if (x > 0) {
      Tile = TileAdrPtr[-SimHeight];
      NeutralizeRoad(Tile);
      if (((Tile == 238) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 78) && (Tile != 237) && (Tile != 65))
	adjTile |= 0x0008;
    }

    (*TileAdrPtr) = _RoadTable[adjTile] | BULLBIT | BURNBIT;
    return;
  }

  if ((Tile >= 226) && (Tile <= 236)) {		/* Cleanup Rail */

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 221) && (Tile != 237) && (Tile != 224))
	adjTile |= 0x0001;
    }

    if (x < (SimWidth - 1)) {
      Tile = TileAdrPtr[SimHeight];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 222) && (Tile != 238) && (Tile != 225))
	adjTile |= 0x0002;
    }

    if (y < (SimHeight - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 221) && (Tile != 237) && (Tile != 224))
	adjTile |= 0x0004;
    }

    if (x > 0) {
      Tile = TileAdrPtr[-SimHeight];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 222) && (Tile != 238) && (Tile != 225))
	adjTile |= 0x0008;
    }

    (*TileAdrPtr) = _RailTable[adjTile] | BULLBIT | BURNBIT;
    return;
  }

  if ((Tile >= 210) && (Tile <= 220)) {		/* Cleanup Wire */

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 209) && (Tile != 78) && (Tile != 222))
	  adjTile |= 0x0001;
      }
    }

    if (x < (SimWidth - 1)) {
      Tile = TileAdrPtr[SimHeight];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 208) && (Tile != 77) && (Tile != 221))
	  adjTile |= 0x0002;
      }
    }

    if (y < (SimHeight - 1)) {
      Tile = TileAdrPtr[1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 209) && (Tile != 78) && (Tile != 222))
	  adjTile |= 0x0004;
      }
    }

    if (x > 0) {
      Tile = TileAdrPtr[-SimHeight];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 208) && (Tile != 77) && (Tile != 221))
	  adjTile |= 0x0008;
      }
    }

    (*TileAdrPtr) = _WireTable[adjTile] | BULLBIT | BURNBIT | CONDBIT;
    return;
  }
}

/* comefrom: ConnecTile */
void _FixZone(int x, int y, int *TileAdrPtr)
{
  _FixSingle(x,y, &TileAdrPtr[0]);

  if (y > 0) {
    _FixSingle(x, y-1, &TileAdrPtr[-1]);
  }

  if (x < (SimWidth - 1)) {
    _FixSingle(x+1, y, &TileAdrPtr[SimHeight]);
  }

  if (y < (SimHeight - 1)) {
    _FixSingle(x, y+1, &TileAdrPtr[1]);
  }

  if (x > 0) {
    _FixSingle(x-1, y, &TileAdrPtr[-SimHeight]);
  }

}


/* comefrom: check3Border check4Border check5Border processWand */
ToolResult ConnecTile(int x, int y, int* TileAdrPtr, int Command, Budget& budget)
{
    int Tile;

    /* AutoDoze */
    if ((Command >= 2) && (Command <= 4))
    {
        if ((autoBulldoze != 0) &&
            (budget.CurrentFunds() > 0) &&
            ((Tile = (*TileAdrPtr)) & BULLBIT))
        {
            NeutralizeRoad(Tile);
            /* Maybe this should check BULLBIT instead of checking tile values? */
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
    case 0:	/* Fix zone */
        _FixZone(x, y, TileAdrPtr);
        break;

    case 1:	/* Doze zone */
        result = _LayDoze(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    case 2:	/* Lay Road */
        result = _LayRoad(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    case 3:	/* Lay Rail */
        result = _LayRail(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    case 4:	/* Lay Wire */
        result = _LayWire(x, y, TileAdrPtr, budget);
        _FixZone(x, y, TileAdrPtr);
        break;

    }

    return result;
}
