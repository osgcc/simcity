#pragma once

#include <map>
#include <string>

enum class Tool
{
    Residential,
    Commercial,
    Industrial,
    Fire,
    Query,
    Police,
    Wire,
    Bulldoze,
    Rail,
    Road,
    Stadium,
    Park,
    Seaport,
    Coal,
    Nuclear,
    Airport,
    Network,
    None
};


struct ToolProperties
{
    int cost{};
    int size{};
    int offset{};
    const std::string name{};
};


extern std::map<Tool, ToolProperties> Tools;

void ToolDown(int mapX, int mapY);

int bulldozer_tool(int x, int y);
bool tally(int tileValue);
void put3x3Rubble(int x, int y);
int putDownPark(int mapH, int mapV);
int putDownNetwork(int mapH, int mapV);
int query_tool(int x, int y);

extern Tool PendingTool;
extern int PendingX;
extern int PendingY;
