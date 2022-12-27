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


#include "s_alloc.h"


#include <SDL2/SDL.h>


struct Graph
{
    using PointsList = std::array<SDL_Point, HistoryLength>;

    const GraphHistory& history;
    const std::string name;
    const SDL_Color color;
    PointsList points;
};
