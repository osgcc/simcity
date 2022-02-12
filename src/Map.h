#pragma once

#include "Point.h"
#include "Vector.h"

//void DrawBigMap(const Point<int>& drawOrigin, const Point<int>& offset, const Vector<int>& loops);
void DrawBigMapSegment(const Point<int>& begin, const Point<int>& end);
void DrawBigMap();

unsigned int getTileValue(const int x, const int y);

bool tilePowered(const unsigned int tile);
bool tileIsZoned(const unsigned int tile);
bool blink();
