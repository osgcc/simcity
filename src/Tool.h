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

#include "Point.h"
#include "Vector.h"

#include <map>
#include <string>

class Budget;

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
    bool draggable{ false };
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


void ToolDown(const Point<int> location, Budget& budget);
bool tally(int tileValue);

const ToolProperties& toolProperties(const Tool);
const ToolProperties& pendingToolProperties();

Tool pendingTool();
void pendingTool(const Tool);

void toolStart(const Point<int>&);
const Point<int>& toolStart();

void toolEnd(const Point<int>&);
const Point<int>& toolEnd();

int longestAxis(const Vector<int>&);
void validateDraggableToolVector(Vector<int>&, Budget&);
void executeDraggableTool(const Vector<int>&, const Point<int>&, Budget&);
