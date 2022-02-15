/* g_setup.c
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

#include "w_resrc.h"
#include "w_stubs.h"


void* GetObjectXpms(void* /*XDisplay**/ xd, int id, int frames)
{
	/*
	Pixmap* pixmaps = (Pixmap*)malloc(2 * frames * sizeof(Pixmap));
	XpmAttributes attributes;
	char name[256];

	attributes.visual = Tk_DefaultVisual(xd->screen);
	attributes.colormap = Tk_DefaultColormap(xd->screen);
	attributes.depth = Tk_DefaultDepth(xd->screen);
	attributes.valuemask = XpmVisual | XpmColormap | XpmDepth;

	for (int i = 0; i < frames; i++)
	{
		sprintf(name, "%s/images/obj%d-%d.xpm", HomeDir, id, i);
		if (XpmReadFileToPixmap(xd->dpy, RootWindowOfScreen(xd->screen), name, &pixmaps[i + i], &pixmaps[i + i + 1], &attributes) < 0)
		{
			fprintf(stderr, "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n", name);
			sim_exit(1); // Just sets tkMustExit and ExitReturn
			return NULL;
		}
	}
	return pixmaps;
	*/

	return nullptr;
}


void GetPixmaps(void* /*XDisplay**/ xd)
{
	/*
	if (xd->gray25_stipple == None)
	{
		xd->gray25_stipple = XCreatePixmapFromBitmapData(xd->dpy, xd->root, gray25_bits, gray25_width, gray25_height, 1, 0, 1);
		xd->gray50_stipple = XCreatePixmapFromBitmapData(xd->dpy, xd->root, gray50_bits, gray50_width, gray50_height, 1, 0, 1);
		xd->gray75_stipple = XCreatePixmapFromBitmapData(xd->dpy, xd->root, gray75_bits, gray75_width, gray75_height, 1, 0, 1);
		xd->vert_stipple = XCreatePixmapFromBitmapData(xd->dpy, xd->root, vert_bits, vert_width, vert_height, 1, 0, 1);
		xd->horiz_stipple = XCreatePixmapFromBitmapData(xd->dpy, xd->root, horiz_bits, horiz_width, horiz_height, 1, 0, 1);
		xd->diag_stipple = XCreatePixmapFromBitmapData(xd->dpy, xd->root, diag_bits, diag_width, diag_height, 1, 0, 1);
	}

	if (xd->objects == NULL)
	{
		Pixmap** pm;

		xd->objects = pm = (Pixmap**)malloc(OBJN * sizeof(Pixmap*));

		pm[0] = NULL; // no object number 0
		pm[TRA] = GetObjectXpms(xd, TRA, 5);
		pm[COP] = GetObjectXpms(xd, COP, 8);
		pm[AIR] = GetObjectXpms(xd, AIR, 11);
		pm[SHI] = GetObjectXpms(xd, SHI, 8);
		pm[GOD] = GetObjectXpms(xd, GOD, 16);
		pm[TOR] = GetObjectXpms(xd, TOR, 3);
		pm[EXP] = GetObjectXpms(xd, EXP, 6);
		pm[BUS] = GetObjectXpms(xd, BUS, 4);
	}
	*/
}
