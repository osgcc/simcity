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

#include "w_sound.h"

#include <iostream>
#include <string>


int earthquake_timer_set = 0;
int earthquake_delay = 3000;
int PerformanceTiming;


bool Eval(const std::string& buf)
{
    std::cout << "Eval: " << buf << std::endl;
    return false;
}


void StopEarthquake()
{
    std::cout << "StopEarthquake" << std::endl;
    ShakeNow = 0;
    if (earthquake_timer_set)
    {
        //Tk_DeleteTimerHandler(earthquake_timer_token);
    }
    earthquake_timer_set = 0;
}


void DoEarthQuake()
{
    std::cout << "DoEarthQuake" << std::endl;
    MakeSound("city", "Explosion-Low");
    Eval("UIEarthQuake");
    ShakeNow++;
    if (earthquake_timer_set)
    {
        //Tk_DeleteTimerHandler(earthquake_timer_token);
    }
    //Tk_CreateTimerHandler(earthquake_delay, (void (*)())StopEarthquake, (ClientData)0);
    earthquake_timer_set = 1;
}
