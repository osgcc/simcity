#pragma once

typedef Byte* Ptr;
typedef char** Handle;

extern std::string CityName;

Ptr NewPtr(int size);

void Spend(int dollars);
void SetFunds(int dollars);
int TickCount();
