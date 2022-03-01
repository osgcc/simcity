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

#include "main.h"
#include "Map.h"

#include "s_alloc.h"
#include "s_disast.h"
#include "s_eval.h"
#include "s_msg.h"
#include "s_power.h"
#include "s_scan.h"
#include "s_traf.h"
#include "s_zone.h"

#include "w_budget.h"
#include "w_graph.h"
#include "w_stubs.h"
#include "w_util.h"

#include "Sprite.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <random>


/* Simulation */

int CrimeRamp, PolluteRamp ;
int RValve, CValve, IValve;
int ResCap, ComCap, IndCap;
int CashFlow;
float EMarket = 4.0;
int DisasterEvent;
int DisasterWait;
int ScoreType;
int ScoreWait;
int PwrdZCnt;
int unPwrdZCnt;
int NewPower; // post  <-- ?
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
            int Xtem = SMapX + DX[z];
            int Ytem = SMapY + DY[z];
            if (CoordinatesValid(Xtem, Ytem, SimWidth, SimHeight))
            {
                int c = Map[Xtem][Ytem];
                if (c & BURNBIT)
                {
                    if (c & ZONEBIT)
                    {
                        FireZone(Xtem, Ytem, c);
                        if ((c & LOMASK) > IZB) //  Explode
                        {
                            MakeExplosionAt((Xtem << 4) + 8, (Ytem << 4) + 8);
                        }
                    }
                    Map[Xtem][Ytem] = FIRE + (Rand16() & 3) + ANIMBIT;
                }
            }
        }
    }
   
    int z = FireRate[SMapX >> 3][SMapY >> 3];
    
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
        Map[SMapX][SMapY] = RUBBLE + (Rand16() & 3) + BULLBIT;
    }
}


void DoAirport()
{
    if (!(RandomRange(0, 5)))
    {
        GeneratePlane(SMapX, SMapY);
        return;
    }
    if (!(RandomRange(0, 12)))
    {
        GenerateCopter(SMapX, SMapY);
    }
}


void DoMeltdown(const int x, const int y)
{
    MakeExplosion(x, y);

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

        if(!CoordinatesValid(radiationX, radiationY, SimWidth, SimHeight))
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


void DoRail()
{
    RailTotal++;
    GenerateTrain(SMapX, SMapY);
   
    if (RoadEffect < 30) // Deteriorating  Rail
    {
        if (RandomRange(0, 511) == 0)
        {
            if (!(CChr & CONDBIT))
            {
                if (RoadEffect < RandomRange(0, 31))
                {
                    if (CChr9 < (RAILBASE + 2))
                    {
                        Map[SMapX][SMapY] = RIVER;
                    }
                    else
                    {
                        Map[SMapX][SMapY] = RUBBLE + RandomRange(0, 3) + BULLBIT;
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
        Map[SMapX][SMapY] = DIRT;
    }
}


int GetBoatDis()
{
    int dist = 99999;
    int mx = (SMapX * 16) + 8;
    int my = (SMapY * 16) + 8;

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

  if (CChr9 == BRWV) { /*  Vertical bridge close */
    if ((!(Rand16() & 3)) &&
	(GetBoatDis() > 340))
      for (z = 0; z < 7; z++) { /* Close  */
	x = SMapX + VDx[z];
	y = SMapY + VDy[z];
	if (CoordinatesValid(x, y, SimWidth, SimHeight))
	  if ((Map[x][y] & LOMASK) == (VBRTAB[z] & LOMASK))
	    Map[x][y] = VBRTAB2[z];
      }
    return true;
  }
  if (CChr9 == BRWH) { /*  Horizontal bridge close  */
    if ((!(Rand16() & 3)) &&
	(GetBoatDis() > 340))
      for (z = 0; z < 7; z++) { /* Close  */
	x = SMapX + HDx[z];
	y = SMapY + HDy[z];
	if (CoordinatesValid(x, y, SimWidth, SimHeight))
	  if ((Map[x][y] & LOMASK) == (HBRTAB[z] & LOMASK))
	    Map[x][y] = HBRTAB2[z];
      }
    return true;
  }

  if ((GetBoatDis() < 300) || (!(Rand16() & 7))) {
    if (CChr9 & 1) {
      if (SMapX < (SimWidth - 1))
	if (Map[SMapX + 1][SMapY] == CHANNEL) { /* Vertical open */
	  for (z = 0; z < 7; z++) {
	    x = SMapX + VDx[z];
	    y = SMapY + VDy[z];
	    if (CoordinatesValid(x, y, SimWidth, SimHeight))  {
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
      if (SMapY > 0)
	if (Map[SMapX][SMapY - 1] == CHANNEL) { /* Horizontal open  */
	  for (z = 0; z < 7; z++) {
	    x = SMapX + HDx[z];
	    y = SMapY + HDy[z];
	    if (CoordinatesValid(x, y, SimWidth, SimHeight)) {
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
    // GenerateBus(SMapX, SMapY);

    if (RoadEffect < 30) // Deteriorating Roads
    {
        if (!(Rand16() & 511))
        {
            if (!(CChr & CONDBIT))
            {
                if (RoadEffect < (Rand16() & 31))
                {
                    if (((CChr9 & 15) < 2) || ((CChr9 & 15) == 15))
                    {
                        Map[SMapX][SMapY] = RIVER;
                    }
                    else
                    {
                        Map[SMapX][SMapY] = RUBBLE + (Rand16() & 3) + BULLBIT;
                    }
                    return;
                }
            }
        }
    }

    if (!(CChr & BURNBIT)) /* If Bridge */
    {
        RoadTotal += 4;
        if (DoBridge())
        {
            return;
        }
    }

    int trafficDensity{};

    if (CChr9 < LTRFBASE)
    {
        trafficDensity = 0;
    }
    else if (CChr9 < HTRFBASE)
    {
        trafficDensity = 1;
    }
    else
    {
        RoadTotal++;
        trafficDensity = 2;
    }

    int Density = TrfDensity[SMapX / 2][SMapY / 2] / 64;  // Set Traf Density
   
    if (Density > 2)
    {
        Density = 2;
    }

    if (trafficDensity != Density) /* tden 0..2   */
    {
        int z = ((CChr9 - ROADBASE) & 15) + DensityTable[Density];
        
        z += CChr & (ALLBITS - ANIMBIT);
        
        if (Density)
        {
            z += ANIMBIT;
        }

        Map[SMapX][SMapY] = z;
    }
}


/* comefrom: DoSPZone DoHospChur */
void RepairZone(int ZCent, int zsize)
{
  int cnt;
  int x, y, ThCh;

  zsize--;
  cnt = 0;
  for (y = -1; y < zsize; y++)
    for (x = -1; x < zsize; x++) {
      int xx = SMapX + x;
      int yy = SMapY + y;
      cnt++;
      if (CoordinatesValid(xx, yy, SimWidth, SimHeight)) {
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
    for (int y = (SMapY - 1); y < (SMapY + 3); y++)
    {
        for (int x = (SMapX - 1); x < (SMapX + 3); x++)
        {
            Map[x][y] = (z++) | BNCNBIT;
        }
    }
 
    Map[SMapX][SMapY] |= ZONEBIT | PWRBIT;
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


void DoSPZone(int PwrOn)
{
    static int MltdwnTab[3] = { 30000, 20000, 10000 };  /* simadj */
    int z;

    switch (CChr9)
    {
    case POWERPLANT:
        CoalPop++;
        if (!(CityTime & 7)) /* post */
        {
            RepairZone(POWERPLANT, 4);
        }
        PushPowerStack();
        CoalSmoke(SMapX, SMapY);
        return;

    case NUCLEAR:
        if (!NoDisasters && !RandomRange(0, MltdwnTab[GameLevel()]))
        {
            DoMeltdown(SMapX, SMapY);
            return;
        }
        NuclearPop++;
        if (!(CityTime & 7)) /* post */
        {
            RepairZone(NUCLEAR, 4);
        }
        PushPowerStack();
        return;

    case FIRESTATION:
        FireStPop++;
        if (!(CityTime & 7)) /* post */
        {
            RepairZone(FIRESTATION, 3);
        }

        if (PwrOn) /* if powered get effect  */
        {
            z = FireEffect;
        }
        else /* from the funding ratio  */
        {
            z = FireEffect >> 1;
        }

        if (!FindPRoad()) /* post FD's need roads  */
        {
            z = z / 2;
        }

        FireStMap[SMapX >> 3][SMapY >> 3] += z;
        return;

    case POLICESTATION:
        PolicePop++;
        if (!(CityTime & 7))
        {
            RepairZone(POLICESTATION, 3); /* post */
        }

        if (PwrOn)
        {
            z = PoliceEffect;
        }
        else
        {
            z = PoliceEffect / 2;
        }

        if (!FindPRoad())
        {
            z = z >> 1; /* post PD's need roads */
        }

        PoliceMap[SMapX >> 3][SMapY >> 3] += z;
        return;

    case STADIUM:
        StadiumPop++;
        if (!(CityTime & 15))
        {
            RepairZone(STADIUM, 4);
        }
        if (PwrOn)
        {
            if (!((CityTime + SMapX + SMapY) & 31)) // post release
            {
                DrawStadium(FULLSTADIUM);
                Map[SMapX + 1][SMapY] = FOOTBALLGAME1 + ANIMBIT;
                Map[SMapX + 1][SMapY + 1] = FOOTBALLGAME2 + ANIMBIT;
            }
        }
        return;

    case FULLSTADIUM:
        StadiumPop++;
        if (!((CityTime + SMapX + SMapY) & 7))	/* post release */
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

        if (PwrOn) // post
        { 
            if ((Map[SMapX + 1][SMapY - 1] & LOMASK) == RADAR)
            {
                Map[SMapX + 1][SMapY - 1] = RADAR + ANIMBIT + CONDBIT + BURNBIT;
            }
        }
        else
        {
            Map[SMapX + 1][SMapY - 1] = RADAR + CONDBIT + BURNBIT;
        }

        if (PwrOn)
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
        if (PwrOn &&
            (GetSprite(SHI) == NULL))
        {
            GenerateShip();
        }
        return;
    }
}


/* comefrom: Simulate DoSimInit */
void MapScan(int x1, int x2)
{
    for (int x = x1; x < x2; x++)
    {
        for (int y = 0; y < SimHeight; y++)
        {
            if (CChr = Map[x][y])
            {
                CChr9 = CChr & LOMASK;	// Mask off status bits
                if (CChr9 >= FLOOD)
                {
                    SMapX = x;
                    SMapY = y;
                    if (CChr9 < ROADBASE)
                    {
                        if (CChr9 >= FIREBASE)
                        {
                            FirePop++;
                            if (!(Rand16() & 3)) // 1 in 4 times
                            {
                                DoFire();
                            }
                            continue;
                        }
                        if (CChr9 < RADTILE)
                        {
                            DoFlood();
                        }
                        else
                        {
                            DoRadTile();
                        }
                        continue;
                    }

                    if (NewPower && (CChr & CONDBIT))
                    {
                        SetZPower();
                    }

                    if ((CChr9 >= ROADBASE) && (CChr9 < POWERBASE))
                    {
                        DoRoad();
                        continue;
                    }

                    if (CChr & ZONEBIT) // process Zones
                    {
                        DoZone();
                        continue;
                    }

                    if ((CChr9 >= RAILBASE) && (CChr9 < RESBASE))
                    {
                        DoRail();
                        continue;
                    }
                    if ((CChr9 >= SOMETINYEXP) && (CChr9 <= LASTTINYEXP)) // clear AniRubble
                    {
                        Map[x][y] = RUBBLE + (Rand16() & 3) + BULLBIT;
                    }
                }
            }
        }
    }
}


void SetValves(const Budget& budget)
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
    MiscHis[15] = GameLevel();
    MiscHis[16] = CityClass;
    MiscHis[17] = CityScore;

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
    switch (GameLevel())
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

    int index = std::clamp(budget.TaxRate() + GameLevel(), 0, 20);

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
    PwrdZCnt = 0;
    unPwrdZCnt = 0;
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
    PowerStackNum = 0; // Reset before Mapscan

    for (int x = 0; x < SmX; x++)
    {
        for (int y = 0; y < SmY; y++)
        {
            FireStMap[x][y] = 0;
            PoliceMap[x][y] = 0;
        }
    }
}


void TakeCensus()
{
    /* put census#s in Historical Graphs and scroll data  */
    ResHisMax = 0;
    ComHisMax = 0;
    IndHisMax = 0;
    for (int x = 118; x >= 0; x--)
    {
        if ((ResHis[x + 1] = ResHis[x]) > ResHisMax)
        {
            ResHisMax = ResHis[x];
        }
        if ((ComHis[x + 1] = ComHis[x]) > ComHisMax)
        {
            ComHisMax = ComHis[x];
        }
        if ((IndHis[x + 1] = IndHis[x]) > IndHisMax)
        {
            IndHisMax = IndHis[x];
        }
        CrimeHis[x + 1] = CrimeHis[x];
        PollutionHis[x + 1] = PollutionHis[x];
        MoneyHis[x + 1] = MoneyHis[x];
    }

    Graph10Max = ResHisMax;
    if (ComHisMax > Graph10Max)
    {
        Graph10Max = ComHisMax;
    }
    if (IndHisMax > Graph10Max)
    {
        Graph10Max = IndHisMax;
    }

    ResHis[0] = ResPop / 8;
    ComHis[0] = ComPop;
    IndHis[0] = IndPop;

    CrimeRamp += (CrimeAverage - CrimeRamp) / 4;
    CrimeHis[0] = CrimeRamp;

    PolluteRamp += (PolluteAverage - PolluteRamp) / 4;
    PollutionHis[0] = PolluteRamp;

    //int x = std::clamp((CashFlow / 20) + 128, 0, 255);

    MoneyHis[0] = std::clamp((CashFlow / 20) + 128, 0, 255); // scale to 0..255
    CrimeHis[0] = std::clamp(CrimeHis[0], 0, 255);
    PollutionHis[0] = std::clamp(PollutionHis[0], 0, 255);

    ChangeCensus(); /* XXX: if 10 year graph view */

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
    Res2HisMax = 0;
    Com2HisMax = 0;
    Ind2HisMax = 0;
    for (int x = 238; x >= 120; x--)
    {
        if ((ResHis[x + 1] = ResHis[x]) > Res2HisMax)
        {
            Res2HisMax = ResHis[x];
        }
        if ((ComHis[x + 1] = ComHis[x]) > Com2HisMax)
        {
            Com2HisMax = ComHis[x];
        }
        if ((IndHis[x + 1] = IndHis[x]) > Ind2HisMax)
        {
            Ind2HisMax = IndHis[x];
        }

        CrimeHis[x + 1] = CrimeHis[x];
        PollutionHis[x + 1] = PollutionHis[x];
        MoneyHis[x + 1] = MoneyHis[x];
    }

    Graph120Max = Res2HisMax;

    if (Com2HisMax > Graph120Max)
    {
        Graph120Max = Com2HisMax;
    }
    if (Ind2HisMax > Graph120Max)
    {
        Graph120Max = Ind2HisMax;
    }

    ResHis[120] = ResPop / 8;
    ComHis[120] = ComPop;
    IndHis[120] = IndPop;
    CrimeHis[120] = CrimeHis[0];
    PollutionHis[120] = PollutionHis[0];
    MoneyHis[120] = MoneyHis[0];
    ChangeCensus(); /* XXX: if 120 year graph view */
}



void CollectTax(Budget& budget)
{
    static float RLevels[3] = { 0.7f, 0.9f, 1.2f };
    static float FLevels[3] = { 1.4f, 1.2f, 0.8f };

    CashFlow = 0;
    // XXX: do something with z
    int z = AvCityTax / 48;  // post
    AvCityTax = 0;
    budget.PoliceFund( PolicePop * 100);
    budget.FireFund(FireStPop * 100);
    budget.RoadFund(static_cast<int>((RoadTotal + (RailTotal * 2)) * RLevels[GameLevel()]));
    budget.TaxFund(static_cast<int>(((static_cast<float>(TotalPop) * LVAverage) / 120.0f) * budget.TaxRate() * FLevels[GameLevel()])); //yuck

    if (TotalPop) // if there are people to tax
    {
        CashFlow = budget.CashFlow();
        DoBudget(budget);
    }
    else
    {
        RoadEffect = 32;
        PoliceEffect = 1000;
        FireEffect = 1000;
    }
}


// tends to empty RateOGMem
// ROG == Rate Of Growth
void DecROGMem()
{
    for (int x = 0; x < SmX; x++)
    {
        for (int y = 0; y < SmY; y++)
        {
            int z = RateOGMem[x][y];
            if (z == 0)
            {
                continue;
            }
            if (z > 0)
            {
                --RateOGMem[x][y];
                if (z > 200) // prevent overflow
                {
                    RateOGMem[x][y] = std::min(z, 200);
                }
                continue;
            }
            if (z < 0)
            {
                ++RateOGMem[x][y];
                if (z < -200)
                {
                    RateOGMem[x][y] = -200;
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
    //TaxFund = 0;
    /*
      if ((GameLevel > 2) || (GameLevel < 0)) GameLevel = 0;
      setGameLevel(GameLevel);
    */
}


void InitSimMemory()
{
    SetCommonInits();
    for (int x = 0; x < HistoryLength; x++)
    {
        ResHis[x] = 0;
        ComHis[x] = 0;
        IndHis[x] = 0;
        MoneyHis[x] = 128;
        CrimeHis[x] = 0;
        PollutionHis[x] = 0;
    }

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

    // This clears powermem
    PowerStackNum = 0;
    DoPowerScan();
    NewPower = 1; // post rel

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
                SMapX = x;
                SMapY = y;
                CChr = z;
                SetZPower();
            }
        }
    }
}


/* tends to empty TrfDensity   */
void DecTrafficMem()
{   
    for (int x = 0; x < HalfWorldWidth; x++)
    {
        for (int y = 0; y < HalfWorldHeight; y++)
        {
            int z = TrfDensity[x][y];
            if (z != 0)
            {
                if (z > 24)
                {
                    if (z > 200)
                    {
                        TrfDensity[x][y] = z - 34;
                    }
                    else
                    {
                        TrfDensity[x][y] = z - 24;
                    }
                }
                else TrfDensity[x][y] = 0;
            }
        }
    }
}


/* comefrom: DoSimInit */
void SimLoadInit()
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
    GameLevel(MiscHis[15]);

    if (CityTime < 0)
    {
        CityTime = 0;
    }

    if (!EMarket)
    {
        EMarket = 4.0;
    }

    SetCommonInits();

    CityClass = MiscHis[16];
    CityScore = MiscHis[17];

    if ((CityClass > 5) || (CityClass < 0))
    {
        CityClass = 0;
    }
    if ((CityScore > 999) || (CityScore < 1))
    {
        CityScore = 500;
    }

    ResCap = 0;
    ComCap = 0;
    IndCap = 0;

    AvCityTax = (CityTime % 48) * 7; /* post */

    for (int z = 0; z < PWRMAPSIZE; z++) /* set power Map */
    {
        PowerMap[z] = ~0;
    }

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
    int PowerScanFrequency[4] = { 1,  2,  4,  5 };
    int PollutionScanFrequency[4] = { 1,  2,  7, 17 };
    int CrimeScanFrequency[4] = { 1,  1,  8, 18 };
    int PopulationDensityScanFrequency[4] = { 1,  1,  9, 19 };
    int FireAnalysisFrequency[4] = { 1,  1, 10, 20 };
};


void Simulate(int mod16, Budget& budget)
{
    int speed = static_cast<int>(SimulationSpeed()); // ew, find a better way to do this

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
            SetValves(budget);
        }
        
        ClearCensus();
        break;

    case 1:
        MapScan(0, 1 * SimWidth / 8);
        break;

    case 2:
        MapScan(1 * SimWidth / 8, 2 * SimWidth / 8);
        break;

    case 3:
        MapScan(2 * SimWidth / 8, 3 * SimWidth / 8);
        break;

    case 4:
        MapScan(3 * SimWidth / 8, 4 * SimWidth / 8);
        break;

    case 5:
        MapScan(4 * SimWidth / 8, 5 * SimWidth / 8);
        break;

    case 6:
        MapScan(5 * SimWidth / 8, 6 * SimWidth / 8);
        break;

    case 7:
        MapScan(6 * SimWidth / 8, 7 * SimWidth / 8);
        break;

    case 8:
        MapScan(7 * SimWidth / 8, SimWidth);
        break;

    case 9:
        if (!(CityTime % CENSUSRATE))
        {
            TakeCensus();
        }
        if (!(CityTime % (CENSUSRATE * 12)))
        {
            Take2Census();
        }

        if (!(CityTime % TAXFREQ))
        {
            CollectTax(budget);
            CityEvaluation(budget);
        }
        break;

    case 10:
        if (!(Scycle % 5))
        {
            DecROGMem();
        }
        DecTrafficMem();
        NewMapFlags[TDMAP] = 1;
        NewMapFlags[RDMAP] = 1;
        NewMapFlags[ALMAP] = 1;
        NewMapFlags[REMAP] = 1;
        NewMapFlags[COMAP] = 1;
        NewMapFlags[INMAP] = 1;
        NewMapFlags[DYMAP] = 1;
        SendMessages(budget);
        break;

    case 11:
        if (!(Scycle % PowerScanFrequency[speed]))
        {
            DoPowerScan();
            NewMapFlags[PRMAP] = 1;
            NewPower = 1; /* post-release change */
        }
        break;

    case 12:
        if (!(Scycle % PollutionScanFrequency[speed]))
        {
            PTLScan();
        }
        break;

    case 13:
        if (!(Scycle % CrimeScanFrequency[speed]))
        {
            CrimeScan();
        }
        break;

    case 14:
        if (!(Scycle % PopulationDensityScanFrequency[speed]))
        {
            PopDenScan();
        }
        break;

    case 15:
        if (!(Scycle % FireAnalysisFrequency[speed]))
        {
            FireAnalysis();
        }
        DoDisasters();
        break;
    }
}


void SimFrame(Budget& budget)
{
    if (SimSpeed() == SimulationSpeed::Paused)
    {
        return;
    }

    if (++Fcycle > 1024)
    {
        Fcycle = 0;
    }
    
    Simulate(Fcycle % 16, budget);
}


void DoSimInit(Budget& budget)
{
    Fcycle = 0;
    Scycle = 0;

    if (InitSimLoad == 2) 			/* if new city    */
    {
        InitSimMemory();
    }

    if (InitSimLoad == 1)  			/* if city just loaded  */
    {
        SimLoadInit();
    }

    SetValves(budget);
    ClearCensus();
    MapScan(0, SimWidth); /* XXX are you sure ??? */
    DoPowerScan();
    NewPower = 1;		/* post rel */
    PTLScan();
    CrimeScan();
    PopDenScan();
    FireAnalysis();
    NewMap = 1;
    doAllGraphs();
    NewGraph = 1;
    TotalPop = 1;
    DoInitialEval = 1;
}


void UpdateFundEffects(const Budget& budget)
{
    if (budget.RoadFund())
    {
        RoadEffect = (int)(((float)budget.RoadSpend() / (float)budget.RoadFund()) * 32.0);
    }
    else
    {
        RoadEffect = 32;
    }

    if (budget.PoliceFund())
    {
        PoliceEffect = (int)(((float)budget.PoliceSpend() / (float)budget.PoliceFund()) * 1000.0);
    }
    else
    {
        PoliceEffect = 1000;
    }

    if (budget.FireFund())
    {
        FireEffect = (int)(((float)budget.FireSpend() / (float)budget.FireFund()) * 1000.0);
    }
    else
    {
        FireEffect = 1000;
    }

    drawCurrPercents();
}


void FireZone(int Xloc, int Yloc, int ch)
{
    int Xtem, Ytem;
    int XYmax;

    RateOGMem[Xloc / 8][Yloc / 8] -= 20;

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


static std::random_device RandomDevice;
static std::mt19937 PseudoRandomNumberGenerator(RandomDevice());


int RandomRange(int min, int max)
{
    std::uniform_int_distribution<std::mt19937::result_type> prngDistribution(min, max);
    return prngDistribution(PseudoRandomNumberGenerator);
}


int Random()
{
    return RandomRange(0, std::mt19937::max());
}


int Rand16()
{
    return RandomRange(0, 32767) * (RandomRange(0, 1) ? -1 : 1);
}
