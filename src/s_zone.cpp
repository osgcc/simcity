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
#include "Power.h"

#include "s_alloc.h"
#include "s_sim.h"
#include "s_traf.h"

#include "w_util.h"


/* Zone Stuff */

bool SetZPower()		/* set bit in MapWord depending on powermap  */
{
  bool z = testPowerBit(SimulationTarget);

  if (z)
    Map[SimulationTarget.x][SimulationTarget.y] = CurrentTile | PWRBIT;
  else
    Map[SimulationTarget.x][SimulationTarget.y] = CurrentTile & (~PWRBIT);

  return (z);
}


void ZonePlop (int base)
{
  int z, x;
  static int Zx[9] = {-1, 0, 1,-1, 0, 1,-1, 0, 1};
  static int Zy[9] = {-1,-1,-1, 0, 0, 0, 1, 1, 1};

  for (z = 0; z < 9; z++) {		/* check for fire  */
    int xx = SimulationTarget.x + Zx[z];
    int yy = SimulationTarget.y + Zy[z];
    if (CoordinatesValid({ xx, yy })) {
      x = Map[xx][yy] & LOMASK;
      if ((x >= FLOOD) && (x < ROADBASE)) return;
    }
  }
  for (z = 0; z < 9; z++) {
    int xx = SimulationTarget.x + Zx[z];
    int yy = SimulationTarget.y + Zy[z];
    if (CoordinatesValid({ xx, yy })) {
      Map[xx][yy] = base + BNCNBIT;
    }
  base++;
  }
  CurrentTile = Map[SimulationTarget.x][SimulationTarget.y];
  SetZPower();
  Map[SimulationTarget.x][SimulationTarget.y] |= ZONEBIT + BULLBIT;
}


void ResPlop (int Den, int Value)
{
  int base;

  base = (((Value * 4) + Den) * 9) + RZB - 4;
  ZonePlop(base);
}


void ComPlop (int Den, int Value)
{
  int base;
	
  base = (((Value * 5) + Den) * 9) + CZB - 4;
  ZonePlop(base);
}


void IndPlop (int Den, int Value)
{
  int base;
	
  base = (((Value * 4) + Den) * 9) + (IZB - 4);
  ZonePlop (base);
}


int RZPop(int Ch9)
{
  int CzDen;

  CzDen = (((Ch9 - RZB) / 9) % 4);
  return ((CzDen * 8) + 16);
}


int CZPop(int Ch9)
{
  int CzDen;

  if (Ch9 == COMCLR) return (0);
  CzDen = (((Ch9 - CZB) / 9) % 5) + 1;
  return (CzDen);
}


int IZPop(int Ch9)
{
  int CzDen;

  if (Ch9 == INDCLR) return (0);
  CzDen = (((Ch9 - IZB) / 9) % 4) + 1;
  return (CzDen);
}


void DoHospChur()
{
  if (CurrentTileMasked == HOSPITAL) {
    HospPop++;
    if (!(CityTime & 15)) RepairZone (HOSPITAL, 3); /*post*/
    if (NeedHosp == -1)
      if (!RandomRange(0, 20))
	ZonePlop(RESBASE);
  }
  if (CurrentTileMasked == CHURCH) {
    ChurchPop++;
    if (!(CityTime & 15)) RepairZone (CHURCH, 3); /*post*/
    if (NeedChurch == -1)
      if (!RandomRange(0, 20))
	ZonePlop(RESBASE);
  }
}


#define T 1
#define F 0
#define ASCBIT (ANIMBIT | CONDBIT | BURNBIT)
#define REGBIT (CONDBIT | BURNBIT)

void SetSmoke(int ZonePower)
{
  static int AniThis[8] = {    T,    F,    T,    T,    F,    F,    T,    T };
  static int DX1[8]	  = {   -1,    0,    1,    0,    0,    0,    0,    1 };
  static int DY1[8]	  = {   -1,    0,   -1,   -1,    0,    0,   -1,   -1 };
  static int DX2[8]	  = {   -1,    0,    1,    1,    0,    0,    1,    1 };
  static int DY2[8]	  = {   -1,    0,    0,   -1,    0,    0,   -1,    0 };
  static int AniTabA[8] = {    0,    0,   32,   40,    0,    0,   48,   56 };
  static int AniTabB[8] = {    0,    0,   36,   44,    0,    0,   52,   60 };
  static int AniTabC[8] = { IND1,    0, IND2, IND4,    0,    0, IND6, IND8 };
  static int AniTabD[8] = { IND1,    0, IND3, IND5,    0,    0, IND7, IND9 };
  int z;

  if (CurrentTileMasked < IZB) return;
  z = (CurrentTileMasked - IZB) >>3;
  z = z & 7;
  if (AniThis[z]) {
    int xx = SimulationTarget.x + DX1[z];
    int yy = SimulationTarget.y + DY1[z];
    if (CoordinatesValid({ xx, yy })) {
      if (ZonePower) {
	if ((Map[xx][yy] & LOMASK) == AniTabC[z]) {
	  Map[xx][yy] =
	    ASCBIT | (SMOKEBASE + AniTabA[z]);
	  Map[xx][yy] =
	    ASCBIT | (SMOKEBASE + AniTabB[z]);
	}
      } else {
	if ((Map[xx][yy] & LOMASK) > AniTabC[z]) {
	  Map[xx][yy] =
	    REGBIT | AniTabC[z];
	  Map[xx][yy] =
	    REGBIT | AniTabD[z];
	}
      }
    }
  }
}


void MakeHosp()
{
  if (NeedHosp > 0) {
    ZonePlop(HOSPITAL - 4);
    NeedHosp = false;
    return;
  }
  if (NeedChurch > 0) {
    ZonePlop(CHURCH - 4);
    NeedChurch = false;
    return;
  }
}


int GetCRVal()
{
  int LVal;

  LVal = LandValueMap[SimulationTarget.x >>1][SimulationTarget.y >>1];
  LVal -= PollutionMap[SimulationTarget.x >>1][SimulationTarget.y >>1];
  if (LVal < 30) return (0);
  if (LVal < 80) return (1);
  if (LVal < 150) return (2);
  return (3);
}


int EvalLot (int x, int y)
{
  int z, score;
  static int DX[4] = { 0, 1, 0,-1};
  static int DY[4] = {-1, 0, 1, 0};

  /* test for clear lot */
  z = Map[x][y] & LOMASK;
  if (z && ((z < RESBASE) || (z > RESBASE + 8)))
    return (-1);
  score = 1;
  for (z = 0; z < 4; z++) {
    int xx = x + DX[z];
    int yy = y + DY[z];
    if (CoordinatesValid({ xx, yy }) &&
	Map[xx][yy] &&
	((Map[xx][yy] & LOMASK) <= LASTROAD)) {
      score++;		/* look for road */
    }
  }
  return (score);
}


int EvalRes (int traf)
{
  int Value;

  if (traf < 0) return (-3000);

  Value = LandValueMap[SimulationTarget.x >>1][SimulationTarget.y >>1];
  Value -= PollutionMap[SimulationTarget.x >>1][SimulationTarget.y >>1];

  if (Value < 0) Value = 0;		/* Cap at 0 */
  else Value = Value <<5;

  if (Value > 6000) Value = 6000;	/* Cap at 6000 */

  Value = Value - 3000;
  return (Value);
}


int EvalCom (int traf)
{
  int Value;

  if (traf < 0) return (-3000);
  Value = ComRate[SimulationTarget.x >>3][SimulationTarget.y >>3];
  return (Value);
}


int EvalInd (int traf)
{
  if (traf < 0) return (-1000);
  return (0);
}


void BuildHouse(int value)
{
  int z, score, hscore, BestLoc;
  static int ZeX[9] = { 0,-1, 0, 1,-1, 1,-1, 0, 1};
  static int ZeY[9] = { 0,-1,-1,-1, 0, 0, 1, 1, 1};

  BestLoc = 0;
  hscore = 0;
  for (z = 1; z < 9; z++) {
    int xx = SimulationTarget.x + ZeX[z];
    int yy = SimulationTarget.y + ZeY[z];
    if (CoordinatesValid({ xx, yy })) {
      score = EvalLot(xx, yy);
      if (score != 0) {
	if (score > hscore) {
	  hscore = score;
	  BestLoc = z;
	}
	if ((score == hscore) && !(Rand16() & 7))
	  BestLoc = z;
      }
    }
  }
  if (BestLoc) {
    int xx = SimulationTarget.x + ZeX[BestLoc];
    int yy = SimulationTarget.y + ZeY[BestLoc];
    if (CoordinatesValid({ xx, yy })) {
      Map[xx][yy] = HOUSE + BLBNCNBIT + RandomRange(0, 2) + (value * 3);
    }
  }
}


void IncROG(int amount)
{
  RateOfGrowthMap[SimulationTarget.x>>3][SimulationTarget.y>>3] += amount<<2;
}


void DoResIn(int pop, int value)
{
  int z;

  z = PollutionMap[SimulationTarget.x >>1][SimulationTarget.y >>1];
  if (z > 128) return;

  if (CurrentTileMasked == FREEZ) {
    if (pop < 8) {
      BuildHouse(value);
      IncROG(1);
      return;
    }
    if (PopulationDensityMap[SimulationTarget.x >>1][SimulationTarget.y >>1] > 64) {
      ResPlop(0, value);
      IncROG(8);
      return;
    }
    return;
  }
  if (pop < 40) {
    ResPlop((pop / 8) - 1, value);
    IncROG(8);
  }
}


void DoComIn(int pop, int value)
{
  int z;

  z = LandValueMap[SimulationTarget.x >>1][SimulationTarget.y >>1];
  z = z >>5;
  if (pop > z) return;

  if (pop < 5) {
    ComPlop(pop, value);
    IncROG(8);
  }
}


void DoIndIn(int pop, int value)
{
  if (pop < 4) {
    IndPlop(pop, value);
    IncROG(8);
  }
}


void DoResOut(int pop, int value)
{
  static int Brdr[9] = {0,3,6,1,4,7,2,5,8};
  int x, y, loc, z;

  if (!pop) return;
  if (pop > 16) {
    ResPlop(((pop - 24) / 8), value);
    IncROG(-8);
    return;
  }
  if (pop == 16) {
    IncROG(-8);
    Map[SimulationTarget.x][SimulationTarget.y] = (FREEZ | BLBNCNBIT | ZONEBIT);
    for (x = SimulationTarget.x - 1; x <= SimulationTarget.x + 1; x++)
      for (y = SimulationTarget.y - 1; y <= SimulationTarget.y + 1; y++)
	if (x >= 0 && x < SimWidth &&
	    y >= 0 && y < SimHeight) {
	  if ((Map[x][y] & LOMASK) != FREEZ)
	    Map[x][y] = LHTHR + value +
          RandomRange(0, 2) + BLBNCNBIT;
	}
  }
  if (pop < 16) {
    IncROG(-1);
    z = 0;
    for (x = SimulationTarget.x - 1; x <= SimulationTarget.x + 1; x++)
      for (y = SimulationTarget.y - 1; y <= SimulationTarget.y + 1; y++) {
	if (x >= 0 && x < SimWidth &&
	    y >= 0 && y < SimHeight) {
	  loc = Map[x][y] & LOMASK;
	  if ((loc >= LHTHR) && (loc <= HHTHR)) {
	    Map[x][y] = Brdr[z] +
	      BLBNCNBIT + FREEZ - 4;
	    return;
	  }
	}
	z++;
      }
  }
}


void DoComOut(int pop, int value)
{
  if (pop > 1) {
    ComPlop(pop - 2, value);
    IncROG(-8);
    return;
  }
  if (pop == 1) {
    ZonePlop(COMBASE);
    IncROG(-8);
  }
}


void DoIndOut(int pop, int value)
{
  if (pop > 1) {
    IndPlop(pop - 2, value);
    IncROG(-8);
    return;
  }
  if (pop == 1) {
    ZonePlop(INDCLR - 4);
    IncROG(-8);
  }
}


int DoFreePop ()
{
  int count;
  int loc, x, y;

  count = 0;
  for (x = SimulationTarget.x - 1; x <= SimulationTarget.x + 1; x++)
    for (y = SimulationTarget.y - 1; y <= SimulationTarget.y + 1; y++) {
      if (x >= 0 && x < SimWidth &&
	  y >= 0 && y < SimHeight) {
	loc = Map[x][y] & LOMASK;
	if ((loc >= LHTHR) && (loc <= HHTHR))
	  count++;
      }
    }
  return (count);
}


void DoIndustrial(int ZonePwrFlg)
{
    int tpop, zscore, TrfGood;

    IndZPop++;

    SetSmoke(ZonePwrFlg);

    tpop = IZPop(CurrentTileMasked);
    IndPop += tpop;

    if (tpop > RandomRange(0, 5))
    {
        TrfGood = MakeTraf(2);
    }
    else
    {
        TrfGood = true;
    }

    if (TrfGood == -1)
    {
        DoIndOut(tpop, Rand16() & 1);
        return;
    }

    if (!(Rand16() & 7))
    {
        zscore = IValve + EvalInd(TrfGood);

        if (!ZonePwrFlg)
        {
            zscore = -500;
        }

        if ((zscore > -350) && (zscore - 26380) > Rand16())
        {
            DoIndIn(tpop, Rand16() & 1);
            return;
        }

        if ((zscore < 350) && (zscore + 26380) < Rand16())
        {
            DoIndOut(tpop, Rand16() & 1);
        }
    }
}


void DoCommercial(int ZonePwrFlg)
{
    int TrfGood;
    int zscore, locvalve, value;

    ComZPop++;

    int tpop = CZPop(CurrentTileMasked);

    ComPop += tpop;
   
    if (tpop > RandomRange(0, 5))
    {
        TrfGood = MakeTraf(1);
    }
    else
    {
        TrfGood = 1;
    }

    if (TrfGood == -1)
    {
        value = GetCRVal();
        DoComOut(tpop, value);
        return;
    }

    if (!(Rand16() & 7))
    {
        locvalve = EvalCom(TrfGood);
        zscore = CValve + locvalve;
        
        if (!ZonePwrFlg)
        {
            zscore = -500;
        }

        if (TrfGood && (zscore > -350) && zscore - 26380 > Rand16())
        {
            value = GetCRVal();
            DoComIn(tpop, value);
            return;
        }
        if (zscore < 350 && zscore + 26380 < Rand16())
        {
            value = GetCRVal();
            DoComOut(tpop, value);
        }
    }
}


void DoResidential(int ZonePwrFlg)
{
    int tpop, value, TrfGood;

    ResZPop++;
    if (CurrentTileMasked == FREEZ)
    {
        tpop = DoFreePop();
    }
    else
    {
        tpop = RZPop(CurrentTileMasked);
    }

    ResPop += tpop;
    if (tpop > RandomRange(0, 35))
    {
        TrfGood = MakeTraf(0);
    }
    else
    {
        TrfGood = true;
    }

    if (TrfGood == -1)
    {
        value = GetCRVal();
        DoResOut(tpop, value);
        return;
    }

    if ((CurrentTileMasked == FREEZ) || (!(Rand16() & 7)))
    {
        int locvalve = EvalRes(TrfGood);
        int zscore = RValve + locvalve;
        if (!ZonePwrFlg)
        {
            zscore = -500;
        }

        if (zscore > -350 && zscore - 26380 > -Rand16())
        {
            if ((!tpop) && (!(Rand16() & 3)))
            {
                MakeHosp();
                return;
            }
            value = GetCRVal();
            DoResIn(tpop, value);
            return;
        }
        if ((zscore < 350) && zscore + 26380 < Rand16())
        {
            value = GetCRVal();
            DoResOut(tpop, value);
        }
    }
}


void DoZone(const CityProperties& properties)
{
  int ZonePwrFlg;

  ZonePwrFlg = SetZPower();	/* Set Power Bit in Map from PowerMap */
  if (ZonePwrFlg) PwrdZCnt++;
  else unPwrdZCnt++;

  if (CurrentTileMasked > PORTBASE) {	/* do Special Zones  */
    DoSPZone(ZonePwrFlg, properties);
    return;
  }
  if (CurrentTileMasked < HOSPITAL) {	
    DoResidential(ZonePwrFlg);
    return;
  }
  if (CurrentTileMasked < COMBASE) {
    DoHospChur();
    return;
  }
  if (CurrentTileMasked < INDBASE)  {
    DoCommercial(ZonePwrFlg);
    return;
  }
  DoIndustrial(ZonePwrFlg);
  return;
}
