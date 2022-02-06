#pragma once

void setAnyCityName(const char* name);
void DoNewGame();
void makeDollarDecimalStr(char* numStr, char* dollarStr);
std::string NumberToDollarDecimal(int value);
void Pause();
void Resume();
int CurrentYear();
void SetYear(int year);
void SetGameLevelFunds(int level);
void setSpeed(int speed);
void setSkips(int skips);
void SetGameLevel(int level);
bool TestBounds(int x, int y, int width, int height);
