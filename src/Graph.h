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
