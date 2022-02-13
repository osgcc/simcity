#pragma once

extern std::string CityName;
extern int autoGo;

void Spend(int dollars);
void SetFunds(int dollars);
int TotalFunds();

int TickCount();

void InitGame();
void GameStarted();
void DoPlayNewCity();
