/* g_smmaps.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
#include "main.h"

#include "s_alloc.h"


int DynamicData[32];


void drawAll(SimView * view)
{
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    short* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < WORLD_X; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < WORLD_Y; row++)
        {

            unsigned short tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            unsigned long* mem = (unsigned long*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned long l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
}


void drawRes(SimView* view)
{
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    short* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < WORLD_X; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < WORLD_Y; row++)
        {

            unsigned short tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if (tile > 422)
            {
                tile = 0;
            }
            ////////

            unsigned long* mem = (unsigned long*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned long l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
}


void drawCom(SimView* view)
{
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    short* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < WORLD_X; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < WORLD_Y; row++)
        {

            unsigned short tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if ((tile > 609) || ((tile >= 232) && (tile < 423)))
            {
                tile = 0;
            }
            ////////

            unsigned long* mem = (unsigned long*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned long l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
}


void drawInd(SimView* view)
{
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    short* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < WORLD_X; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < WORLD_Y; row++)
        {

            unsigned short tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if (((tile >= 240) && (tile <= 611)) ||
                ((tile >= 693) && (tile <= 851)) ||
                ((tile >= 860) && (tile <= 883)) ||
                (tile >= 932))
            {
                tile = 0;
            }
            ////////

            unsigned long* mem = (unsigned long*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned long l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
}


void drawLilTransMap(SimView* view)
{
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    short* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < WORLD_X; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < WORLD_Y; row++)
        {

            unsigned short tile = *(mp++) & LOMASK;
            if (tile >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            ////////
            if ((tile >= 240) ||
                ((tile >= 207) && tile <= 220) ||
                (tile == 223))
            {
                tile = 0;
            }
            ////////

            unsigned long* mem = (unsigned long*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned long l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
}


/* color pixel values */
#define UNPOWERED	COLOR_LIGHTBLUE
#define POWERED		COLOR_RED
#define CONDUCTIVE	COLOR_LIGHTGRAY


void drawPower(SimView* view)
{
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    int pix = 0;

    int powered = view->pixels[POWERED];
    int unpowered = view->pixels[UNPOWERED];
    int conductive = view->pixels[CONDUCTIVE];

    short* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < WORLD_X; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < WORLD_Y; row++)
        {
            unsigned short tile = *(mp++);

            if ((tile & LOMASK) >= TILE_COUNT)
            {
                tile -= TILE_COUNT;
            }

            if ((unsigned short)(tile & LOMASK) <= (unsigned short)63)
            {
                tile &= LOMASK;
                pix = -1;
            }
            else if (tile & ZONEBIT)
            {
                pix = (tile & PWRBIT) ? powered : unpowered;
            }
            else
            {
                if (tile & CONDBIT)
                {
                    pix = conductive;
                }
                else
                {
                    tile = 0;
                    pix = -1;
                }
            }

            if (pix < 0)
            {
                unsigned long* mem = (unsigned long*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
                for (int i = 0; i < 3; ++i)
                {
                    unsigned long l = mem[i];
                    image[0] = l >> 24;
                    image[1] = l >> 16;
                    image[2] = l >> 8;
                    image += lineBytes;
                }
            }
            else
            {
                for (int y = 0; y < 3; y++)
                {
                    unsigned char* img = image;
                    for (int x = 0; x < 4; x++)
                    {
                        *(img++) = (pix >> 0) & 0xff;
                        *(img++) = (pix >> 8) & 0xff;
                        *(img++) = (pix >> 16) & 0xff;
                        if (pixelBytes == 4)
                        {
                            img++;
                        }
                    }
                    image += lineBytes;
                }
            }
        }
    }
}


int dynamicFilter(int col, int row)
{
    int x;

    int r = row >> 1;
    int c = col >> 1;

    if (((DynamicData[0] > DynamicData[1]) ||
        ((x = PopDensity[c][r]) >= DynamicData[0]) &&
        (x <= DynamicData[1])) &&
        ((DynamicData[2] > DynamicData[3]) ||
            ((x = RateOGMem[c >> 2][r >> 2]) >= ((2 * DynamicData[2]) - 256)) &&
            (x <= ((2 * DynamicData[3]) - 256))) &&
        ((DynamicData[4] > DynamicData[5]) ||
            ((x = TrfDensity[c][r]) >= DynamicData[4]) &&
            (x <= DynamicData[5])) &&
        ((DynamicData[6] > DynamicData[7]) ||
            ((x = PollutionMem[c][r]) >= DynamicData[6]) &&
            (x <= DynamicData[7])) &&
        ((DynamicData[8] > DynamicData[9]) ||
            ((x = CrimeMem[c][r]) >= DynamicData[8]) &&
            (x <= DynamicData[9])) &&
        ((DynamicData[10] > DynamicData[11]) ||
            ((x = LandValueMem[c][r]) >= DynamicData[10]) &&
            (x <= DynamicData[11])) &&
        ((DynamicData[12] > DynamicData[13]) ||
            ((x = PoliceMapEffect[c >> 2][r >> 2]) >= DynamicData[12]) &&
            (x <= DynamicData[13])) &&
        ((DynamicData[14] > DynamicData[15]) ||
            ((x = FireRate[c >> 2][r >> 2]) >= DynamicData[14]) &&
            (x <= DynamicData[15]))) {
        return 1;
    }
    else
    {
        return 0;
    }
}


void drawDynamic(SimView* view)
{
    int lineBytes = view->line_bytes;
    int pixelBytes = view->pixel_bytes;

    short* mp = &Map[0][0];
    unsigned char* imageBase = view->data;

    for (int col = 0; col < WORLD_X; col++)
    {
        unsigned char* image = imageBase + (3 * pixelBytes * col);
        for (int row = 0; row < WORLD_Y; row++)
        {

            unsigned short tile = *(mp++) & LOMASK;
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

            unsigned long* mem = (unsigned long*)&view->smalltiles[tile * 4 * 4 * pixelBytes];
            for (int i = 0; i < 3; ++i)
            {
                unsigned long l = mem[i];
                image[0] = l >> 24;
                image[1] = l >> 16;
                image[2] = l >> 8;
                image += lineBytes;
            }
        }
    }
}
