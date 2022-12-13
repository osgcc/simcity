// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 Leeor Dicker
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

class CityProperties;

int doFreePop();
int residentialZonePopulation(int tile);
int commercialZonePopulation(int tile);
int industrialZonePopulation(int tile);
bool setZonePower(const Point<int>& location);
void updateZone(const Point<int>& location, const CityProperties&);
