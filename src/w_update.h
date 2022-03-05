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

class Budget;

enum class Month
{
	Jan,
	Feb,
	Mar,
	Apr,
	May,
	Jun,
	Jul,
	Aug,
	Sep,
	Oct,
	Nov,
	Dec
};


void UpdateEvaluation();
void UpdateFunds(Budget&);
void updateDate();

extern const std::string dateStr[12];

int LastCityTime();
void LastCityTime(int time);

int LastCityMonth();
void LastCityMonth(int month);

int LastCityYear();
void LastCityYear(int year);

const std::string& MonthString(Month month);
