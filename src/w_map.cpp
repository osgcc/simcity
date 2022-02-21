/* w_map.c
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



namespace
{
    
};


void map_command_init()
{
    /*
    int new;
    extern int TileViewCmd(ClientData clientData, Tcl_Interp * interp, int argc, char** argv);

    Tcl_CreateCommand(tk_mainInterp, "mapview", TileViewCmd,
        (ClientData)MainWindow, (void (*)()) NULL);

    Tcl_InitHashTable(&MapCmds, TCL_STRING_KEYS);

#define MAP_CMD(name) HASHED_CMD(Map, name)

    MAP_CMD(configure);
    MAP_CMD(position);
    MAP_CMD(size);
    MAP_CMD(MapState);
    MAP_CMD(ShowEditors);
    MAP_CMD(PanStart);
    MAP_CMD(PanTo);
    MAP_CMD(Visible);
    MAP_CMD(ViewAt);
    */
}

/*
int DoMapCmd(ClientData clientData, Tcl_Interp* interp, int argc, char** argv)
{
    
    SimView* view = (SimView*)clientData;
    Tcl_HashEntry* ent;
    int result = TCL_OK;
    int (*cmd)();

    if (argc < 2) {
        return TCL_ERROR;
    }

    if (ent = Tcl_FindHashEntry(&MapCmds, argv[1])) {
        cmd = (int (*)())ent->clientData;
        Tk_Preserve((ClientData)view);
        result = cmd(view, interp, argc, argv);
        Tk_Release((ClientData)view);
    }
    else {
        Tcl_AppendResult(interp, "unknown command name: \"",
            argv[0], " ", argv[1], "\".", (char*)NULL);
        result = TCL_ERROR;
    }
    return result;
   
    return 0;
}
 */


void DoNewMap()
{
    //sim->maps++; view->next = sim->map; sim->map = view;
    //NewMap = 1;
    //view->invalid = 1;
}


/**
 * This appears to draw the camera indicator box in the minimap
 */
void DrawMapEditorViews()
{
    /*
    Pixmap pm = Tk_WindowId(view->tkwin);
    struct SimView* ed;
    int left, right, top, bottom, width, height;
    int mine;

    XSetLineAttributes(view->x->dpy, view->x->gc, 1, LineSolid, CapButt, JoinBevel);

    for (ed = sim->editor; ed != NULL; ed = ed->next)
    {
        if ((ed->x != view->x) || (ed->show_me == 0))
            continue;

        width = ed->w_width;
        height = ed->w_height;
        left = ed->pan_x - (width / 2);
        top = ed->pan_y - (height / 2);
        right = left + width;
        bottom = top + height;

        left = left * 3 / 16;
        top = top * 3 / 16;
        right = right * 3 / 16;
        bottom = bottom * 3 / 16;
        width = right - left;
        height = bottom - top;

        XSetForeground(view->x->dpy, view->x->gc, view->pixels[COLOR_WHITE]);
        XDrawRectangle(view->x->dpy, pm, view->x->gc, left - 3, top - 3, width + 3, height + 3);

        XSetForeground(view->x->dpy, view->x->gc, view->pixels[COLOR_BLACK]);
        XDrawRectangle(view->x->dpy, pm, view->x->gc, left - 1, top - 1, width + 3, height + 3);

        XSetForeground(view->x->dpy, view->x->gc, view->pixels[COLOR_YELLOW]);
        XDrawRectangle(view->x->dpy, pm, view->x->gc, left - 2, top - 2, width + 3, height + 3);
    }
    */
}


/* 
 * Sending the whole image is 108108 bytes.
 * Sending points is 4.4 bytes per point.
 * One image is as big as 24570 points.
 * But we have to sort these dang things.
 */

#define MAX_PIX 256
int max_pix = MAX_PIX;

struct Pix {
  int color;
  int x, y;
};

struct Pix pix[MAX_PIX];


int CompareColor(struct Pix* p1, struct Pix* p2)
{
    char c1 = p1->color, c2 = p2->color;

    if (c1 == c2)
    {
        return (0);
    }
    return ((c1 < c2) ? -1 : 1);
}


int DoUpdateMap()
{
    // Frame skips?
    /*
    int dx, dy, i;

    view->updates++;

    if (!view->visible)
    {
        return 0;
    }

    if ((!ShakeNow) && (!view->update) && (sim_skips || view->skips))
    {
        if (sim_skips)
        {
            if (sim_skip > 0)
            {
                return 0;
            }
        }
        else
        {
            if (view->skip > 0)
            {
                --view->skip;
                return 0;
            }
            else
            {
                view->skip = view->skips;
            }
        }
    }

    view->update = 0;
    view->skip = 0;

    if (view->invalid || NewMap || ShakeNow)
    {

        view->invalid = 0;

        switch (view->type)
        {

        case X_Mem_View:
            MemDrawMap(view);
            break;

        case X_Wire_View:
            WireDrawMap(view);
            break;
        }

    }

    // XXX: don't do this stuff if just redrawing overlay
    for (dx = dy = i = 0; i < ShakeNow; i++)
    {
        dx += RandomRange(0, 16) - 8;
        dy += RandomRange(0, 16) - 8;
    }

    //XCopyArea(view->x->dpy, view->pixmap, view->pixmap2, view->x->gc, dx, dy, view->w_width, view->w_height, 0, 0);
    DrawMapInk(view);

    //* XXX: do this if just redrawing overlay
    //XCopyArea(view->x->dpy, view->pixmap2, Tk_WindowId(view->tkwin), view->x->gc, 0, 0, view->w_width, view->w_height, 0, 0);

    if (view->show_editors)
    {
        DrawMapEditorViews(view);
    }
    */

    /*
    MemDrawMap(SimView *view)
    {
        (*mapProcs[view->map_state])(view);
    }
    */

    return 1;
}
