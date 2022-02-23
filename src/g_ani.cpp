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
#include "animtab.h"

#include "main.h"
#include "Map.h"

#include <iostream>

void animateTiles()
{
    int* tMapPtr = &(Map[0][0]);
    for (int i = SimWidth * SimHeight; i > 0; i--)
    {
        int tilevalue = (*tMapPtr);
        if (tilevalue & ANIMBIT)
        {
            const int tileflags = tilevalue & ALLBITS;
            tilevalue &= LOMASK;

            tilevalue = aniTile[tilevalue];

            tilevalue |= tileflags;
            (*tMapPtr) = tilevalue;
        }
        tMapPtr++;
    }
}
