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
#include "main.h"

#include "Graphics.h"

#include "Map.h"

#include "s_alloc.h"

#include "Texture.h"

int DynamicData[32];

int dynamicFilter(int col, int row)
{
    /*
    int x;

    int r = row >> 1;
    int c = col >> 1;

    if (((DynamicData[0] > DynamicData[1]) ||
        ((x = PopulationDensityMap.value({ c, r })) >= DynamicData[0]) &&
        (x <= DynamicData[1])) &&
        ((DynamicData[2] > DynamicData[3]) ||
            ((x = RateOfGrowthMap[ c >> 2][r >> 2 ]) >= ((2 * DynamicData[2]) - 256)) &&
            (x <= ((2 * DynamicData[3]) - 256))) &&
        ((DynamicData[4] > DynamicData[5]) ||
            ((x = TrafficDensityMap.value({ c, r })) >= DynamicData[4]) &&
            (x <= DynamicData[5])) &&
        ((DynamicData[6] > DynamicData[7]) ||
            ((x = PollutionMap.value({ c, r })) >= DynamicData[6]) &&
            (x <= DynamicData[7])) &&
        ((DynamicData[8] > DynamicData[9]) ||
            ((x = CrimeMap.value({ c, r })) >= DynamicData[8]) &&
            (x <= DynamicData[9])) &&
        ((DynamicData[10] > DynamicData[11]) ||
            ((x = LandValueMap.value({ c, r })) >= DynamicData[10]) &&
            (x <= DynamicData[11])) &&
        ((DynamicData[12] > DynamicData[13]) ||
            ((x = PoliceProtectionMap[c >> 2][r >> 2]) >= DynamicData[12]) &&
            (x <= DynamicData[13])) &&
        ((DynamicData[14] > DynamicData[15]) ||
            ((x = FireProtectionMap[c >> 2][r >> 2]) >= DynamicData[14]) &&
            (x <= DynamicData[15]))) {
        return 1;
    }
    else
    {
        return 0;
    }
    */
    return 0;
}


void drawDynamic()
{
    /*
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    int* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < SimWidth; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < SimHeight; row++)
        {

            unsigned int tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if (tile > 63)
            {
                if (!dynamicFilter(col, row))
                {
                    tile = 0;
                }
            }
            ////////

            unsigned int* mem = (unsigned int*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned int l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
    */
}
