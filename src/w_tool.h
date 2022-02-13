#pragma once

void setWandState(int state);
int bulldozer_tool(int x, int y);
bool tally(int tileValue);
void put3x3Rubble(int x, int y);
int putDownPark(int mapH, int mapV);
int putDownNetwork(int mapH, int mapV);
int query_tool(int x, int y);

extern const int ToolSize[];
extern const int ToolOffset[];
