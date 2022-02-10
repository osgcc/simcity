#pragma once

void UpdateMaps();
void UpdateGraphs();
void UpdateEvaluation();
void UpdateHeads();
void UpdateFunds();
void doTimeStuff();
void showValves();
void ReallyUpdateFunds();
void updateOptions();

extern const std::string dateStr[12];

int LastCityTime();
void LastCityTime(int time);

int LastCityMonth();
void LastCityMonth(int month);

int LastCityYear();
void LastCityYear(int year);
