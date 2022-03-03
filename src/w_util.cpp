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
#include "w_util.h"

#include "Budget.h"

#include "main.h"

#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"

#include <algorithm>
#include <array>
#include <string>


namespace
{
    SimulationSpeed simulationSpeed;
    SimulationSpeed previousSimulationSpeed;

    std::array<std::string, 4> speedStringTable
    {
        "Paused", "Slow", "Normal", "Fast"
    };
};


Point<int> PositionToCell(const Point<int>& position, const Point<int>& offset)
{
    return
    {
        (((position.x) + (offset.x % 16)) / 16),
        (((position.y) + (offset.y % 16)) / 16),
    };
}


bool CoordinatesValid(int x, int y, int width, int height)
{
    return (((x) >= 0) && ((x) < width) && ((y) >= 0) && ((y) < height));
}


std::string NumberToDollarDecimal(int value)
{
    std::string valueString{ "$" + std::to_string(value)};

    if (value < 1000)
    {
        return valueString;
    }

    const size_t numOfCommas = (valueString.size() - 1) / 3;

    for (size_t i = 1; i <= numOfCommas; ++i)
    {
        valueString.insert((valueString.size() - (i - 1)) - (i * 3), ",");
    }

    return valueString;
}


const std::string& SpeedString(SimulationSpeed speed)
{
    return speedStringTable[static_cast<int>(speed)];
}


void SimSpeed(SimulationSpeed speed)
{
    if (speed == SimulationSpeed::Paused)
    {
        previousSimulationSpeed = simulationSpeed;
        simulationSpeed = speed;
        return;
    }

    simulationSpeed = speed;
}


SimulationSpeed SimSpeed()
{
    return simulationSpeed;
}


void Pause()
{
    previousSimulationSpeed = simulationSpeed;
    SimSpeed(SimulationSpeed::Paused);
}


void Resume()
{
    SimSpeed(previousSimulationSpeed);
}


bool Paused()
{
    return simulationSpeed == SimulationSpeed::Paused;
}


void SetGameLevelFunds(int level, Budget& budget)
{
    switch (level)
    {
    default:
    case 0:
        budget.CurrentFunds(20000);
        GameLevel(0);
        break;

    case 1:
        budget.CurrentFunds(10000);
        GameLevel(1);
        break;

    case 2:
        budget.CurrentFunds(5000);
        GameLevel(2);
        break;
    }
}


void setAnyCityName(const char *name)
{
  /*
  char buf[1024];

  CityName = name;
  sprintf(buf, "UISetCityName {%s}", CityName);
  Eval(buf);
  */
}


void SetYear(int year)
{
    // Must prevent year from going negative, since it screws up the non-floored modulo arithmetic.
    if (year < StartingYear)
    {
        year = StartingYear;
    }

    year = (year - StartingYear) - (CityTime / 48);
    CityTime += year * 48;
    updateDate();
}


int CurrentYear()
{
    return (CityTime / 48 + StartingYear);
}


void DoNewGame()
{
  //Eval("UINewGame");
}


void DoGeneratedCityImage(char *name, int time, int pop, char* _class, int score)
{
  /* XXX: TODO: print city */
}


void DoStartElmd()
{
  /* XXX: TODO: start elm daemon */
}


void DoPopUpMessage(char *msg)
{
  /*
  char buf[1024];

  sprintf(buf, "UIPopUpMessage {%s}", msg);
  Eval(buf);
  */
}
