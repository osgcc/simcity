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
#include "s_sim.h"

#include "Budget.h"
#include "CityProperties.h"
#include "Evaluation.h"

#include "main.h"
#include "Map.h"

#include "Point.h"
#include "Power.h"

#include "s_alloc.h"
#include "s_disast.h"
#include "s_msg.h"

#include "w_util.h"

#include "Scan.h"
#include "Sprite.h"
#include "Traffic.h"
#include "Zone.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <iostream>


/* Simulation */

constexpr auto CensusRate = 4;
constexpr auto TaxFrequency = 48;

int CrimeRamp, PolluteRamp ;
int RValve, CValve, IValve;
int ResCap, ComCap, IndCap;
float EMarket = 4.0;
int DisasterEvent;
int DisasterWait;
int ScoreType;
int ScoreWait;
int PoweredZoneCount;
int UnpoweredZoneCount;
int AvCityTax;
int Scycle = 0;
int Fcycle = 0;
int DoInitialEval = 0;
int MeltX, MeltY;


void DoFire()
{
    static int DX[4] = { -1,  0,  1,  0 };
    static int DY[4] = { 0, -1,  0,  1 };

    for (int z = 0; z < 4; z++)
    {
        if (!(Rand16() & 7))
        {
            int Xtem = SimulationTarget.x + DX[z];
            int Ytem = SimulationTarget.y + DY[z];
            if (CoordinatesValid({ Xtem, Ytem }))
            {
                int c = Map[Xtem][Ytem];
                if (c & BURNBIT)
                {
                    if (c & ZONEBIT)
                    {
                        FireZone(Xtem, Ytem, c);
                        if ((c & LOMASK) > IZB) //  Explode
                        {
                            makeExplosionAt({ (Xtem * 16) + 8, (Ytem * 16) + 8 });
                        }
                    }
                    Map[Xtem][Ytem] = FIRE + RandomRange(0, 3) + ANIMBIT;
                }
            }
        }
    }
   
    int z = FireProtectionMap.value(SimulationTarget.skewInverseBy({ 8, 8 }));
    
    int Rate = 10;
    if (z)
    {
        Rate = 3;
        if (z > 20)
        {
            Rate = 2;
        }
        if (z > 100)
        {
            Rate = 1;
        }
    }
    if (!RandomRange(0, Rate))
    {
        Map[SimulationTarget.x][SimulationTarget.y] = RUBBLE + RandomRange(0, 3) + BULLBIT;
    }
}


void DoAirport()
{
    if (!(RandomRange(0, 5)))
    {
        generateAirplane(SimulationTarget);
        return;
    }
    if (!(RandomRange(0, 12)))
    {
        generateHelicopter(SimulationTarget);
    }
}


void DoMeltdown(const int x, const int y)
{
    generateExplosion({ x, y });

    for (int row = (x - 1); row < (x + 3); ++row)
    {
        for (int col = (y - 1); col < (y + 3); ++col)
        {
            Map[row][col] = FIRE + RandomRange(0, 3) | ANIMBIT;
        }
    }

    for (int i = 0; i < 200; ++i)
    {
        const int radiationX = x - 20 + RandomRange(0, 40);
        const int radiationY = y - 15 + RandomRange(0, 30);

        if (!CoordinatesValid({ radiationX, radiationY }))
        {
            continue;
        }

        const int tile = Map[radiationX][radiationY];

        if (tile & ZONEBIT)
        {
            continue;
        }

        if ((tile & BURNBIT) || tile == DIRT)
        {
            Map[radiationX][radiationY] = RADTILE;
        }
    }

    ClearMes();
    SendMesAt(NotificationId::NuclearMeltdownReported, x, y);
}


void DoRail(const Point<int>& position)
{
    RailTotal++;
    generateTrain(position);
   
    if (RoadEffect < 30) // Deteriorating  Rail
    {
        if (RandomRange(0, 511) == 0)
        {
            const unsigned int tile = tileValue(position.x, position.y);
            if (!(tile & CONDBIT))
            {
                if (RoadEffect < RandomRange(0, 31))
                {
                    if (maskedTileValue(tile) < (RAILBASE + 2))
                    {
                        Map[position.x][position.y] = RIVER;
                    }
                    else
                    {
                        Map[position.x][position.y] = RUBBLE + RandomRange(0, 3) + BULLBIT;
                    }
                    return;
                }
            }
        }
    }
}


void DoRadTile()
{
    if (RandomRange(0, 4095) == 0) // Radioactive decay
    {
        Map[SimulationTarget.x][SimulationTarget.y] = DIRT;
    }
}


int GetBoatDis()
{
    int dist = 99999;
    //int mx = (SimulationTarget.x * 16) + 8;
    //int my = (SimulationTarget.y * 16) + 8;

    /*
    for (SimSprite* sprite = sim->sprite; sprite != NULL; sprite = sprite->next)
    {
        if ((sprite->type == SHI) && (sprite->frame != 0))
        {
            if ((dx = sprite->x + sprite->x_hot - mx) < 0)
            {
                dx = -dx;
            }
            if ((dy = sprite->y + sprite->y_hot - my) < 0)
            {
                dy = -dy;
            }

            dx += dy;

            if (dx < dist)
            {
                dist = dx;
            }
        }
    }
    */

    return dist;
}


bool DoBridge()
{
  static int HDx[7] = { -2,  2, -2, -1,  0,  1,  2 };
  static int HDy[7] = { -1, -1,  0,  0,  0,  0,  0 };
  static int HBRTAB[7] = {
    HBRDG1 | BULLBIT, HBRDG3 | BULLBIT, HBRDG0 | BULLBIT,
    RIVER, BRWH | BULLBIT, RIVER, HBRDG2 | BULLBIT };
  static int HBRTAB2[7] = {
    RIVER, RIVER, HBRIDGE | BULLBIT, HBRIDGE | BULLBIT, HBRIDGE | BULLBIT,
    HBRIDGE | BULLBIT, HBRIDGE | BULLBIT };
  static int VDx[7] = {  0,  1,  0,  0,  0,  0,  1 };
  static int VDy[7] = { -2, -2, -1,  0,  1,  2,  2 };
  static int VBRTAB[7] = {
    VBRDG0 | BULLBIT, VBRDG1 | BULLBIT, RIVER, BRWV | BULLBIT,
    RIVER, VBRDG2 | BULLBIT, VBRDG3 | BULLBIT };
  static int VBRTAB2[7] = {
    VBRIDGE | BULLBIT, RIVER, VBRIDGE | BULLBIT, VBRIDGE | BULLBIT,
    VBRIDGE | BULLBIT, VBRIDGE | BULLBIT, RIVER };
  int z, x, y, MPtem;

  if (CurrentTileMasked == BRWV) { /*  Vertical bridge close */
    if ((!(Rand16() & 3)) &&
	(GetBoatDis() > 340))
      for (z = 0; z < 7; z++) { /* Close  */
	x = SimulationTarget.x + VDx[z];
	y = SimulationTarget.y + VDy[z];
	if (CoordinatesValid({ x, y }))
	  if ((Map[x][y] & LOMASK) == (VBRTAB[z] & LOMASK))
	    Map[x][y] = VBRTAB2[z];
      }
    return true;
  }
  if (CurrentTileMasked == BRWH) { /*  Horizontal bridge close  */
    if ((!(Rand16() & 3)) &&
	(GetBoatDis() > 340))
      for (z = 0; z < 7; z++) { /* Close  */
	x = SimulationTarget.x + HDx[z];
	y = SimulationTarget.y + HDy[z];
	if (CoordinatesValid({ x, y }))
	  if ((Map[x][y] & LOMASK) == (HBRTAB[z] & LOMASK))
	    Map[x][y] = HBRTAB2[z];
      }
    return true;
  }

  if ((GetBoatDis() < 300) || (!(Rand16() & 7))) {
    if (CurrentTileMasked & 1) {
      if (SimulationTarget.x < (SimWidth - 1))
	if (Map[SimulationTarget.x + 1][SimulationTarget.y] == CHANNEL) { /* Vertical open */
	  for (z = 0; z < 7; z++) {
	    x = SimulationTarget.x + VDx[z];
	    y = SimulationTarget.y + VDy[z];
	    if (CoordinatesValid({ x, y }))  {
	      MPtem = Map[x][y];
	      if ((MPtem == CHANNEL) ||
		  ((MPtem & 15) == (VBRTAB2[z] & 15)))
		Map[x][y] = VBRTAB[z];
	    }
	  }
	  return true;
	}
      return false;
    } else {
      if (SimulationTarget.y > 0)
	if (Map[SimulationTarget.x][SimulationTarget.y - 1] == CHANNEL) { /* Horizontal open  */
	  for (z = 0; z < 7; z++) {
	    x = SimulationTarget.x + HDx[z];
	    y = SimulationTarget.y + HDy[z];
	    if (CoordinatesValid({ x, y })) {
	      MPtem = Map[x][y];
	      if (((MPtem & 15) == (HBRTAB2[z] & 15)) ||
		  (MPtem == CHANNEL))
		Map[x][y] = HBRTAB[z];
	    }
	  }
	  return true;
	}
      return false;
    }
  }
  return false;
}


void DoRoad()
{
    static int DensityTable[3] =
    {
        ROADBASE,   // No Traffic
        LTRFBASE,   // Light Traffic
        HTRFBASE    // Heavy Traffic
    };

    RoadTotal++;

    if (RoadEffect < 30) // Deteriorating Roads
    {
        if (!(Rand16() & 511))
        {
            if (!(CurrentTile & CONDBIT))
            {
                if (RoadEffect < (Rand16() & 31))
                {
                    if (((CurrentTileMasked & 15) < 2) || ((CurrentTileMasked & 15) == 15))
                    {
                        Map[SimulationTarget.x][SimulationTarget.y] = RIVER;
                    }
                    else
                    {
                        Map[SimulationTarget.x][SimulationTarget.y] = RUBBLE + (Rand16() & 3) + BULLBIT;
                    }
                    return;
                }
            }
        }
    }

    if (!(CurrentTile & BURNBIT)) /* If Bridge */
    {
        RoadTotal += 4;
        if (DoBridge())
        {
            return;
        }
    }

    int trafficDensity{};

    if (CurrentTileMasked < LTRFBASE)
    {
        trafficDensity = 0;
    }
    else if (CurrentTileMasked < HTRFBASE)
    {
        trafficDensity = 1;
    }
    else
    {
        RoadTotal++;
        trafficDensity = 2;
    }

    int Density = TrafficDensityMap.value(SimulationTarget.skewInverseBy({ 2, 2 })) / 64;  // Set Traf Density
   
    if (Density > 2)
    {
        Density = 2;
    }

    if (trafficDensity != Density) /* tden 0..2   */
    {
        int z = ((CurrentTileMasked - ROADBASE) & 15) + DensityTable[Density];
        
        z += CurrentTile & (ALLBITS - ANIMBIT);
        
        if (Density)
        {
            z += ANIMBIT;
        }

        Map[SimulationTarget.x][SimulationTarget.y] = z;
    }
}


/* comefrom: DoSPZone spawnHospital */
void RepairZone(int ZCent, int zsize)
{
  int cnt;
  int x, y, ThCh;

  zsize--;
  cnt = 0;
  for (y = -1; y < zsize; y++)
    for (x = -1; x < zsize; x++) {
      int xx = SimulationTarget.x + x;
      int yy = SimulationTarget.y + y;
      cnt++;
      if (CoordinatesValid({ xx, yy })) {
	ThCh = Map[xx][yy];
	if (ThCh & ZONEBIT) continue;
	if (ThCh & ANIMBIT) continue;
	ThCh = ThCh & LOMASK;
	if ((ThCh < RUBBLE) || (ThCh >= ROADBASE)) {
	  Map[xx][yy] = ZCent - 3 - zsize + cnt + CONDBIT + BURNBIT;
	}
      }
    }
}


/* comefrom: DoSPZone */
void DrawStadium(int z)
{
    z = z - 5;
    for (int y = (SimulationTarget.y - 1); y < (SimulationTarget.y + 3); y++)
    {
        for (int x = (SimulationTarget.x - 1); x < (SimulationTarget.x + 3); x++)
        {
            Map[x][y] = (z++) | BNCNBIT;
        }
    }
 
    Map[SimulationTarget.x][SimulationTarget.y] |= ZONEBIT | PWRBIT;
}


void CoalSmoke(int mx, int my)
{
    static int SmTb[4] = { COALSMOKE1, COALSMOKE2, COALSMOKE3, COALSMOKE4 };
    static int dx[4] = { 1,  2,  1,  2 };
    static int dy[4] = { -1, -1,  0,  0 };
    int x;

    for (x = 0; x < 4; x++)
    {
        Map[mx + dx[x]][my + dy[x]] = SmTb[x] | ANIMBIT | CONDBIT | PWRBIT | BURNBIT;
    }
}


/*
 * fixme: Break this into smaller chunks
 */
void DoSPZone(bool powered, const CityProperties& properties)
{
    static int MltdwnTab[3] = { 30000, 20000, 10000 };  /* simadj */
    int z;

    switch (CurrentTileMasked)
    {
    case POWERPLANT:
        CoalPop++;
        if (!(CityTime & 7)) /* post */
        {
            RepairZone(POWERPLANT, 4);
        }
        pushPowerStack(SimulationTarget);
        CoalSmoke(SimulationTarget.x, SimulationTarget.y);
        return;

    case NUCLEAR:
        if (!NoDisasters && !RandomRange(0, MltdwnTab[properties.GameLevel()]))
        {
            DoMeltdown(SimulationTarget.x, SimulationTarget.y);
            return;
        }
        NuclearPop++;
        if (!(CityTime & 7)) /* post */
        {
            RepairZone(NUCLEAR, 4);
        }
        pushPowerStack(SimulationTarget);
        return;

    case FIRESTATION:
        FireStPop++;
        if (!(CityTime & 7)) /* post */
        {
            RepairZone(FIRESTATION, 3);
        }

        if (powered) /* if powered get effect  */
        {
            z = FireEffect;
        }
        else /* from the funding ratio  */
        {
            z = FireEffect / 2;
        }

        if (!roadOnZonePerimeter()) /* post FD's need roads  */
        {
            z = z / 2;
        }

        {
            const auto fstVal = FireStationMap.value({ SimulationTarget.x >> 3, SimulationTarget.y >> 3 });
            FireStationMap.value({ SimulationTarget.x >> 3, SimulationTarget.y >> 3 }) = fstVal + z;
        }
        return;

    case POLICESTATION:
        PolicePop++;
        if (!(CityTime & 7))
        {
            RepairZone(POLICESTATION, 3); /* post */
        }

        if (powered)
        {
            z = PoliceEffect;
        }
        else
        {
            z = PoliceEffect / 2;
        }

        if (!roadOnZonePerimeter())
        {
            z = z / 2; /* post PD's need roads */
        }

        {
            const auto pstVal = PoliceStationMap.value({ SimulationTarget.x >> 3, SimulationTarget.y >> 3 });
            PoliceStationMap.value({ SimulationTarget.x >> 3, SimulationTarget.y >> 3 }) = pstVal + z;
        }
        return;

    case STADIUM:
        StadiumPop++;
        if (!(CityTime & 15))
        {
            RepairZone(STADIUM, 4);
        }
        if (powered)
        {
            if (!((CityTime + SimulationTarget.x + SimulationTarget.y) & 31)) // post release
            {
                DrawStadium(FULLSTADIUM);
                Map[SimulationTarget.x + 1][SimulationTarget.y] = FOOTBALLGAME1 + ANIMBIT;
                Map[SimulationTarget.x + 1][SimulationTarget.y + 1] = FOOTBALLGAME2 + ANIMBIT;
            }
        }
        return;

    case FULLSTADIUM:
        StadiumPop++;
        if (!((CityTime + SimulationTarget.x + SimulationTarget.y) & 7))	/* post release */
        {
            DrawStadium(STADIUM);
        }
        return;

    case AIRPORT:
        APortPop++;
        
        if (!(CityTime & 7))
        {
            RepairZone(AIRPORT, 6);
        }

        if (powered) // post
        { 
            if ((Map[SimulationTarget.x + 1][SimulationTarget.y - 1] & LOMASK) == RADAR)
            {
                Map[SimulationTarget.x + 1][SimulationTarget.y - 1] = RADAR + ANIMBIT + CONDBIT + BURNBIT;
            }
        }
        else
        {
            Map[SimulationTarget.x + 1][SimulationTarget.y - 1] = RADAR + CONDBIT + BURNBIT;
        }

        if (powered)
        {
            DoAirport();
        }
        return;

    case PORT:
        PortPop++;
        if ((CityTime & 15) == 0)
        {
            RepairZone(PORT, 4);
        }

        SimSprite* shipSprite = getSprite(SimSprite::Type::Ship);
        if ((powered && shipSprite == nullptr) || (shipSprite != nullptr && !shipSprite->active))
        {
            generateShip();
        }
        return;
    }
}


/* comefrom: Simulate DoSimInit */
void MapScan(int x1, int x2, const CityProperties& properties)
{
    for (int x = x1; x < x2; x++)
    {
        for (int y = 0; y < SimHeight; y++)
        {
            CurrentTile = Map[x][y];
            if (CurrentTile != 0)
            {
                CurrentTileMasked = CurrentTile & LOMASK;	// Mask off status bits

                const int tile = maskedTileValue(x, y);

                if (CurrentTileMasked >= FLOOD)
                {
                    SimulationTarget = { x, y };

                    if (CurrentTileMasked < ROADBASE)
                    {
                        if (CurrentTileMasked >= FIREBASE)
                        {
                            FirePop++;
                            if (!(Rand16() & 3)) // 1 in 4 times
                            {
                                DoFire();
                            }
                            continue;
                        }
                        if (CurrentTileMasked < RADTILE)
                        {
                            DoFlood();
                        }
                        else
                        {
                            DoRadTile();
                        }
                        continue;
                    }

                    if (CurrentTile & CONDBIT)
                    {
                        setZonePower({ x, y });
                    }

                    if ((CurrentTileMasked >= ROADBASE) && (CurrentTileMasked < POWERBASE))
                    {
                        DoRoad();
                        continue;
                    }

                    if (CurrentTile & ZONEBIT) // process Zones
                    {
                        updateZone({ x, y }, properties);
                        continue;
                    }

                    if ((tile >= RAILBASE) && (tile < ResidentialBase))
                    {
                        DoRail({ x, y });
                        continue;
                    }
                    if ((CurrentTileMasked >= SOMETINYEXP) && (CurrentTileMasked <= LASTTINYEXP)) // clear AniRubble
                    {
                        Map[x][y] = RUBBLE + (Rand16() & 3) + BULLBIT;
                    }
                }
            }
        }
    }
}


void SetValves(const CityProperties& properties, const Budget& budget)
{
    static int TaxTable[21] = {
      200, 150, 120, 100, 80, 50, 30, 0, -10, -40, -100,
      -150, -200, -250, -300, -350, -400, -450, -500, -550, -600 };

    float Employment, Migration, Births, LaborBase, IntMarket;
    float Rratio, Cratio, Iratio, temp;
    float NormResPop, PjResPop, PjComPop, PjIndPop;

    MiscHis[1] = static_cast<int>(EMarket);
    MiscHis[2] = ResPop;
    MiscHis[3] = ComPop;
    MiscHis[4] = IndPop;
    MiscHis[5] = RValve;
    MiscHis[6] = CValve;
    MiscHis[7] = IValve;
    MiscHis[10] = CrimeRamp;
    MiscHis[11] = PolluteRamp;
    MiscHis[12] = LVAverage;
    MiscHis[13] = CrimeAverage;
    MiscHis[14] = PolluteAverage;
    MiscHis[15] = properties.GameLevel();
    MiscHis[16] = static_cast<int>(cityClass());
    MiscHis[17] = cityScore();

    NormResPop = static_cast<float>(ResPop / 8);
    LastTotalPop = TotalPop;
    TotalPop = static_cast<int>(NormResPop) + ComPop + IndPop;

    if (NormResPop) Employment = ((ComHis[1] + IndHis[1]) / NormResPop);
    else Employment = 1;

    Migration = NormResPop * (Employment - 1);
    Births = NormResPop * 0.02f; 			/* Birth Rate  */
    PjResPop = NormResPop + Migration + Births;	/* Projected Res.Pop  */

    if (float result = static_cast<float>(ComHis[1] + IndHis[1]))
    {
        LaborBase = (ResHis[1] / result);
    }
    else
    {
        LaborBase = 1.0f;
    }

    if (LaborBase > 1.3f)
    {
        LaborBase = 1.3f;
    }
    if (LaborBase < 0.0f)
    {
        LaborBase = 0.0f;  /* LB > 1 - .1  */
    }

    // Point of this? It adds this all up then just ignores the result?
    for (int z = 0; z < 2; z++)
    {
        temp = static_cast<float>(ResHis[z] + ComHis[z] + IndHis[z]);
    }

    IntMarket = (NormResPop + ComPop + IndPop) / 3.7f;

    PjComPop = IntMarket * LaborBase;

    temp = 1;
    switch (properties.GameLevel())
    {
    case 0:
        temp = 1.2f;
        break;

    case 1:
        temp = 1.1f;
        break;

    case 2:
        temp = 0.98f;
        break;
    }

    PjIndPop = IndPop * LaborBase * temp;
    if (PjIndPop < 5)
    {
        PjIndPop = 5;
    }

    if (NormResPop)  /* projected -vs- actual */
    {
        Rratio = (PjResPop / NormResPop);
    }
    else
    {
        Rratio = 1.3f;
    }
    if (ComPop)
    {
        Cratio = (PjComPop / ComPop);
    }
    else
    {
        Cratio = PjComPop;
    }
    if (IndPop)
    {
        Iratio = (PjIndPop / IndPop);
    }
    else
    {
        Iratio = PjIndPop;
    }

    if (Rratio > 2) { Rratio = 2; }
    if (Cratio > 2) { Cratio = 2; }
    if (Iratio > 2) { Iratio = 2; }

    int index = std::clamp(budget.TaxRate() + properties.GameLevel(), 0, 20);

    Rratio = ((Rratio - 1) * 600) + TaxTable[index]; /* global tax/Glevel effects */
    Cratio = ((Cratio - 1) * 600) + TaxTable[index];
    Iratio = ((Iratio - 1) * 600) + TaxTable[index];

    if (Rratio > 0)		/* ratios are velocity changes to valves  */
    {
        if (RValve < 2000)
        {
            RValve += static_cast<int>(Rratio);
        }
    }
    if (Rratio < 0)
    {
        if (RValve > -2000)
        {
            RValve += static_cast<int>(Rratio);
        }
    }
    if (Cratio > 0)
    {
        if (CValve < 1500)
        {
            CValve += static_cast<int>(Cratio);
        }
    }
    if (Cratio < 0)
    {
        if (CValve > -1500)
        {
            CValve += static_cast<int>(Cratio);
        }
    }
    if (Iratio > 0)
    {
        if (IValve < 1500)
        {
            IValve += static_cast<int>(Iratio);
        }
    }
    if (Iratio < 0)
    {
        if (IValve > -1500)
        {
            IValve += static_cast<int>(Iratio);
        }
    }

    RValve = std::clamp(RValve, -1500, 1500);
    CValve = std::clamp(CValve, -1500, 1500);
    IValve = std::clamp(IValve, -1500, 1500);

    if ((ResCap) && (RValve > 0)) // Stad, Prt, Airprt
    {
        RValve = 0;
    }
    if ((ComCap) && (CValve > 0))
    {
        CValve = 0;
    }
    if ((IndCap) && (IValve > 0))
    {
        IValve = 0;
    }
}


void ClearCensus()
{
    PoweredZoneCount = 0;
    UnpoweredZoneCount = 0;
    FirePop = 0;
    RoadTotal = 0;
    RailTotal = 0;
    ResPop = 0;
    ComPop = 0;
    IndPop = 0;
    ResZPop = 0;
    ComZPop = 0;
    IndZPop = 0;
    HospPop = 0;
    ChurchPop = 0;
    PolicePop = 0;
    FireStPop = 0;
    StadiumPop = 0;
    CoalPop = 0;
    NuclearPop = 0;
    PortPop = 0;
    APortPop = 0;
    resetPowerStack(); // Reset before Mapscan

    FireStationMap.reset();
    PoliceStationMap.reset();
}


void TakeCensus(Budget& budget)
{
    /* put census#s in Historical Graphs and scroll data  */
    std::rotate(ResHis.rbegin(), ResHis.rbegin() + 1, ResHis.rend());
    std::rotate(ComHis.rbegin(), ComHis.rbegin() + 1, ComHis.rend());
    std::rotate(IndHis.rbegin(), IndHis.rbegin() + 1, IndHis.rend());
    std::rotate(CrimeHis.rbegin(), CrimeHis.rbegin() + 1, CrimeHis.rend());
    std::rotate(PollutionHis.rbegin(), PollutionHis.rbegin() + 1, PollutionHis.rend());
    std::rotate(MoneyHis.rbegin(), MoneyHis.rbegin() + 1, MoneyHis.rend());

    ResHisMax = *std::max_element(ResHis.begin(), ResHis.end());
    ComHisMax = *std::max_element(ComHis.begin(), ComHis.end());
    IndHisMax = *std::max_element(IndHis.begin(), IndHis.end());

    ResHis[0] = ResPop / 8; // magic number
    ComHis[0] = ComPop;
    IndHis[0] = IndPop;

    CrimeRamp += (CrimeAverage - CrimeRamp) / 4; // magic number
    CrimeHis[0] = CrimeRamp;

    PolluteRamp += (PolluteAverage - PolluteRamp) / 4; // magic number
    PollutionHis[0] = PolluteRamp;

    MoneyHis[0] = std::clamp((budget.CashFlow() / 20) + 128, 0, 255); // scale to 0..255
    CrimeHis[0] = std::clamp(CrimeHis[0], 0, 255);
    PollutionHis[0] = std::clamp(PollutionHis[0], 0, 255);

    if (HospPop < (ResPop / 256))
    {
        NeedHosp = 1;
    }
    if (HospPop > (ResPop / 256))
    {
        NeedHosp = -1;
    }
    if (HospPop == (ResPop / 256))
    {
        NeedHosp = 0;
    }

    if (ChurchPop < (ResPop / 256))
    {
        NeedChurch = 1;
    }
    if (ChurchPop > (ResPop / 256))
    {
        NeedChurch = -1;
    }
    if (ChurchPop == (ResPop / 256))
    {
        NeedChurch = 0;
    }
}


// Long Term Graphs
void Take2Census()
{
    std::rotate(ResHis120Years.rbegin(), ResHis120Years.rbegin() + 1, ResHis120Years.rend());
    std::rotate(ComHis120Years.rbegin(), ComHis120Years.rbegin() + 1, ComHis120Years.rend());
    std::rotate(IndHis120Years.rbegin(), IndHis120Years.rbegin() + 1, IndHis120Years.rend());
    std::rotate(CrimeHis120Years.rbegin(), CrimeHis120Years.rbegin() + 1, CrimeHis120Years.rend());
    std::rotate(PollutionHis120Years.rbegin(), PollutionHis120Years.rbegin() + 1, PollutionHis120Years.rend());
    std::rotate(MoneyHis120Years.rbegin(), MoneyHis120Years.rbegin() + 1, MoneyHis120Years.rend());

    ResHis120Years[0] = ResPop / 8; // magic number
    ComHis120Years[0] = ComPop;
    IndHis120Years[0] = IndPop;

    CrimeHis120Years[0] = CrimeHis[0];
    PollutionHis120Years[0] = PollutionHis[0];
    MoneyHis120Years[0] = MoneyHis[0];
}


void CollectTax(const CityProperties& properties, Budget& budget)
{
    static float RLevels[3] = { 0.7f, 0.9f, 1.2f };
    static float FLevels[3] = { 1.4f, 1.2f, 0.8f };

    // XXX: do something with z
    //int z = AvCityTax / 48;  // post
    AvCityTax = 0;
    
    budget.PoliceFundsNeeded(PolicePop * 100);
    budget.FireFundsNeeded(FireStPop * 100);
    budget.RoadFundsNeeded(static_cast<int>((RoadTotal + (RailTotal * 2)) * RLevels[properties.GameLevel()]));

    budget.TaxIncome(static_cast<int>(((static_cast<float>(TotalPop) * LVAverage) / 120.0f) * budget.TaxRate() * FLevels[properties.GameLevel()])); //yuck

    if (TotalPop) // if there are people to tax
    {
        budget.update();
    }
    else
    {
        RoadEffect = 32;
        PoliceEffect = 1000;
        FireEffect = 1000;
    }
}


// tends to empty RateOfGrowthMap
// ROG == Rate Of Growth
void DecROGMem()
{
    for (int x = 0; x < EighthWorldWidth; x++)
    {
        for (int y = 0; y < EighthWorldHeight; y++)
        {
            int z = RateOfGrowthMap.value({ x, y });
            if (z == 0)
            {
                continue;
            }
            if (z > 0)
            {
                const auto rogVal = RateOfGrowthMap.value({ x, y });
                RateOfGrowthMap.value({ x, y }) = rogVal - 1;
                if (z > 200) // prevent overflow
                {
                    RateOfGrowthMap.value({ x, y }) = std::min(z, 200);
                }
                continue;
            }
            if (z < 0)
            {
                const auto rogVal = RateOfGrowthMap.value({ x, y });
                RateOfGrowthMap.value({ x, y }) = rogVal + 1;
                if (z < -200)
                {
                    RateOfGrowthMap.value({ x, y }) = -200;
                }
            }
        }
    }
}


/* comefrom: InitSimMemory SimLoadInit */
void SetCommonInits()
{
    EvalInit();
    RoadEffect = 32;
    PoliceEffect = 1000;
    FireEffect = 1000;
}


void InitSimMemory()
{
    SetCommonInits();

    ResHis.fill(0);
    ComHis.fill(0);
    IndHis.fill(0);
    MoneyHis.fill(128); // magic number
    CrimeHis.fill(0);
    PollutionHis.fill(0);

    CrimeRamp = 0;
    PolluteRamp = 0;
    TotalPop = 0;
    RValve = 0;
    CValve = 0;
    IValve = 0;
    ResCap = 0;
    ComCap = 0;
    IndCap = 0;

    EMarket = 6.0;
    DisasterEvent = 0;
    ScoreType = 0;

    resetPowerStack();
    powerScan();

    InitSimLoad = 0;
}


void DoNilPower()
{
    for (int x = 0; x < SimWidth; x++)
    {
        for (int y = 0; y < SimHeight; y++)
        {
            int z = Map[x][y];
            if (z & ZONEBIT)
            {
                SimulationTarget = { x, y };
                setZonePower({ x, y });
            }
        }
    }
}


/* tends to empty TrafficDensityMap   */
void DecTrafficMem()
{   
    for (int x = 0; x < HalfWorldWidth; x++)
    {
        for (int y = 0; y < HalfWorldHeight; y++)
        {
            int z = TrafficDensityMap.value({ x, y });
            if (z != 0)
            {
                if (z > 24)
                {
                    if (z > 200)
                    {
                        TrafficDensityMap.value({ x, y }) = z - 34;
                    }
                    else
                    {
                        TrafficDensityMap.value({ x, y }) = z - 24;
                    }
                }
                else TrafficDensityMap.value({ x, y }) = 0;
            }
        }
    }
}


/* comefrom: DoSimInit */
void SimLoadInit(CityProperties& properties)
{
    static int DisTab[9] = { 0, 2, 10, 5, 20, 3, 5, 5, 2 * 48 };
    static int ScoreWaitTab[9] = { 0, 30 * 48, 5 * 48, 5 * 48, 10 * 48,
                     5 * 48, 10 * 48, 5 * 48, 10 * 48 };

    EMarket = (float)MiscHis[1];
    ResPop = MiscHis[2];
    ComPop = MiscHis[3];
    IndPop = MiscHis[4];
    RValve = MiscHis[5];
    CValve = MiscHis[6];
    IValve = MiscHis[7];
    CrimeRamp = MiscHis[10];
    PolluteRamp = MiscHis[11];
    LVAverage = MiscHis[12];
    CrimeAverage = MiscHis[13];
    PolluteAverage = MiscHis[14];
    properties.GameLevel(MiscHis[15]);

    if (CityTime < 0)
    {
        CityTime = 0;
    }

    if (!EMarket)
    {
        EMarket = 4.0;
    }

    SetCommonInits();

    cityClass(static_cast<CityClass>(MiscHis[16]));
    cityScore(MiscHis[17]);

    if ((cityClass() > CityClass::Megalopolis) || (cityClass() < CityClass::Village))
    {
        cityClass(CityClass::Village);
    }
    if ((cityScore() > 999) || (cityScore() < 1))
    {
        cityScore(500);
    }

    ResCap = 0;
    ComCap = 0;
    IndCap = 0;

    AvCityTax = (CityTime % 48) * 7; /* post */

    resetPowerMap();

    DoNilPower();

    if (ScenarioID > 8)
    {
        ScenarioID = 0;
    }

    if (ScenarioID)
    {
        DisasterEvent = ScenarioID;
        DisasterWait = DisTab[DisasterEvent];
        ScoreType = DisasterEvent;
        ScoreWait = ScoreWaitTab[DisasterEvent];
    }
    else
    {
        DisasterEvent = 0;
        ScoreType = 0;
    }

    RoadEffect = 32;
    PoliceEffect = 1000; /*post*/
    FireEffect = 1000;
    InitSimLoad = 0;
}


namespace
{
    int PowerScanFrequency[5] = { 1,  2,  4,  5, 6 };
    int PollutionScanFrequency[5] = { 1,  2,  7, 17, 27 };
    int CrimeScanFrequency[5] = { 1,  1,  8, 18, 28 };
    int PopulationDensityScanFrequency[5] = { 1,  1,  9, 19, 29 };
    int FireAnalysisFrequency[5] = { 1,  1, 10, 20, 30 };
};


void Simulate(int mod16, CityProperties& properties, Budget& budget)
{
    int speed = static_cast<int>(SimSpeed()); // ew, find a better way to do this

    switch (mod16)
    {
    case 0:
        ++Scycle > 1023 ? Scycle = 0 : Scycle;
        
        if (DoInitialEval)
        {
            DoInitialEval = 0;
            CityEvaluation(budget);
        }
        
        CityTime++;
        AvCityTax += budget.TaxRate(); // post <-- ?
        
        if (!(Scycle % 2))
        {
            SetValves(properties, budget);
        }
        
        ClearCensus();
        break;

    case 1:
        MapScan(0, 1 * SimWidth / 8, properties);
        break;

    case 2:
        MapScan(1 * SimWidth / 8, 2 * SimWidth / 8, properties);
        break;

    case 3:
        MapScan(2 * SimWidth / 8, 3 * SimWidth / 8, properties);
        break;

    case 4:
        MapScan(3 * SimWidth / 8, 4 * SimWidth / 8, properties);
        break;

    case 5:
        MapScan(4 * SimWidth / 8, 5 * SimWidth / 8, properties);
        break;

    case 6:
        MapScan(5 * SimWidth / 8, 6 * SimWidth / 8, properties);
        break;

    case 7:
        MapScan(6 * SimWidth / 8, 7 * SimWidth / 8, properties);
        break;

    case 8:
        MapScan(7 * SimWidth / 8, SimWidth, properties);
        break;

    case 9:
        if (!(CityTime % CensusRate))
        {
            TakeCensus(budget);
        }
        if (!(CityTime % (CensusRate * 12)))
        {
            Take2Census();
        }

        if (!(CityTime % TaxFrequency))
        {
            CollectTax(properties, budget);
            CityEvaluation(budget);
        }
        break;

    case 10:
        if (!(Scycle % 5))
        {
            DecROGMem();
        }
        DecTrafficMem();
        SendMessages(budget);
        break;

    case 11:
        if (!(Scycle % PowerScanFrequency[speed]))
        {
            powerScan();
        }
        break;

    case 12:
        if (!(Scycle % PollutionScanFrequency[speed]))
        {
            pollutionAndLandValueScan();
        }
        break;

    case 13:
        if (!(Scycle % CrimeScanFrequency[speed]))
        {
            crimeScan();
        }
        break;

    case 14:
        if (!(Scycle % PopulationDensityScanFrequency[speed]))
        {
            scanPopulationDensity();
        }
        break;

    case 15:
        if (!(Scycle % FireAnalysisFrequency[speed]))
        {
            fireAnalysis();
        }
        DoDisasters(properties);
        break;
    }
}


void SimFrame(CityProperties& properties, Budget& budget)
{
    if (SimSpeed() == SimulationSpeed::Paused)
    {
        return;
    }

    if (++Fcycle > 1024)
    {
        Fcycle = 0;
    }

    Simulate(Fcycle % 16, properties, budget);
}


void DoSimInit(CityProperties& properties, Budget& budget)
{
    Fcycle = 0;
    Scycle = 0;

    if (InitSimLoad == 2) 			/* if new city    */
    {
        InitSimMemory();
    }

    if (InitSimLoad == 1)  			/* if city just loaded  */
    {
        SimLoadInit(properties);
    }

    SetValves(properties, budget);
    ClearCensus();
    MapScan(0, SimWidth, properties); /* XXX are you sure ??? */
    powerScan();
    pollutionAndLandValueScan();
    crimeScan();
    scanPopulationDensity();
    fireAnalysis();
    newMap(true);
    TotalPop = 1;
    DoInitialEval = 1;
}


void UpdateFundEffects(const Budget& budget)
{
    if (budget.RoadFundsNeeded())
    {
        RoadEffect = (int)(((float)budget.RoadFundsGranted() / (float)budget.RoadFundsNeeded()) * 32.0);
    }
    else
    {
        RoadEffect = 32;
    }

    if (budget.PoliceFundsNeeded())
    {
        PoliceEffect = (int)(((float)budget.PoliceFundsGranted() / (float)budget.PoliceFundsNeeded()) * 1000.0);
    }
    else
    {
        PoliceEffect = 1000;
    }

    if (budget.FireFundsNeeded())
    {
        FireEffect = (int)(((float)budget.FireFundsGranted() / (float)budget.FireFundsNeeded()) * 1000.0);
    }
    else
    {
        FireEffect = 1000;
    }
}


void FireZone(int Xloc, int Yloc, int ch)
{
    int Xtem, Ytem;
    int XYmax;

    const auto rogVal = RateOfGrowthMap.value({ Xloc / 8, Yloc / 8 });
    RateOfGrowthMap.value({ Xloc / 8, Yloc / 8 }) = rogVal - 20;

    ch = ch & LOMASK;
    if (ch < PORTBASE)
    {
        XYmax = 2;
    }
    else
    {
        if (ch == AIRPORT)
        {
            XYmax = 5;
        }
        else
        {
            XYmax = 4;
        }
    }

    for (int x = -1; x < XYmax; x++)
    {
        for (int y = -1; y < XYmax; y++)
        {
            Xtem = Xloc + x;
            Ytem = Yloc + y;
            if ((Xtem < 0) || (Xtem > (SimWidth - 1)) || (Ytem < 0) || (Ytem > (SimHeight - 1)))
            {
                continue;
            }

            if ((int)(Map[Xtem][Ytem] & LOMASK) >= ROADBASE) // post release
            {
                Map[Xtem][Ytem] |= BULLBIT;
            }
        }
    }
}
