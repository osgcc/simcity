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
#include "w_map.h"

namespace
{
    MiniMapOverlay miniMapOverlay{ MiniMapOverlay::All };
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
