#pragma once

typedef Byte* Ptr;
typedef char** Handle;

Ptr NewPtr(int size);

void Spend(int dollars);
void SetFunds(int dollars);
