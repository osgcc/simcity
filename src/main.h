/* sim.h:  Main include file
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

#pragma once

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

 /* Constants */

constexpr auto SimWidth = 120;
constexpr auto SimHeight = 100;

constexpr auto HalfWorldWidth = SimWidth / 2;
constexpr auto HalfWorldHeight = SimHeight / 2;

constexpr auto QuarterWorldWidth = SimWidth / 4;
constexpr auto QuarterWorldHeight = SimHeight / 4;

constexpr auto SmX = SimWidth / 8;
constexpr auto SmY = (SimHeight + 7) / 8;

#define EDITOR_W (SimWidth * 16)
#define EDITOR_H (SimHeight * 16)

#define MAP_W (SimWidth * 3)
#define MAP_H (SimHeight * 3)

#define NIL		0
#define HORIZ		1
#define VERT		0

#define PROBNUM 10

constexpr auto HistoryLength = 240;
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

#define VALVERATE 2
#define CENSUSRATE 4
#define TAXFREQ 48

/* These are names of the 16 colors */
#define COLOR_WHITE		0
#define COLOR_YELLOW		1
#define COLOR_ORANGE		2
#define COLOR_RED		3
#define COLOR_DARKRED		4
#define COLOR_DARKBLUE		5
#define COLOR_LIGHTBLUE		6
#define COLOR_BROWN		7
#define COLOR_LIGHTGREEN	8
#define COLOR_DARKGREEN		9
#define COLOR_OLIVE		10
#define COLOR_LIGHTBROWN	11
#define COLOR_LIGHTGRAY		12
#define COLOR_MEDIUMGRAY	13
#define COLOR_DARKGRAY		14
#define COLOR_BLACK		15

/* Status Bits */

#define PWRBIT		32768	/*20	bit 15	*/
#define CONDBIT		16384	/*10	bit 14	*/
#define BURNBIT		8192	/*8	bit 13	*/
#define BULLBIT		4096	/*4	bit 12	*/
#define ANIMBIT		2048	/*2	bit 11	*/
#define ZONEBIT		1024	/*1	bit 10	*/
#define ALLBITS		64512	/*  mask for upper 6 bits	*/
#define LOMASK		1023	/*	mask for low 10 bits	*/

#define BLBNBIT		(BULLBIT+BURNBIT)
#define BLBNCNBIT	(BULLBIT+BURNBIT+CONDBIT)
#define BNCNBIT		(BURNBIT+CONDBIT)

/* Object & Sound Numbers */

#define TRA		1
#define COP		2
#define AIR		3
#define SHI		4
#define GOD		5
#define TOR		6
#define EXP		7
#define BUS		8

/* Max # of Objects */

#define OBJN		9

/* Graph Histories */
#define RES_HIST	0
#define COM_HIST	1
#define IND_HIST	2
#define MONEY_HIST	3
#define CRIME_HIST	4
#define POLLUTION_HIST	5
#define HISTORIES	6
#define ALL_HISTORIES	((1 <<HISTORIES) - 1)

/* Character Mapping */

#define DIRT		0
#define RIVER		2
#define REDGE		3
#define CHANNEL		4
#define FIRSTRIVEDGE	5
#define LASTRIVEDGE	20
#define TREEBASE	21
#define LASTTREE	36
#define WOODS		37
#define UNUSED_TRASH1	38
#define UNUSED_TRASH2	39
#define WOODS2		40
#define WOODS3		41
#define WOODS4		42
#define WOODS5		43
#define RUBBLE		44
#define LASTRUBBLE	47
#define FLOOD		48
#define LASTFLOOD	51
#define RADTILE		52
#define UNUSED_TRASH3	53
#define UNUSED_TRASH4	54
#define UNUSED_TRASH5	55
#define FIRE		56
#define FIREBASE	56
#define LASTFIRE	63
#define ROADBASE	64
#define HBRIDGE		64
#define VBRIDGE		65
#define ROADS		66
#define INTERSECTION	76
#define HROADPOWER	77
#define VROADPOWER	78
#define BRWH		79
#define LTRFBASE	80
#define BRWV		95
#define BRWXXX1		111
#define BRWXXX2		127
#define BRWXXX3		143
#define HTRFBASE	144
#define BRWXXX4		159
#define BRWXXX5		175
#define BRWXXX6		191
#define LASTROAD	206
#define BRWXXX7		207
#define POWERBASE	208
#define HPOWER		208
#define VPOWER		209
#define LHPOWER		210
#define LVPOWER		211
#define RAILHPOWERV	221
#define RAILVPOWERH	222
#define LASTPOWER	222
#define UNUSED_TRASH6	223
#define RAILBASE	224
#define HRAIL		224
#define VRAIL		225
#define LHRAIL		226
#define LVRAIL		227
#define HRAILROAD	237
#define VRAILROAD	238
#define LASTRAIL	238
#define ROADVPOWERH	239 /* bogus? */
#define RESBASE		240
#define FREEZ		244
#define HOUSE		249
#define LHTHR		249
#define HHTHR		260
#define RZB		265
#define HOSPITAL	409
#define CHURCH		418
#define COMBASE		423
#define COMCLR		427
#define CZB		436
#define INDBASE		612
#define INDCLR		616
#define LASTIND		620
#define IND1		621
#define IZB		625
#define IND2		641
#define IND3		644
#define IND4		649
#define IND5		650
#define IND6		676
#define IND7		677
#define IND8		686
#define IND9		689
#define PORTBASE	693
#define PORT		698
#define LASTPORT	708
#define AIRPORTBASE	709
#define RADAR		711
#define AIRPORT		716
#define COALBASE	745
#define POWERPLANT	750
#define LASTPOWERPLANT	760
#define FIRESTBASE	761
#define FIRESTATION	765
#define POLICESTBASE	770
#define POLICESTATION	774
#define STADIUMBASE	779
#define STADIUM		784
#define FULLSTADIUM	800
#define NUCLEARBASE	811
#define NUCLEAR		816
#define LASTZONE	826
#define LIGHTNINGBOLT	827
#define HBRDG0		828
#define HBRDG1		829
#define HBRDG2		830
#define HBRDG3		831
#define RADAR0		832
#define RADAR1		833
#define RADAR2		834
#define RADAR3		835
#define RADAR4		836
#define RADAR5		837
#define RADAR6		838
#define RADAR7		839
#define FOUNTAIN	840
#define INDBASE2	844
#define TELEBASE	844
#define TELELAST	851
#define SMOKEBASE	852
#define TINYEXP		860
#define SOMETINYEXP	864
#define LASTTINYEXP	867
#define COALSMOKE1	916
#define COALSMOKE2	920
#define COALSMOKE3	924
#define COALSMOKE4	928
#define FOOTBALLGAME1	932
#define FOOTBALLGAME2	940
#define VBRDG0		948
#define VBRDG1		949
#define VBRDG2		950
#define VBRDG3		951

#define TILE_COUNT	960

/* 
 * These describe the wand values, the object dragged around on the screen.
 */

#define residentialState 0
#define commercialState 1
#define industrialState 2
#define fireState 3
#define queryState 4
#define policeState 5
#define wireState 6
#define dozeState 7
#define rrState 8
#define roadState 9
#define UNUSED_STATE_1 10
#define UNUSED_STATE_2 11
#define stadiumState 12
#define parkState 13
#define seaportState  14
#define powerState 15
#define nuclearState 16
#define airportState 17
#define networkState 18


#define firstState residentialState
#define lastState networkState

#define STATE_CMD 0
#define STATE_TILES 1
#define STATE_OVERLAYS 2
#define STATE_GRAPHS 3

typedef unsigned char Byte;

extern int SpriteXOffset[OBJN];
extern int SpriteYOffset[OBJN];
extern int SMapX, SMapY;
extern int CChr, CChr9;
extern int RoadTotal, RailTotal, FirePop;
extern int ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
extern int ResZPop, ComZPop, IndZPop, TotalZPop;
extern int HospPop, ChurchPop, StadiumPop;
extern int PolicePop, FireStPop;
extern int CoalPop, NuclearPop, PortPop, APortPop;
extern int NeedHosp, NeedChurch;
extern int CrimeAverage, PolluteAverage, LVAverage;
extern const std::string MicropolisVersion;
extern std::string CityFileName;
extern std::string StartupName;
extern int StartingYear;
extern int CityTime;
extern int LastFunds;
extern int GameLevel;
extern int Cycle;
extern int ScenarioID;
extern int ShakeNow;
extern int DonDither;
extern int DoOverlay;

extern float roadPercent, policePercent, firePercent;
extern int RoadSpend, PoliceSpend, FireSpend;
extern int roadMaxValue, policeMaxValue, fireMaxValue;
extern int TaxFund, RoadFund, PoliceFund, FireFund;
extern int RoadEffect, PoliceEffect, FireEffect;
extern int TaxFlag, CityTax;
extern int TotalFunds;

extern int costOf[];

extern int flagBlink;
extern unsigned char tileSynch;
extern int aniTile[];
extern unsigned char aniSynch[];
extern int TilesAnimated;
extern int DoAnimation;
extern int DoMessages;
extern int DoNotices;
extern unsigned char ColorIntensities[];

extern int LastMesTime;

extern int NoDisasters;
extern int autoBulldoze;
extern int autoBudget;
extern bool AutoGo;

extern int DisasterEvent;
extern int DisasterWait;

extern int ResCap, ComCap, IndCap;
extern int RValve, CValve, IValve;
extern int PwrdZCnt;
extern int unPwrdZCnt;

extern int Graph10Max, Graph120Max;
extern int Res2HisMax, Com2HisMax, Ind2HisMax;
extern unsigned char *History10[HISTORIES], *History120[HISTORIES];
extern int CityScore;
extern int deltaCityScore;
extern int ScoreType;
extern int ScoreWait;
extern int CityClass;
extern int PolMaxX, PolMaxY;
extern int PowerStackNum;
extern int TrafficAverage;
extern int PosStackN;
extern int SMapXStack[], SMapYStack[];
extern int Zsource;
extern bool HaveLastMessage;
extern int PdestX, PdestY;
extern int CdestX, CdestY;
extern int absDist;
extern int CopFltCnt;
extern int GodCnt;
extern int GdestX, GdestY;
extern int GorgX, GorgY;
extern int GodControl;
extern int CopControl;
extern int CdestX, CdestY;
extern int TrafMaxX, TrafMaxY;
extern int CrimeMaxX, CrimeMaxY;
extern int FloodX, FloodY;
extern int CrashX, CrashY;
extern int CCx, CCy;
extern int CityPop, deltaCityPop;
extern int CityYes, CityNo;
extern int ProblemTable[PROBNUM];
extern int ProblemOrder[4];
extern int CityAssValue;

extern int InitSimLoad;
extern int DoInitialEval;
extern int Startup;
extern int StartupGameLevel;
extern int PerformanceTiming;
extern double FlushTime;


extern char *optarg;
extern int optind;
//extern struct timeval now_time;
extern int WireMode;
extern int MultiPlayerMode;
extern int sim_skips;
extern int sim_skip;

extern int heat_steps;
extern int heat_flow;
extern int heat_rule;
extern int UpdateDelayed;
extern int DynamicData[32];
extern int Players;
extern int Votes;
extern int BobHeight;
extern int OverRide;
extern int Expensive;
extern int PendingTool;
extern int PendingX;
extern int PendingY;
extern int TreeLevel;
extern int LakeLevel;
extern int CurveLevel;
extern int specialBase;
extern int PunishCnt;

extern int toolSize[];
extern int toolOffset[];

extern std::string Displays;
extern std::string FirstDisplay;

extern bool NewMap;
extern int NewMapFlags[NMAPS];
extern int NewGraph;
extern bool MustUpdateFunds;
extern int MustUpdateOptions;
extern int EvalChanged;
extern int MeltX, MeltY;
extern int NeedRest;

extern int FlushStyle;
extern int GotXError;

void sim_exit();
void sim_update_editors();
void drawValve();
