#pragma once

typedef Byte* Ptr;
typedef char** Handle;

extern std::string CityName;
extern int autoGo;

Ptr NewPtr(int size);

void Spend(int dollars);
void SetFunds(int dollars);
int TickCount();

void InitGame();
void GameStarted();
