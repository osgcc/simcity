/* s_alloc.c
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
#include "w_stubs.h"


/* Allocate Stuff */


int SMapX, SMapY;
int CChr, CChr9;

int RoadTotal, RailTotal, FirePop;

int ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
int ResZPop, ComZPop, IndZPop, TotalZPop;
int HospPop, ChurchPop, StadiumPop;
int PolicePop, FireStPop;
int CoalPop, NuclearPop, PortPop, APortPop;

int NeedHosp, NeedChurch;
int CrimeAverage, PolluteAverage, LVAverage;

int CityTime;
int StartingYear;

int* Map[SimWidth];

int ResHisMax, Res2HisMax;
int ComHisMax, Com2HisMax;
int IndHisMax, Ind2HisMax;
int CensusChanged;

int MessagePort;
int MesX, MesY;

int RoadSpend, PoliceSpend, FireSpend;
int RoadFund, PoliceFund, FireFund;
int RoadEffect, PoliceEffect, FireEffect;
int TaxFund; 
int CityTax, TaxFlag;
unsigned char tileSynch = 0x01;

Byte* PopDensity[HalfWorldWidth];
Byte* TrfDensity[HalfWorldWidth];
Byte* PollutionMem[HalfWorldWidth];
Byte* LandValueMem[HalfWorldWidth];
Byte* CrimeMem[HalfWorldWidth];
Byte* tem[HalfWorldWidth];
Byte* tem2[HalfWorldWidth];
Byte* TerrainMem[QuarterWorldWidth];
Byte* Qtem[QuarterWorldWidth];

int RateOGMem[SmX][SmY];
int FireStMap[SmX][SmY];
int PoliceMap[SmX][SmY];
int PoliceMapEffect[SmX][SmY];

int FireRate[SmX][SmY];
int ComRate[SmX][SmY];
int STem[SmX][SmY];

Ptr terrainBase;
Ptr qTemBase;
Ptr tem1Base;
Ptr tem2Base;

Ptr popPtr;
Ptr trfPtr;
Ptr polPtr;
Ptr landPtr;
Ptr crimePtr;
	
Ptr auxPopPtr;
Ptr auxTrfPtr;
Ptr auxPolPtr;
Ptr auxLandPtr;
Ptr auxCrimePtr;
Ptr brettPtr;

unsigned int *mapPtr;
int *ResHis;
int *ComHis;
int *IndHis;
int *MoneyHis;
int *PollutionHis;
int *CrimeHis;
int *MiscHis;
int *PowerMap;


void initMapArrays()
{
    if (!mapPtr)
    {
        mapPtr = (unsigned int*)NewPtr(sizeof(unsigned int) * SimWidth * SimHeight);
    }

   auto auxPtr = mapPtr;

    for (int i = 0; i < SimWidth; i++)
    {
        auto temp1 = auxPtr + i * SimHeight;
        Map[i] = (int*)temp1;
    }

    popPtr = NewPtr(HalfWorldWidth * HalfWorldHeight);
    trfPtr = NewPtr(HalfWorldWidth * HalfWorldHeight);
    polPtr = NewPtr(HalfWorldWidth * HalfWorldHeight);
    landPtr = NewPtr(HalfWorldWidth * HalfWorldHeight);
    crimePtr = NewPtr(HalfWorldWidth * HalfWorldHeight);

    tem1Base = NewPtr(HalfWorldWidth * HalfWorldHeight);
    tem2Base = NewPtr(HalfWorldWidth * HalfWorldHeight);

    auxPopPtr = popPtr;
    auxTrfPtr = trfPtr;
    auxPolPtr = polPtr;
    auxLandPtr = landPtr;
    auxCrimePtr = crimePtr;

    for (int i = 0; i < HalfWorldWidth; i++)
    {
        PopDensity[i] = (Byte*)auxPopPtr + (i * HalfWorldHeight);
        TrfDensity[i] = (Byte*)auxTrfPtr + (i * HalfWorldHeight);
        PollutionMem[i] = (Byte*)auxPolPtr + (i * HalfWorldHeight);
        LandValueMem[i] = (Byte*)auxLandPtr + (i * HalfWorldHeight);
        CrimeMem[i] = (Byte*)auxCrimePtr + (i * HalfWorldHeight);

        tem[i] = (Byte*)tem1Base + (i * HalfWorldHeight);
        tem2[i] = (Byte*)tem2Base + (i * HalfWorldHeight);
    }

    brettPtr = (Ptr)&PopDensity[0][0];

    terrainBase = NewPtr(QuarterWorldWidth * QuarterWorldHeight);
    qTemBase = NewPtr(QuarterWorldWidth * QuarterWorldHeight);

    for (int i = 0; i < QuarterWorldWidth; i++)
    {
        TerrainMem[i] = (Byte*)terrainBase + (i * QuarterWorldHeight);
        Qtem[i] = (Byte*)qTemBase + (i * QuarterWorldHeight);
    }

    ResHis = (int*)NewPtr(HISTLEN);
    ComHis = (int*)NewPtr(HISTLEN);
    IndHis = (int*)NewPtr(HISTLEN);
    MoneyHis = (int*)NewPtr(HISTLEN);
    PollutionHis = (int*)NewPtr(HISTLEN);
    CrimeHis = (int*)NewPtr(HISTLEN);
    MiscHis = (int*)NewPtr(MISCHISTLEN);
    PowerMap = (int*)NewPtr(POWERMAPLEN);		/* power alloc */
}
