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
#pragma once

#include "Point.h"
#include "Texture.h"

#include <string>

#include <SDL2/SDL.h>

 /* Constants */

constexpr auto SimWidth = 120;
constexpr auto SimHeight = 100;

constexpr auto HalfWorldWidth = SimWidth / 2;
constexpr auto HalfWorldHeight = SimHeight / 2;

constexpr auto QuarterWorldWidth = SimWidth / 4;
constexpr auto QuarterWorldHeight = SimHeight / 4;

constexpr auto EighthWorldWidth = SimWidth / 8;
constexpr auto EighthWorldHeight = (SimHeight + 7) / 8;

constexpr auto HistoryLength = 120;
constexpr auto MiscHistoryLength = 240;


/* These adjust frequency in Simulate() */
constexpr auto CensusRate = 4;
constexpr auto TaxFrequency = 48;


/* Status Bits */
#define PWRBIT	0x8000 /* 20 bit 15 */
#define CONDBIT	0x4000 /* 10 bit 14 */
#define BURNBIT	0x2000 /* 8 bit 13 */
#define BULLBIT	0x1000 /* 4 bit 12 */
#define ANIMBIT	0x0800 /* 2 bit 11 */
#define ZONEBIT	0x0400 /* 1 bit 10 */
#define ALLBITS	0xFC00 /* mask for upper 6 bits	*/
#define LOMASK	0x03FF /* mask for low 10 bits	*/

#define BLBNBIT		(BULLBIT+BURNBIT)
#define BLBNCNBIT	(BULLBIT+BURNBIT+CONDBIT)
#define BNCNBIT		(BURNBIT+CONDBIT)

extern int CurrentTile; // unmasked tile value
extern int CurrentTileMasked; // masked tile value

extern int RoadTotal, RailTotal, FirePop;
extern int ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
extern int ResZPop, ComZPop, IndZPop, TotalZPop;
extern int HospPop, ChurchPop, StadiumPop;
extern int PolicePop, FireStPop;
extern int CoalPop, NuclearPop, PortPop, APortPop;
extern int NeedHosp, NeedChurch;
extern int CrimeAverage, PolluteAverage, LVAverage;

extern int StartingYear;
extern int CityTime;
extern int ScenarioID;
extern int ShakeNow;

extern int RoadEffect, PoliceEffect, FireEffect;

extern bool NoDisasters;
extern bool AutoBulldoze;

extern int DisasterEvent;
extern int DisasterWait;

extern int ResCap, ComCap, IndCap;
extern int RValve, CValve, IValve;
extern int PwrdZCnt;
extern int unPwrdZCnt;

extern int ScoreType;
extern int ScoreWait;
extern int PolMaxX, PolMaxY;
extern int CrashX, CrashY;
extern int CCx, CCy;

extern int InitSimLoad;
extern int DoInitialEval;

extern SDL_Renderer* MainWindowRenderer;
extern Texture SmallTileset;

void showBudgetWindow();

bool autoBudget();
void autoBudget(const bool b);

bool autoGoto();
void autoGoto(const bool b);

bool animationEnabled();
void animationEnabled(bool b);

void simExit();
const Point<int>& viewOffset();

SDL_Rect& miniMapTileRect();
