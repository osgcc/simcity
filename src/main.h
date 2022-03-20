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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <malloc.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <io.h>

#include <string>

#include <SDL2/SDL.h>

 /* Constants */

constexpr auto SimWidth = 120;
constexpr auto SimHeight = 100;

constexpr auto HalfWorldWidth = SimWidth / 2;
constexpr auto HalfWorldHeight = SimHeight / 2;

constexpr auto QuarterWorldWidth = SimWidth / 4;
constexpr auto QuarterWorldHeight = SimHeight / 4;

constexpr auto EightWorldWidth = SimWidth / 8;
constexpr auto EightWorldHeight = (SimHeight + 7) / 8;

constexpr auto SmX = SimWidth / 8;
constexpr auto SmY = (SimHeight + 7) / 8;

constexpr auto PROBNUM = 10;

constexpr auto HistoryLength = 120;
constexpr auto MISCHISTLEN = 240;

constexpr auto POWERMAPROW = ((SimWidth + 15) / 16);

#define POWERWORD(x, y)		(((x) >>4) + ((y) <<3))

#define SETPOWERBIT(x, y)	PowerMap[POWERWORD((x), (y))] |= 1 << ((x) & 15)
constexpr auto PWRMAPSIZE = (POWERMAPROW * SimHeight);
#define PWRSTKSIZE		((SimWidth * SimHeight) / 4)

#define ALMAP 0 /* all */
#define REMAP 1 /* residential */
#define COMAP 2 /* commercial */
#define INMAP 3 /* industrial */

#define PRMAP 4 /* power */
#define RDMAP 5 /* road */

#define PDMAP 6 /* population density */
#define RGMAP 7 /* rate of growth */

#define TDMAP 8 /* traffic density */
#define PLMAP 9 /* pollution */
#define CRMAP 10 /* crime */
#define LVMAP 11 /* land value */

#define FIMAP 12 /* fire radius */
#define POMAP 13 /* police radius */
#define DYMAP 14 /* dynamic */

#define NMAPS 15


/* These adjust frequency in Simulate() */
#define CENSUSRATE 4
#define TAXFREQ 48


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

extern int CChr; // unmasked tile value
extern int CChr9; // masked tile value

extern int SMapX, SMapY;
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

extern int DoAnimation;

extern bool NoDisasters;
extern bool AutoBulldoze;

extern int DisasterEvent;
extern int DisasterWait;

extern int ResCap, ComCap, IndCap;
extern int RValve, CValve, IValve;
extern int PwrdZCnt;
extern int unPwrdZCnt;

extern int CityScore;
extern int deltaCityScore;
extern int ScoreType;
extern int ScoreWait;
extern int CityClass;
extern int PolMaxX, PolMaxY;
extern int PowerStackNum;
extern int TrafficAverage;
extern int CrashX, CrashY;
extern int CCx, CCy;
extern int CityPop, deltaCityPop;
extern int CityYes, CityNo;
extern int ProblemOrder[4];
extern int CityAssessedValue;

extern int InitSimLoad;
extern int DoInitialEval;

extern int NewMapFlags[NMAPS];

extern SDL_Renderer* MainWindowRenderer;
extern Texture SmallTileset;

void showBudgetWindow();

bool autoBudget();
void autoBudget(const bool b);

bool autoGoto();
void autoGoto(const bool b);

void sim_exit();
const Point<int>& viewOffset();

SDL_Rect& miniMapTileRect();
