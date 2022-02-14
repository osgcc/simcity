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


enum class ToolResult
{
    Success,
    OutOfBounds,
    InsufficientFunds,
    InvalidLocation,
    InvalidOperation,
    NetworkVotedNo,
    RequiresBulldozing,
    CannotBulldoze
};


void ToolDown(int mapX, int mapY);
bool tally(int tileValue);

extern std::map<Tool, ToolProperties> Tools;

extern Tool PendingTool;
