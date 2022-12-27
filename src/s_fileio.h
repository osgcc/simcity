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

#include <string>

enum class Scenario
{
    Dullsville,
    SanFransisco,
    Hamburg,
    Bern,
    Tokyo,
    Detroit,
    Boston,
    Rio
};

class Budget;
class CityProperties;

bool LoadCity(const std::string& filename, CityProperties&, Budget&);
void LoadScenario(Scenario, CityProperties&, Budget&);
void SaveCity(const std::string&, const CityProperties&, const Budget&);
