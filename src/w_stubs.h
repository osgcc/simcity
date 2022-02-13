#pragma once

extern std::string CityName;
extern int autoGo;

void Spend(int dollars);
void SetFunds(int dollars);
int TotalFunds();
int LastFunds();
void LastFunds(const int funds);

int TickCount();

void InitGame();
void GameStarted();
void DoPlayNewCity();
