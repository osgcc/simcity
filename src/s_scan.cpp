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
#include "s_scan.h"

#include "Map.h"
#include "Point.h"

#include "s_alloc.h"
#include "s_sim.h"
#include "s_zone.h"

#include <array>

/* Map Updates */


bool NewMap;
int CCx, CCy, CCx2, CCy2;
int PolMaxX, PolMaxY;
int CrimeMaxX, CrimeMaxY;

namespace
{
    constexpr auto DonDither = 0;

    std::array<int, NMAPS> NewMapFlags;

    std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> tem;
    std::array<std::array<int, HalfWorldHeight>, HalfWorldWidth> tem2;

    void resetTempArrays()
    {
        for (int x = 0; x < HalfWorldWidth; x++)
        {
            for (int y = 0; y < HalfWorldHeight; y++)
            {
                tem[x][y] = 0;
                tem2[x][y] = 0;
            }
        }
    }
};


std::array<int, NMAPS>& newMapFlags()
{
    return NewMapFlags;
}


void resetNewMapFlags()
{
    NewMapFlags.fill(1);
}


/* comefrom: FireAnalysis */
void SmoothFSMap()
{
  int x, y, edge;

  for (x = 0; x < EighthWorldWidth; x++)
    for (y = 0; y < EighthWorldHeight; y++) {
      edge = 0;
      if (x) edge += FireStMap[x - 1][y];
      if (x < (EighthWorldWidth - 1)) edge += FireStMap[x + 1][y];
      if (y) edge += FireStMap[x][y - 1];
      if (y < (EighthWorldHeight - 1)) edge += FireStMap[x][y + 1];
      edge = (edge >>2) + FireStMap[x][y];
      STem[x][y] = edge >>1;
    }
  for (x = 0; x < EighthWorldWidth; x++)
    for (y = 0; y < EighthWorldHeight; y++)
      FireStMap[x][y] = STem[x][y];
}


/* comefrom: Simulate SpecialInit */
void FireAnalysis()		/* Make firerate map from firestation map  */
{
  int x,y;

  SmoothFSMap();
  SmoothFSMap();
  SmoothFSMap();
  for (x = 0; x < EighthWorldWidth; x++)
    for (y = 0; y < EighthWorldHeight; y++)
      FireRate[x][y] = FireStMap[x][y];

  NewMapFlags[DYMAP] = NewMapFlags[FIMAP] = 1;
}


/* comefrom: PopDenScan */
void ClrTemArray()
{
  int x, y, z;

  z = 0;
  for (x = 0; x < HalfWorldWidth; x++)
    for (y = 0; y < HalfWorldHeight; y++)
      tem[x][y] = z;
}


/* comefrom: PopDenScan */
int GetPDen(int Ch9)
{
  int pop;

  if (Ch9 == FREEZ) {
    pop = DoFreePop(/*Ch9*/);
    return (pop);
  }
  if (Ch9 < COMBASE) {
    pop = RZPop(Ch9);
    return (pop);
  }
  if (Ch9 < INDBASE) {
    pop = (CZPop(Ch9) <<3);
    return (pop);
  }
  if (Ch9 < PORTBASE) {
    pop = (IZPop(Ch9) <<3);
    return (pop);
  }
  return (0);
}


/* comefrom: PopDenScan */
void DoSmooth ()        /* smooths data in tem[x][y] into tem2[x][y]  */
{
  if (DonDither & 2) {
    int x, y = 0, z = 0, dir = 1;

    for (x = 0; x < HalfWorldWidth; x++) {
      for (; y != HalfWorldHeight && y != -1; y += dir) {
/*
	z += tem[(x == 0) ? x : (x - 1)][y] +
	     tem[(x == (HalfWorldWidth - 1)) ? x : (x + 1)][y] +
	     tem[x][(y == 0) ? (0) : (y - 1)] +
	     tem[x][(y == (HalfWorldHeight - 1)) ? y : (y + 1)] +
	     tem[x][y];
	tem2[x][y] = (unsigned char)(((unsigned int)z) >>2);
	z &= 0x3;
*/
	z += tem[(x == 0) ? x : (x - 1)][y] +
	     tem[(x == (HalfWorldWidth - 1)) ? x : (x + 1)][y] +
	     tem[x][(y == 0) ? (0) : (y - 1)] +
	     tem[x][(y == (HalfWorldHeight - 1)) ? y : (y + 1)] +
	     tem[x][y];
	tem2[x][y] = (unsigned char)(((unsigned int)z) >>2);
	z &= 3;
      }
      dir = -dir;
      y += dir;
    }
  } else {
    int x,y,z;

    for (x = 0; x < HalfWorldWidth; x++) {
      for (y = 0; y < HalfWorldHeight; y++) {
	z = 0;
	if (x > 0) z += tem[x - 1][y];
	if (x < (HalfWorldWidth - 1)) z += tem[x + 1][y];
	if (y > 0) z += tem[x][y - 1];
	if (y < (HalfWorldHeight - 1)) z += tem[x][y + 1];
	z = (z + tem[x][y]) >>2;
	if (z > 255) z = 255;
	tem2[x][y] = (unsigned char)z;
      }
    }
  }
}


/* comefrom: PopDenScan */
void DoSmooth2 ()        /* smooths data in tem2[x][y] into tem[x][y]  */
{
  if (DonDither & 4) {
    int x, y = 0, z = 0, dir = 1;

    for (x = 0; x < HalfWorldWidth; x++) {
      for (; y != HalfWorldHeight && y != -1; y += dir) {
/*
	z += tem2[(x == 0) ? x : (x - 1)][y] +
	     tem2[(x == (HalfWorldWidth - 1)) ? x : (x + 1)][y] +
	     tem2[x][(y == 0) ? (0) : (y - 1)] +
	     tem2[x][(y == (HalfWorldHeight - 1)) ? y : (y + 1)] +
	     tem2[x][y];
	tem[x][y] = (unsigned char)(z >>2);
	z &= 0x3;
*/
	z += tem2[(x == 0) ? x : (x - 1)][y] +
	    tem2[(x == (HalfWorldWidth - 1)) ? x : (x + 1)][y] +
	    tem2[x][(y == 0) ? (0) : (y - 1)] +
	    tem2[x][(y == (HalfWorldHeight - 1)) ? y : (y + 1)] +
	    tem2[x][y];
	tem[x][y] = (unsigned char)(((unsigned char)z) >>2);
	z &= 3;
      }
      dir = -dir;
      y += dir;
    }
  } else {
    int x,y,z;

    for (x = 0; x < HalfWorldWidth; x++) {
      for (y = 0; y < HalfWorldHeight; y++) {
	z = 0;
	if (x > 0) z += tem2[x - 1][y];
	if (x < (HalfWorldWidth - 1)) z += tem2[x + 1][y];
	if (y > 0) z += tem2[x][y - 1];
	if (y < (HalfWorldHeight - 1)) z += tem2[x][y + 1];
	z = (z + tem2[x][y]) >>2;
	if (z > 255) z = 255;
	tem[x][y] = (unsigned char)z;
      }
    }
  }
}


/* comefrom: PTLScan */
int GetPValue(int loc)
{
  if (loc < POWERBASE) {
    if (loc >= HTRFBASE) return (/* 25 */ 75);	/* heavy traf  */
    if (loc >= LTRFBASE) return (/* 10 */ 50);	/* light traf  */
    if (loc <  ROADBASE) {
      if (loc > FIREBASE) return (/* 60 */ 90);
      /* XXX: Why negative pollution from radiation? */
      if (loc >= RADTILE) return (/* -40 */ 255);	/* radioactivity  */
    }
    return (0);
  }
  if (loc <= LASTIND) return (0);
  if (loc < PORTBASE) return (50);	/* Ind  */
  if (loc <= LASTPOWERPLANT) return (/* 60 */ 100);	/* prt, aprt, cpp */
  return (0);
}


/* comefrom: PTLScan DistIntMarket */
int GetDisCC(int x, int y)
{
  int xdis, ydis, z;

  if (x > CCx2)
    xdis = x - CCx2;
  else
    xdis = CCx2 - x;

  if (y > CCy2)
    ydis = y - CCy2;
  else
    ydis = CCy2 - y;

  z = (xdis + ydis);
  if (z > 32)
    return (32);
  else
    return (z);
}


/* comefrom: PopDenScan */
void DistIntMarket()
{
  int x, y, z;

  for (x = 0; x < EighthWorldWidth; x++)
    for (y = 0; y < EighthWorldHeight; y++) {
      z = GetDisCC(x <<2,y <<2);
      z = z <<2;
      z = 64 - z;
      ComRate[x][y] = z;
  }
}


/* comefrom: Simulate SpecialInit */
void PopDenScan()		/*  sets: PopDensity, , , ComRate  */
{
  int Xtot, Ytot, Ztot;
  int x, y, z;

  ClrTemArray();
  Xtot = 0;
  Ytot = 0;
  Ztot = 0;
  for (x = 0; x < SimWidth; x++)
    for (y = 0; y < SimHeight; y++) {
      z = Map[x][y];
      if (z & ZONEBIT) {
	z = z & LOMASK;
    SimulationTarget = { x, y };
	z = GetPDen(z) <<3;
	if (z > 254)
	  z = 254;
	tem[x >>1][y >>1] = z;
	Xtot += x;
	Ytot += y;
	Ztot++;
      }
    }
  DoSmooth();			/* T1 -> T2 */
  DoSmooth2();			/* T2 -> T1 */
  DoSmooth();			/* T1 -> T2 */

  for (x = 0; x < HalfWorldWidth; x++)
    for (y = 0; y < HalfWorldHeight; y++)
      PopDensity[x][y] = tem2[x][y] <<1;

  DistIntMarket();		/* set ComRate w/ (/ComMap) */

  if (Ztot) {			/* Find Center of Mass for City */
    CCx = Xtot / Ztot;
    CCy = Ytot / Ztot;
  } else {
    CCx = HalfWorldWidth;		/* if pop=0 center of Map is CC */
    CCy = HalfWorldHeight;
  }
  CCx2 = CCx >>1;
  CCy2 = CCy >>1;
  NewMapFlags[DYMAP] = NewMapFlags[PDMAP] = NewMapFlags[RGMAP] = 1;
}


/* comefrom: PTLScan */
void SmoothTerrain()
{
  if (DonDither & 1) {
    int x, y = 0, z = 0, dir = 1;

    for (x = 0; x < QuarterWorldWidth; x++) {
      for (; y != QuarterWorldHeight && y != -1; y += dir) {
	z += Qtem[(x == 0) ? x : (x - 1)][y] +
	     Qtem[(x == (QuarterWorldWidth - 1)) ? x : (x + 1)][y] +
	     Qtem[x][(y == 0) ? (0) : (y - 1)] +
	     Qtem[x][(y == (QuarterWorldHeight - 1)) ? y : (y + 1)] +
	     (Qtem[x][y] <<2);
	TerrainMem[x][y] = (unsigned char)(((unsigned)z) >>3);
	z &= 0x7;
      }
      dir = -dir;
      y += dir;
    }
  } else {
    int x,y,z;

    for (x = 0; x < QuarterWorldWidth; x++)
      for (y = 0; y < QuarterWorldHeight; y++) {
	z = 0;
	if (x > 0) z += Qtem[x - 1][y];
	if (x < (QuarterWorldWidth - 1)) z += Qtem[x + 1][y];
	if (y > 0) z += Qtem[x][y - 1];
	if (y < (QuarterWorldHeight - 1)) z += Qtem[x][y + 1];
	TerrainMem[x][y] = (unsigned char)((z >>2) + Qtem[x][y]) >>1;
      }
  }
}


/* comefrom: CrimeScan */
void SmoothPSMap()
{
  int x, y, edge;

  for (x = 0; x < EighthWorldWidth; x++)
    for (y = 0; y < EighthWorldHeight; y++) {
      edge = 0;
      if (x) edge += PoliceMap[x - 1][y];
      if (x < (EighthWorldWidth - 1)) edge += PoliceMap[x + 1][y];
      if (y) edge += PoliceMap[x][y - 1];
      if (y < (EighthWorldHeight - 1)) edge += PoliceMap[x][y + 1];
      edge = (edge >>2) + PoliceMap[x][y];
      STem[x][y] = edge >>1;
    }
  for (x = 0; x < EighthWorldWidth; x++)
    for (y = 0; y < EighthWorldHeight; y++)
      PoliceMap[x][y] = STem[x][y];
}


/* comefrom: Simulate SpecialInit */
void PTLScan()   	/* Does pollution, terrain, land value   */
{
  int ptot, LVtot;
  int x, y, z, dis;
  int Plevel, LVflag, loc, zx, zy, Mx, My, pnum, LVnum, pmax;

  for (x = 0; x < QuarterWorldWidth; x++)
    for (y = 0; y < QuarterWorldHeight; y++)
      Qtem[x][y] = 0;
  LVtot = 0;
  LVnum = 0;
  for (x = 0; x < HalfWorldWidth; x++)
    for (y = 0; y < HalfWorldHeight; y++) {
      Plevel = 0;
      LVflag = 0;
      zx = x <<1;
      zy = y <<1;
      for (Mx = zx; Mx <= zx + 1; Mx++)
	for (My = zy; My <= zy + 1; My++) {
	  if (loc = (Map[Mx][My] & LOMASK)) {
	    if (loc < RUBBLE) {
	      Qtem[x >>1][y >>1] += 15;	/* inc terrainMem */
	      continue;
	    }
	    Plevel += GetPValue(loc);
	    if (loc >= ROADBASE)
	      LVflag++;
	  }
	}
/* XXX ???
      if (Plevel < 0)
	Plevel = 250;
*/
      if (Plevel > 255)
	Plevel = 255;
      tem[x][y] = Plevel;
      if (LVflag) {			/* LandValue Equation */
	dis = 34 - GetDisCC(x, y);
	dis = dis <<2;
	dis += (TerrainMem[x >>1][y >>1] );
	dis -= (PollutionMem[x][y]);
	if (CrimeMem[x][y] > 190) dis -= 20;
	if (dis > 250) dis = 250;
	if (dis < 1) dis = 1;
	LandValueMem[x][y] = dis;
	LVtot += dis;
	LVnum++;
      } else
	LandValueMem[x][y] = 0;
    }

  if (LVnum)
    LVAverage = LVtot / LVnum;
  else
    LVAverage = 0;

  DoSmooth();
  DoSmooth2();
  pmax = 0;
  pnum = 0;
  ptot = 0;
  for (x = 0; x < HalfWorldWidth; x++) {
    for (y = 0; y < HalfWorldHeight; y++)  {
      z = tem[x][y];
      PollutionMem[x][y] = z;
      if (z) {				/*  get pollute average  */
	pnum++;
	ptot += z;
	/* find max pol for monster  */
	if ((z > pmax) ||
	    ((z == pmax) && (!(Rand16() & 3)))) {
	  pmax = z;
	  PolMaxX = x <<1;
	  PolMaxY = y <<1;
	}
      }
    }
  }
  if (pnum)
    PolluteAverage = ptot / pnum;
  else
    PolluteAverage = 0;

  SmoothTerrain();

  NewMapFlags[DYMAP] = NewMapFlags[PLMAP] = NewMapFlags[LVMAP] = 1;
}


/* comefrom: Simulate SpecialInit */
void CrimeScan()
{
  int numz;
  int totz;
  int x, y, z;
  int cmax;

  SmoothPSMap();
  SmoothPSMap();
  SmoothPSMap();
  totz = 0;
  numz = 0;
  cmax = 0;
  for (x = 0; x < HalfWorldWidth; x++)
    for (y = 0; y < HalfWorldHeight; y++) {
      if (z = LandValueMem[x][y]) {
	++numz;
	z = 128 - z;
	z += PopDensity[x][y];
	if (z > 300) z = 300;
	z -= PoliceMap[x >>2][y >>2];
	if (z > 250) z = 250;
	if (z < 0) z = 0;
	CrimeMem[x][y] = z;
	totz += z;
	if ((z > cmax) ||
	    ((z == cmax) && (!(Rand16() & 3)))) {
	  cmax = z;
	  CrimeMaxX = x <<1;
	  CrimeMaxY = y <<1;
	}
      } else {
	CrimeMem[x][y] = 0;
      }
    }
  if (numz)
    CrimeAverage = totz / numz;
  else
    CrimeAverage = 0;
  for (x = 0; x < EighthWorldWidth; x++)
    for (y = 0; y < EighthWorldHeight; y++)
      PoliceMapEffect[x][y] = PoliceMap[x][y];
  NewMapFlags[DYMAP] = NewMapFlags[CRMAP] = NewMapFlags[POMAP] = 1;
}
