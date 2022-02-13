#pragma once

void setWandState(SimView* view, int state);
int bulldozer_tool(SimView* view, int x, int y);
bool tally(int tileValue);
void put3x3Rubble(int x, int y);
int putDownPark(SimView* view, int mapH, int mapV);
int putDownNetwork(SimView* view, int mapH, int mapV);

extern const int ToolSize[];
extern const int ToolOffset[];
