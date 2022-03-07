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


/* Character Mapping */
constexpr auto DIRT = 0;
constexpr auto RIVER = 2;
constexpr auto REDGE = 3;
constexpr auto CHANNEL = 4;
constexpr auto FIRSTRIVEDGE = 5;
constexpr auto LASTRIVEDGE = 20;
constexpr auto TREEBASE = 21;
constexpr auto LASTTREE = 36;
constexpr auto WOODS = 37;
constexpr auto UNUSED_TRASH1 = 38;
constexpr auto UNUSED_TRASH2 = 39;
constexpr auto WOODS2 = 40;
constexpr auto WOODS3 = 41;
constexpr auto WOODS4 = 42;
constexpr auto WOODS5 = 43;
constexpr auto RUBBLE = 44;
constexpr auto LASTRUBBLE = 47;
constexpr auto FLOOD = 48;
constexpr auto LASTFLOOD = 51;
constexpr auto RADTILE = 52;
constexpr auto UNUSED_TRASH3 = 53;
constexpr auto UNUSED_TRASH4 = 54;
constexpr auto UNUSED_TRASH5 = 55;
constexpr auto FIRE = 56;
constexpr auto FIREBASE = 56;
constexpr auto LASTFIRE = 63;
constexpr auto ROADBASE = 64;
constexpr auto HBRIDGE = 64;
constexpr auto VBRIDGE = 65;
constexpr auto ROADS = 66;
constexpr auto INTERSECTION = 76;
constexpr auto HROADPOWER = 77;
constexpr auto VROADPOWER = 78;
constexpr auto BRWH = 79;
constexpr auto LTRFBASE = 80; // Light traffic base value
constexpr auto BRWV = 95;
constexpr auto BRWXXX1 = 111;
constexpr auto BRWXXX2 = 127;
constexpr auto BRWXXX3 = 143;
constexpr auto HTRFBASE = 144; // Heavy traffic base value
constexpr auto BRWXXX4 = 159;
constexpr auto BRWXXX5 = 175;
constexpr auto BRWXXX6 = 191;
constexpr auto LASTROAD = 206;
constexpr auto BRWXXX7 = 207;
constexpr auto POWERBASE = 208;
constexpr auto HPOWER = 208;
constexpr auto VPOWER = 209;
constexpr auto LHPOWER = 210;
constexpr auto LVPOWER = 211;
constexpr auto RAILHPOWERV = 221;
constexpr auto RAILVPOWERH = 222;
constexpr auto LASTPOWER = 222;
constexpr auto UNUSED_TRASH6 = 223;
constexpr auto RAILBASE = 224;
constexpr auto HRAIL = 224;
constexpr auto VRAIL = 225;
constexpr auto LHRAIL = 226;
constexpr auto LVRAIL = 227;
constexpr auto HRAILROAD = 237;
constexpr auto VRAILROAD = 238;
constexpr auto LASTRAIL = 238;
constexpr auto ROADVPOWERH = 239; /* bogus? */
constexpr auto RESBASE = 240;
constexpr auto FREEZ = 244;
constexpr auto HOUSE = 249;
constexpr auto LHTHR = 249;
constexpr auto HHTHR = 260;
constexpr auto RZB = 265;
constexpr auto HOSPITAL = 409;
constexpr auto CHURCH = 418;
constexpr auto COMBASE = 423;
constexpr auto COMCLR = 427;
constexpr auto CZB = 436;
constexpr auto INDBASE = 612;
constexpr auto INDCLR = 616;
constexpr auto LASTIND = 620;
constexpr auto IND1 = 621;
constexpr auto IZB = 625;
constexpr auto IND2 = 641;
constexpr auto IND3 = 644;
constexpr auto IND4 = 649;
constexpr auto IND5 = 650;
constexpr auto IND6 = 676;
constexpr auto IND7 = 677;
constexpr auto IND8 = 686;
constexpr auto IND9 = 689;
constexpr auto PORTBASE = 693;
constexpr auto PORT = 698;
constexpr auto LASTPORT = 708;
constexpr auto AIRPORTBASE = 709;
constexpr auto RADAR = 711;
constexpr auto AIRPORT = 716;
constexpr auto COALBASE = 745;
constexpr auto POWERPLANT = 750;
constexpr auto LASTPOWERPLANT = 760;
constexpr auto FIRESTBASE = 761;
constexpr auto FIRESTATION = 765;
constexpr auto POLICESTBASE = 770;
constexpr auto POLICESTATION = 774;
constexpr auto STADIUMBASE = 779;
constexpr auto STADIUM = 784;
constexpr auto FULLSTADIUM = 800;
constexpr auto NUCLEARBASE = 811;
constexpr auto NUCLEAR = 816;
constexpr auto LASTZONE = 826;
constexpr auto LIGHTNINGBOLT = 827;
constexpr auto HBRDG0 = 828;
constexpr auto HBRDG1 = 829;
constexpr auto HBRDG2 = 830;
constexpr auto HBRDG3 = 831;
constexpr auto RADAR0 = 832;
constexpr auto RADAR1 = 833;
constexpr auto RADAR2 = 834;
constexpr auto RADAR3 = 835;
constexpr auto RADAR4 = 836;
constexpr auto RADAR5 = 837;
constexpr auto RADAR6 = 838;
constexpr auto RADAR7 = 839;
constexpr auto FOUNTAIN = 840;
constexpr auto INDBASE2 = 844;
constexpr auto TELEBASE = 844;
constexpr auto TELELAST = 851;
constexpr auto SMOKEBASE = 852;
constexpr auto TINYEXP = 860;
constexpr auto SOMETINYEXP = 864;
constexpr auto LASTTINYEXP = 867;
constexpr auto COALSMOKE1 = 916;
constexpr auto COALSMOKE2 = 920;
constexpr auto COALSMOKE3 = 924;
constexpr auto COALSMOKE4 = 928;
constexpr auto FOOTBALLGAME1 = 932;
constexpr auto FOOTBALLGAME2 = 940;
constexpr auto VBRDG0 = 948;
constexpr auto VBRDG1 = 949;
constexpr auto VBRDG2 = 950;
constexpr auto VBRDG3 = 951;

constexpr auto TILE_COUNT = 960;

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

extern int NoDisasters;
extern int autoBulldoze;
extern int autoBudget;

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
extern int Startup;

extern int PunishCnt;

extern bool NewMap;
extern int NewMapFlags[NMAPS];
extern int MustUpdateOptions;

extern SDL_Renderer* MainWindowRenderer;
extern Texture SmallTileset;

void sim_exit();
const Point<int>& viewOffset();

SDL_Rect& miniMapTileRect();
