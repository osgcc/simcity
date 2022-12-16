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

#include "main.h"

#include "Point.h"
#include "Vector.h"

#include <array>


/* Character Mapping */
constexpr auto DIRT = 0;
constexpr auto RIVER = 2;
constexpr auto REDGE = 3;
constexpr auto CHANNEL = 4;
constexpr auto FIRSTRIVEDGE = 5;
constexpr auto LASTRIVEDGE = 20;
constexpr auto TREEBASE = 21;
constexpr auto LASTTREE = 36;
constexpr auto WOODS = 37;
constexpr auto UNUSED_TRASH1 = 38;
constexpr auto UNUSED_TRASH2 = 39;
constexpr auto WOODS2 = 40;
constexpr auto WOODS3 = 41;
constexpr auto WOODS4 = 42;
constexpr auto WOODS5 = 43;
constexpr auto RUBBLE = 44;
constexpr auto LASTRUBBLE = 47;
constexpr auto FLOOD = 48;
constexpr auto LASTFLOOD = 51;
constexpr auto RADTILE = 52; // Radiation tile
constexpr auto UNUSED_TRASH3 = 53;
constexpr auto UNUSED_TRASH4 = 54;
constexpr auto UNUSED_TRASH5 = 55;
constexpr auto FIRE = 56;
constexpr auto FIREBASE = 56; // Fire base value
constexpr auto LASTFIRE = 63; // Fire end value
constexpr auto ROADBASE = 64; // Road base value
constexpr auto HBRIDGE = 64;
constexpr auto VBRIDGE = 65;
constexpr auto ROADS = 66;
constexpr auto ROADSV = 67;
constexpr auto INTERSECTION = 76;
constexpr auto HROADPOWER = 77;
constexpr auto VROADPOWER = 78;
constexpr auto BRWH = 79;
constexpr auto LTRFBASE = 80; // Light traffic base value
constexpr auto BRWV = 95;
constexpr auto BRWXXX1 = 111;
constexpr auto BRWXXX2 = 127;
constexpr auto BRWXXX3 = 143;
constexpr auto HTRFBASE = 144; // Heavy traffic base value
constexpr auto BRWXXX4 = 159;
constexpr auto BRWXXX5 = 175;
constexpr auto BRWXXX6 = 191;
constexpr auto LASTROAD = 206;
constexpr auto BRWXXX7 = 207;
constexpr auto POWERBASE = 208;
constexpr auto HPOWER = 208;
constexpr auto VPOWER = 209;
constexpr auto LHPOWER = 210;
constexpr auto LVPOWER = 211;
constexpr auto RAILHPOWERV = 221;
constexpr auto RAILVPOWERH = 222;
constexpr auto LASTPOWER = 222;
constexpr auto UNUSED_TRASH6 = 223;
constexpr auto RAILBASE = 224;
constexpr auto HRAIL = 224;
constexpr auto VRAIL = 225;
constexpr auto LHRAIL = 226;
constexpr auto LVRAIL = 227;
constexpr auto HRAILROAD = 237;
constexpr auto VRAILROAD = 238;
constexpr auto LASTRAIL = 238;
constexpr auto ROADVPOWERH = 239; /* bogus? */
constexpr auto ResidentialBase = 240;
constexpr auto ResidentialEmpty = 244;
constexpr auto HOUSE = 249;
constexpr auto LHTHR = 249;
constexpr auto HHTHR = 260;
constexpr auto RZB = 265;
constexpr auto HOSPITAL = 409;
constexpr auto CHURCH = 418;
constexpr auto COMBASE = 423;
constexpr auto CommercialEmpty = 427;
constexpr auto CZB = 436;
constexpr auto INDBASE = 612;
constexpr auto IndustryEmpty = 616;
constexpr auto LASTIND = 620;
constexpr auto IND1 = 621;
constexpr auto IZB = 625;
constexpr auto IND2 = 641;
constexpr auto IND3 = 644;
constexpr auto IND4 = 649;
constexpr auto IND5 = 650;
constexpr auto IND6 = 676;
constexpr auto IND7 = 677;
constexpr auto IND8 = 686;
constexpr auto IND9 = 689;
constexpr auto PORTBASE = 693; // Port base value
constexpr auto PORT = 698;
constexpr auto LASTPORT = 708;
constexpr auto AIRPORTBASE = 709;
constexpr auto RADAR = 711;
constexpr auto AIRPORT = 716;
constexpr auto COALBASE = 745;
constexpr auto POWERPLANT = 750;
constexpr auto LASTPOWERPLANT = 760;
constexpr auto FIRESTBASE = 761;
constexpr auto FIRESTATION = 765;
constexpr auto POLICESTBASE = 770;
constexpr auto POLICESTATION = 774;
constexpr auto STADIUMBASE = 779;
constexpr auto STADIUM = 784;
constexpr auto FULLSTADIUM = 800;
constexpr auto NUCLEARBASE = 811;
constexpr auto NUCLEAR = 816;
constexpr auto LASTZONE = 826;
constexpr auto LIGHTNINGBOLT = 827;
constexpr auto HBRDG0 = 828;
constexpr auto HBRDG1 = 829;
constexpr auto HBRDG2 = 830;
constexpr auto HBRDG3 = 831;
constexpr auto RADAR0 = 832;
constexpr auto RADAR1 = 833;
constexpr auto RADAR2 = 834;
constexpr auto RADAR3 = 835;
constexpr auto RADAR4 = 836;
constexpr auto RADAR5 = 837;
constexpr auto RADAR6 = 838;
constexpr auto RADAR7 = 839;
constexpr auto FOUNTAIN = 840;
constexpr auto INDBASE2 = 844;
constexpr auto TELEBASE = 844;
constexpr auto TELELAST = 851;
constexpr auto SMOKEBASE = 852;
constexpr auto TINYEXP = 860;
constexpr auto SOMETINYEXP = 864;
constexpr auto LASTTINYEXP = 867;
constexpr auto COALSMOKE1 = 916;
constexpr auto COALSMOKE2 = 920;
constexpr auto COALSMOKE3 = 924;
constexpr auto COALSMOKE4 = 928;
constexpr auto FOOTBALLGAME1 = 932;
constexpr auto FOOTBALLGAME2 = 940;
constexpr auto VBRDG0 = 948;
constexpr auto VBRDG1 = 949;
constexpr auto VBRDG2 = 950;
constexpr auto VBRDG3 = 951;

constexpr auto TILE_COUNT = 960;


extern std::array<std::array<int, SimHeight>, SimWidth> Map; // Main Map 120 x 100

void DrawBigMapSegment(const Point<int>& begin, const Point<int>& end);
void DrawBigMap();

int& tileValue(const Point<int>& location);
int& tileValue(const int x, const int y);

unsigned int maskedTileValue(const Point<int>& location);
unsigned int maskedTileValue(const int x, const int y);
unsigned int maskedTileValue(unsigned int tile);

bool tilePowered(const unsigned int tile);
bool tileIsZoned(const unsigned int tile);

bool tileIsRoad(const Point<int> coordinates);

bool blink();
void ResetMap();

void toggleBlinkFlag();
