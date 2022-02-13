#pragma once

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

void CityName(const std::string& name);
const std::string& CityName();
