/* g_bigmap.c
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


int dynamicFilter(int c, int r);


void MemDrawBeegMapRect(SimView* view, int x, int y, int w, int h)
{
	int lineBytes = view->line_bytes;
	int pixelBytes = view->pixel_bytes;
	int ii, mm;
	unsigned int* map;
	unsigned int tile;
	unsigned char blink = (flagBlink <= 0), * bt = view->bigtiles;
	int** have, * ha;

	if (x < view->tile_x)
	{
		if ((w -= (view->tile_x - x)) <= 0)
		{
			return;
		}
		x = view->tile_x;
	}
	if (y < view->tile_y)
	{
		if ((h -= (view->tile_y - y)) <= 0)
		{
			return;
		}
		y = view->tile_y;
	}
	if ((x + w) > (view->tile_x + view->tile_width))
	{
		if ((w -= ((x + w) - (view->tile_x + view->tile_width))) <= 0)
		{
			return;
		}
	}
	if ((y + h) > (view->tile_y + view->tile_height))
	{
		if ((h -= ((y + h) - (view->tile_y + view->tile_height))) <= 0)
		{
			return;
		}
	}

	//if (view->x->color)
	//{
	unsigned int* image, * mem;

	image = (unsigned int*)view->data;
	ii = ((lineBytes * h * 16) - 16) / sizeof(unsigned int);
	map = (unsigned int*)&Map[x][y];
	mm = WORLD_Y - h;
	have = view->tiles;

	/*
	 * Huge Berserk Rebel Warthog  <-- ?
	 */

	for (int col = 0; col < w; col++)
	{
		ha = &have[col][0];
		image = (unsigned int*)(view->data + (col * 16 * pixelBytes));
		for (int row = 0; row < h; row++, ha++)
		{
			tile = *(map++);
			if ((tile & LOMASK) >= TILE_COUNT)
			{
				tile -= TILE_COUNT;
			}

			/* Blink lightning bolt in unpowered zone center */
			if (blink && (tile & ZONEBIT) && !(tile & PWRBIT))
			{
				tile = LIGHTNINGBOLT;
			}
			else
			{
				tile &= LOMASK;
			}

			if ((tile > 63) && (view->dynamic_filter != 0) && (dynamicFilter(col + x, row + y) == 0))
			{
				tile = 0;
			}

			if (tile == *ha)
			{
				image = (unsigned int*)(((unsigned char*)image) + (lineBytes * 16));
			}
			else
			{
				*ha = tile;
				mem = (unsigned int*)&(bt[tile * 256 * pixelBytes]);

				/* XXX: handle depth for big tiles */

				/* Not so un-rolled loop. */


				for (int i = 16; i > 0; i--)
				{
					image[0] = mem[0]; image[1] = mem[1];
					image[2] = mem[2]; image[3] = mem[3];
					image = (unsigned int*)(((unsigned char*)image) + lineBytes);
					mem += 4;
				}



			}

		}
		image -= ii;
		map += mm;
	}
	/*}
	else
	{
		unsigned int* image, * mem;

		image = (unsigned int*)view->data;
		ii = ((lineBytes * h * 16) - 2) / sizeof(unsigned int);
		map = (unsigned int*)&Map[x][y];
		mm = WORLD_Y - h;
		have = view->tiles;

		for (int col = 0; col < w; col++)
		{
			ha = &have[col][0];
			image = (unsigned int*)(view->data + (col * 2));
			for (int row = 0; row < h; row++, ha++)
			{
				tile = *(map++);
				if ((tile & LOMASK) >= TILE_COUNT) tile -= TILE_COUNT;

				//* Blink lightning bolt in unpowered zone center
				if (blink && (tile & ZONEBIT) && !(tile & PWRBIT))
				{
					tile = LIGHTNINGBOLT;
				}
				else
				{
					tile &= LOMASK;
				}

				if (tile == *ha)
				{
					image = (unsigned int*)(((unsigned char*)image) + (lineBytes * 16));
				}
				else
				{
					*ha = tile;
					mem = (unsigned int*)&(bt[tile * 32]);

					for (char i = 16; i > 0; i--)
					{
						*image = *mem;
						image = (unsigned int*)(((unsigned char*)image) + lineBytes);
						mem++;
					}

				}
			}
			image -= ii;
			map += mm;
		}
	}
	*/
}


void WireDrawBeegMapRect(SimView* view, int x, int y, int w, int h)
{
	unsigned int* map;
	unsigned int tile;
	unsigned char blink = (flagBlink <= 0);
	int mm;
	int** have, * ha;

	if (x < view->tile_x)
	{
		if ((w -= (view->tile_x - x)) <= 0)
		{
			return;
		}
		x = view->tile_x;
	}
	if (y < view->tile_y)
	{
		if ((h -= (view->tile_y - y)) <= 0)
		{
			return;
		}
		y = view->tile_y;
	}
	if ((x + w) > (view->tile_x + view->tile_width))
	{
		if ((w -= ((x + w) - (view->tile_x + view->tile_width))) <= 0)
		{
			return;
		}
	}
	if ((y + h) > (view->tile_y + view->tile_height))
	{
		if ((h -= ((y + h) - (view->tile_y + view->tile_height))) <= 0)
		{
			return;
		}
	}

	map = (unsigned int*)&Map[x][y];
	mm = WORLD_Y - h;
	have = view->tiles;

	for (int col = 0; col < w; col++)
	{
		ha = &have[col][0];
		for (int row = 0; row < h; row++, ha++)
		{
			tile = *(map++);
			if ((tile & LOMASK) >= TILE_COUNT) tile -= TILE_COUNT;

			/* Blink lightning bolt in unpowered zone center */
			if (blink && (tile & ZONEBIT) && !(tile & PWRBIT))
			{
				tile = LIGHTNINGBOLT;
			}
			else
			{
				tile &= LOMASK;
			}

			if (tile != *ha)
			{
				*ha = tile;
				// blit
				//XCopyArea(display, src, dest, gc, src_x, src_y, width, height,  dest_x, dest_y)
				//XCopyArea(view->x->dpy, view->x->big_tile_pixmap, view->pixmap, view->x->gc, 0, tile * 16, 16, 16, col * 16, row * 16);
			}
		}
		map += mm;
	}
}
