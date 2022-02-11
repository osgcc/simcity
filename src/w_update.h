#pragma once

#include <string>

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


void UpdateMaps();
void UpdateGraphs();
void UpdateEvaluation();
void UpdateHeads();
void UpdateFunds();
void doTimeStuff();
void ReallyUpdateFunds();
void updateOptions();

extern const std::string dateStr[12];

int LastCityTime();
void LastCityTime(int time);

int LastCityMonth();
void LastCityMonth(int month);

int LastCityYear();
void LastCityYear(int year);

const std::string& MonthString(Month month);
