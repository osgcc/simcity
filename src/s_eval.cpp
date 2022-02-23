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

#include "s_alloc.h"
#include "s_sim.h"

#include "w_eval.h"

/* City Evaluation */


int EvalValid;
int CityYes, CityNo;
int ProblemTable[PROBNUM];
int ProblemTaken[PROBNUM];
int ProblemVotes[PROBNUM];		/* these are the votes for each  */
int ProblemOrder[4];			/* sorted index to above  */
int CityPop, deltaCityPop;
int CityAssessedValue;               /* assessed city value */
int CityClass;                  /*  0..5  */
int CityScore, deltaCityScore, AverageCityScore;
int TrafficAverage;


void EvalInit()
{
    CityYes = 0;
    CityNo = 0;
    CityPop = 0;
    deltaCityPop = 0;
    CityAssessedValue = 0;
    CityClass = 0;
    CityScore = 500;
    deltaCityScore = 0;
    EvalValid = 1;
    
    std::fill_n(ProblemVotes, PROBNUM, 0);
    std::fill_n(ProblemOrder, 4, 0);
}


void GetAssessedValue()
{
    int z = RoadTotal * 5;
    z += RailTotal * 10;
    z += PolicePop * 1000;
    z += FireStPop * 1000;
    z += HospPop * 400;
    z += StadiumPop * 3000;
    z += PortPop * 5000;
    z += APortPop * 10000;
    z += CoalPop * 3000;
    z += NuclearPop * 6000;
    CityAssessedValue = z * 1000;
}


void DoPopNum()
{
    int OldCityPop = CityPop;
    CityPop = ((ResPop)+(ComPop * 8) + (IndPop * 8)) * 20;

    if (OldCityPop == -1)
    {
        OldCityPop = CityPop;
    }

    deltaCityPop = CityPop - OldCityPop;

    /**
     * 0 == village
     * 2000 == town
     * 10000 == city
     * 50000 == capital
     * 100000 == metropolis
     * 500000 == megalopolis
     */

    CityClass = 0;
    if (CityPop > 2000) { CityClass++; }
    if (CityPop > 10000) { CityClass++; }
    if (CityPop > 50000) { CityClass++; }
    if (CityPop > 100000) { CityClass++; }
    if (CityPop > 500000) { CityClass++; }
}


void VoteProblems()
{
    int x, z, count;

    for (z = 0; z < PROBNUM; z++)
    {
        ProblemVotes[z] = 0;
    }

    x = 0;
    z = 0;
    count = 0;
    while ((z < 100) && (count < 600))
    {
        if (RandomRange(0, 300) < ProblemTable[x])
        {
            ProblemVotes[x]++;
            z++;
        }
        x++;
        if (x > PROBNUM)
        {
            x = 0;
        }
        count++;
    }
}


int AverageTrf()
{
    int TrfTotal;
    int x, y, count;

    TrfTotal = 0;
    count = 1;
    for (x = 0; x < HalfWorldWidth; x++)
    {
        for (y = 0; y < HalfWorldHeight; y++)
        {
            if (LandValueMem[x][y])
            {
                TrfTotal += TrfDensity[x][y];
                count++;
            }
        }
    }

    TrafficAverage = static_cast<int>((TrfTotal / count) * 2.4);
    return(TrafficAverage);
}


int GetUnemployment()
{
    float r;
    int b;

    b = (ComPop + IndPop) << 3;
    if (b)
    {
        r = ((float)ResPop) / b;
    }
    else
    {
        return 0;
    }

    b = static_cast<int>((r - 1) * 255);
    if (b > 255)
    {
        b = 255;
    }
    return b;
}


int GetFire()
{
    int z;

    z = FirePop * 5;
    if (z > 255)
    {
        return(255);
    }
    else
    {
        return(z);
    }
}


void GetScore()
{
    int x, z;
    int OldCityScore;
    float SM, TM;

    OldCityScore = CityScore;
    x = 0;
    for (z = 0; z < 7; z++)
    {
        x += ProblemTable[z];	/* add 7 probs */
    }

    x = x / 3;			/* 7 + 2 average */
    if (x > 256)
    {
        x = 256;
    }

    z = (256 - x) * 4;
    if (z > 1000)
    {
        z = 1000;
    }
    if (z < 0)
    {
        z = 0;
    }

    if (ResCap) { z = static_cast<int>(z * .85); }
    if (ComCap) { z = static_cast<int>(z * .85); }
    if (IndCap) { z = static_cast<int>(z * .85); }
    if (RoadEffect < 32) { z = z - (32 - RoadEffect); }
    if (PoliceEffect < 1000) { z = static_cast<int>(z * (.9 + (PoliceEffect / 10000.1))); }
    if (FireEffect < 1000) { z = static_cast<int>(z * (.9 + (FireEffect / 10000.1))); }
    if (RValve < -1000) { z = static_cast<int>(z * .85); }
    if (CValve < -1000) { z = static_cast<int>(z * .85); }
    if (IValve < -1000) { z = static_cast<int>(z * .85); }

    SM = 1.0;
    if ((CityPop == 0) || (deltaCityPop == 0))
    {
        SM = 1.0;
    }
    else if (deltaCityPop == CityPop)
    {
        SM = 1.0;
    }
    else if (deltaCityPop > 0)
    {
        SM = ((float)deltaCityPop / CityPop) + 1.0f;
    }
    else if (deltaCityPop < 0)
    {
        SM = .95f + ((float)deltaCityPop / (CityPop - deltaCityPop));
    }
    z = static_cast<int>(z * SM);
    z = z - GetFire();		/* dec score for fires */
    z = z - (CityTax);

    TM = static_cast<float>(unPwrdZCnt + PwrdZCnt);	/* dec score for unpowered zones */
    if (TM) { SM = PwrdZCnt / TM; }
    else { SM = 1.0; }
    z = static_cast<int>(z * SM);

    if (z > 1000) { z = 1000; }
    if (z < 0) { z = 0; }

    CityScore = (CityScore + z) / 2;

    deltaCityScore = CityScore - OldCityScore;
}


void DoVotes()
{
    int z;

    CityYes = 0;
    CityNo = 0;
    for (z = 0; z < 100; z++)
    {
        if (RandomRange(0, 1000) < CityScore)
        {
            CityYes++;
        }
        else
        {
            CityNo++;
        }
    }
}


void DoProblems()
{
    int x, z;
    int ThisProb, Max;

    for (z = 0; z < PROBNUM; z++)
    {
        ProblemTable[z] = 0;
    }

    ProblemTable[0] = CrimeAverage; /* Crime */
    ProblemTable[1] = PolluteAverage; /* Pollution */
    ProblemTable[2] = static_cast<int>(LVAverage * .7); /* Housing */
    ProblemTable[3] = CityTax * 10; /* Taxes */
    ProblemTable[4] = AverageTrf(); /* Traffic */
    ProblemTable[5] = GetUnemployment(); /* Unemployment */
    ProblemTable[6] = GetFire(); /* Fire */

    VoteProblems();
    
    for (z = 0; z < PROBNUM; z++)
    {
        ProblemTaken[z] = 0;
    }

    for (z = 0; z < 4; z++)
    {
        Max = 0;
        for (x = 0; x < 7; x++)
        {
            if ((ProblemVotes[x] > Max) && (!ProblemTaken[x]))
            {
                ThisProb = x;
                Max = ProblemVotes[x];
            }
        }
        if (Max)
        {
            ProblemTaken[ThisProb] = 1;
            ProblemOrder[z] = ThisProb;
        }
        else
        {
            ProblemOrder[z] = 7;
            ProblemTable[7] = 0;
        }
    }
}


void CityEvaluation()
{
    EvalValid = 0;
    if (TotalPop)
    {
        GetAssessedValue();
        DoPopNum();
        DoProblems();
        GetScore();
        DoVotes();
        ChangeEval();
    }
    else
    {
        EvalInit();
        ChangeEval();
    }

    EvalValid = 1;
}
