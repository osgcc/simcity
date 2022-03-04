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

#include "Graph.h"

#include "s_alloc.h"


#include <algorithm>

/* Graph Histories */
constexpr auto RES_HIST = 0;
constexpr auto COM_HIST = 1;
constexpr auto IND_HIST = 2;
constexpr auto MONEY_HIST = 3;
constexpr auto CRIME_HIST = 4;
constexpr auto POLLUTION_HIST = 5;

constexpr auto HISTORIES = 6;
constexpr auto ALL_HISTORIES = ((1 * 64) - 1);

int NewGraph = 0;
int AllMax;

unsigned char* History10[HISTORIES]{};
unsigned char* History120[HISTORIES]{};

bool HistoryInitialized{ false };
int Graph10Max{};
int Graph120Max{};
int GraphUpdateTime{ 100 };


// Hist == histogram?
const char* HistName[] =
{
    "Residential", "Commercial", "Industrial",
    "Cash Flow", "Crime", "Pollution"
};


unsigned char HistColor[] =
{
    COLOR_LIGHTGREEN, COLOR_DARKBLUE, COLOR_YELLOW,
    COLOR_DARKGREEN, COLOR_RED, COLOR_OLIVE
};


/*
static void DisplaySimGraph(ClientData clientData)
{
  Graph *graph = (Graph *) clientData;
  Tk_Window tkwin = graph->tkwin;
  Pixmap pm = None;
  Drawable d;

  graph->flags &= ~VIEW_REDRAW_PENDING;

//fprintf(stderr, "DisplaySimGraph token %d\n", graph->draw_graph_token);

  assert(graph->draw_graph_token != 0);

  if (graph->draw_graph_token != 0) {
//    Tk_DeleteTimerHandler(graph->draw_graph_token);
    graph->draw_graph_token = 0;
  }

  if (graph->visible && (tkwin != NULL) && Tk_IsMapped(tkwin)) {
    DoUpdateGraph(graph);
  }
}
*/


/*
void DestroySimGraph(ClientData clientData)
{
  Graph *graph = (Graph *) clientData;

  DestroyGraph(graph);
}
*/



void EventuallyRedrawGraph(Graph* graph)
{
    /*
    if (!(graph->flags & VIEW_REDRAW_PENDING))
    {
        assert(graph->draw_graph_token == 0);

        if (graph->draw_graph_token == 0)
        {
            graph->draw_graph_token = Tk_CreateTimerHandler(GraphUpdateTime, DisplaySimGraph, (ClientData)graph);
            graph->flags |= VIEW_REDRAW_PENDING;
            fprintf(stderr, "EventuallyRedrawGraph token %d\n", graph->draw_graph_token);
        }
    }
    */
}



/*
void SimGraphEventProc(ClientData clientData, XEvent *eventPtr)
{
  Graph *graph = (Graph *) clientData;

  if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
    graph->visible = 1;
    EventuallyRedrawGraph(graph);
  } else if (eventPtr->type == MapNotify) {
    graph->visible = 1;
  } else if (eventPtr->type == UnmapNotify) {
    graph->visible = 0;
  } else if (eventPtr->type == VisibilityNotify) {
    if (eventPtr->xvisibility.state == VisibilityFullyObscured)
      graph->visible = 0;
    else
      graph->visible = 1;
  } else if (eventPtr->type == ConfigureNotify) {
    DoResizeGraph(graph,
		  eventPtr->xconfigure.width,
		  eventPtr->xconfigure.height);
    EventuallyRedrawGraph(graph);
  } else if (eventPtr->type == DestroyNotify) {
    Tcl_DeleteCommand(graph->interp, Tk_PathName(graph->tkwin));
    graph->tkwin = NULL;
    if (graph->flags & VIEW_REDRAW_PENDING) {
fprintf(stderr, "SimGraphEventProc Destroy token %d\n", graph->draw_graph_token);
      assert(graph->draw_graph_token != 0);
      if (graph->draw_graph_token != 0) {
	Tk_DeleteTimerHandler(graph->draw_graph_token);
	graph->draw_graph_token = 0;
      }
      graph->flags &= ~VIEW_REDRAW_PENDING;
    }
   Tk_EventuallyFree((ClientData) graph, DestroySimGraph);
  }
}
*/

/*
int DoGraphCmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  Graph *graph = (Graph *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if (ent = Tcl_FindHashEntry(&GraphCmds, argv[1])) {
    cmd = (int (*)())ent->clientData;
    Tk_Preserve((ClientData) graph);
    result = cmd(graph, interp, argc, argv);
    Tk_Release((ClientData) graph);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}
*/

/*
int GraphViewCmd(ClientData clientData, Tcl_Interp* interp, int argc, char** argv)
{
    Graph* graph;
    Tk_Window tkwin = (Tk_Window)clientData;

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args:  should be \"",
            argv[0], " pathName ?options?\"", (char*)NULL);
        return TCL_ERROR;
    }

    tkwin = Tk_CreateWindowFromPath(interp, tkwin,
        argv[1], (char*)NULL);
    if (tkwin == NULL) {
        return TCL_ERROR;
    }

    graph = (Graph*)malloc(sizeof(Graph));

    graph->tkwin = tkwin;
    graph->interp = interp;
    graph->flags = 0;

    Tk_SetClass(graph->tkwin, "GraphView");
    Tk_CreateEventHandler(graph->tkwin,
        VisibilityChangeMask |
        ExposureMask |
        StructureNotifyMask,
        SimGraphEventProc, (ClientData)graph);
    Tcl_CreateCommand(interp, Tk_PathName(graph->tkwin),
        DoGraphCmd, (ClientData)graph, (void (*)()) NULL);

    Tk_MakeWindowExist(graph->tkwin);

    if (getenv("XSYNCHRONIZE") != NULL)
    {
        XSynchronize(Tk_Display(tkwin), 1);
    }

    InitNewGraph(graph);
    DoNewGraph(graph);

    if (ConfigureSimGraph(interp, graph, argc - 2, argv + 2, 0) != TCL_OK)
    {
        //* XXX: destroy graph
        Tk_DestroyWindow(graph->tkwin);
        return TCL_ERROR;
    }

    interp->result = Tk_PathName(graph->tkwin);
    return TCL_OK;
}
*/

/*
int ConfigureSimGraph(Tcl_Interp* interp, Graph* graph, int argc, char** argv, int flags)
{
    if (Tk_ConfigureWidget(interp, graph->tkwin, GraphConfigSpecs, argc, argv, (char*)graph, flags) != TCL_OK)
    {
        return TCL_ERROR;
    }

    Tk_SetBackgroundFromBorder(graph->tkwin, graph->border);

    EventuallyRedrawGraph(graph);
    return TCL_OK;
}
*/


void drawMonth(const std::array<int, HistoryLength>& history, unsigned char* s, float scale)
{
    /*
    for (int x = 0; x < 120; x++)
    {
        s[119 - x] = std::clamp(static_cast<int>(history[x] * scale), 0, 255);
    }
    */
}


void doAllGraphs()
{
    AllMax = 0;

    if (ResHisMax > AllMax) { AllMax = ResHisMax; }
    if (ComHisMax > AllMax) { AllMax = ComHisMax; }
    if (IndHisMax > AllMax) { AllMax = IndHisMax; }
    if (AllMax <= 128) { AllMax = 0; }

    float scaleValue = AllMax != 0 ? scaleValue = 128.0f / AllMax : scaleValue = 1.0f;

    // scaleValue = 0.5; // XXX

    drawMonth(ResHis, History10[RES_HIST], scaleValue);
    drawMonth(ComHis, History10[COM_HIST], scaleValue);
    drawMonth(IndHis, History10[IND_HIST], scaleValue);
    //drawMonth(MoneyHis, History10[MONEY_HIST], 1.0);
    //drawMonth(CrimeHis, History10[CRIME_HIST], 1.0);
    //drawMonth(PollutionHis, History10[POLLUTION_HIST], 1.0);

    AllMax = 0;
    if (Res2HisMax > AllMax) { AllMax = Res2HisMax; }
    if (Com2HisMax > AllMax) { AllMax = Com2HisMax; }
    if (Ind2HisMax > AllMax) { AllMax = Ind2HisMax; }
    if (AllMax <= 128) { AllMax = 0; }

    scaleValue = AllMax != 0 ? scaleValue = 128.0f / AllMax : scaleValue = 1.0f;

    // scaleValue = 0.5; // XXX

    //drawMonth(ResHis + 120, History120[RES_HIST], scaleValue);
    //drawMonth(ComHis + 120, History120[COM_HIST], scaleValue);
    //drawMonth(IndHis + 120, History120[IND_HIST], scaleValue);
    //drawMonth(MoneyHis + 120, History120[MONEY_HIST], 1.0);
    //drawMonth(CrimeHis + 120, History120[CRIME_HIST], 1.0);
    //drawMonth(PollutionHis + 120, History120[POLLUTION_HIST], 1.0);
}


static int CensusChanged;

void ChangeCensus()
{
    CensusChanged = 1;
}


void graphDoer()
{
    if (CensusChanged)
    {
        doAllGraphs();
        NewGraph = 1;
        CensusChanged = 0;
    }

    if (NewGraph)
    {
        /*
        for (Graph* graph = sim->graph; graph != NULL; graph = graph->next)
        {
            EventuallyRedrawGraph(graph);
        }
        */
        NewGraph = 0;
    }
}


void initGraphs()
{
    /*
    for (Graph* graph = sim->graph; graph != NULL; graph = graph->next)
    {
        graph->range = 10;
        graph->mask = ALL_HISTORIES;
    }

    if (!HistoryInitialized)
    {
        HistoryInitialized = 1;
        for (int i = 0; i < HISTORIES; i++)
        {
            History10[i] = (unsigned char*)malloc(120);
            History120[i] = (unsigned char*)malloc(120);
        }
    }
    */
}


void InitGraphMax()
{
    ResHisMax = 0;
    ComHisMax = 0;
    IndHisMax = 0;
    for (int x = 118; x >= 0; x--)
    {
        if (ResHis[x] > ResHisMax) { ResHisMax = ResHis[x]; }
        if (ComHis[x] > ComHisMax) { ComHisMax = ComHis[x]; }
        if (IndHis[x] > IndHisMax) { IndHisMax = IndHis[x]; }
        if (ResHis[x] < 0) { ResHis[x] = 0; }
        if (ComHis[x] < 0) { ComHis[x] = 0; }
        if (IndHis[x] < 0) { IndHis[x] = 0; }
    }

    Graph10Max = ResHisMax;

    if (ComHisMax > Graph10Max) { Graph10Max = ComHisMax; }
    if (IndHisMax > Graph10Max) { Graph10Max = IndHisMax; }

    Res2HisMax = 0;
    Com2HisMax = 0;
    Ind2HisMax = 0;

    for (int x = 238; x >= 120; x--)
    {
        if (ResHis[x] > Res2HisMax) { Res2HisMax = ResHis[x]; }
        if (ComHis[x] > Com2HisMax) { Com2HisMax = ComHis[x]; }
        if (IndHis[x] > Ind2HisMax) { Ind2HisMax = IndHis[x]; }
        if (ResHis[x] < 0) { ResHis[x] = 0; }
        if (ComHis[x] < 0) { ComHis[x] = 0; }
        if (IndHis[x] < 0) { IndHis[x] = 0; }
    }

    Graph120Max = Res2HisMax;

    if (Com2HisMax > Graph120Max) { Graph120Max = Com2HisMax; }
    if (Ind2HisMax > Graph120Max) { Graph120Max = Ind2HisMax; }
}


void DoResizeGraph(Graph* graph, int w, int h)
{
    /*
    int resize = 0;

    graph->w_width = w; graph->w_height = h;

    if (graph->pixmap != None)
    {
        XFreePixmap(graph->x->dpy, graph->pixmap);
        graph->pixmap = None;
    }

    graph->pixmap = XCreatePixmap(graph->x->dpy, graph->x->root, w, h, graph->x->depth);

    if (graph->pixmap == None)
    {
        fprintf(stderr, "Sorry, Micropolis can't create a pixmap on X display \"%s\".\n", graph->x->display);
        sim_exit(1); // Just sets tkMustExit and ExitReturn
        return;
    }
    */
}


void DoNewGraph(Graph* graph)
{
    /*
    sim->graphs++;
    graph->next = sim->graph;
    sim->graph = graph;

    NewGraph = 1;
    */
}


void InitNewGraph(Graph* graph)
{
    /*
    int d = 8;
    struct XDisplay* xd;

    graph->next = NULL;
    graph->range = 10;
    graph->mask = ALL_HISTORIES;

    // This stuff was initialized in our caller (GraphCmd)
    //graph->tkwin = NULL;
    //graph->interp = NULL;
    //graph->flags = 0;

    graph->x = NULL;
    graph->visible = 0;
    graph->w_x = graph->w_y = 0;
    graph->w_width = graph->w_height = 0;
    graph->pixmap = None;
    graph->pixels = NULL;
    graph->fontPtr = NULL;
    graph->border = NULL;
    graph->borderWidth = 0;
    graph->relief = TK_RELIEF_FLAT;
    graph->draw_graph_token = 0;

    //fprintf(stderr, "InitNewGraph token %d\n", graph->draw_graph_token);

    graph->x = FindXDisplay(graph->tkwin);
    IncRefDisplay(graph->x);

    graph->pixels = graph->x->pixels;
    graph->fontPtr = NULL;
    */

    DoResizeGraph(graph, 16, 16);
}


#define BORDER 5

void DoUpdateGraph(Graph* graph)
{
    /*
    Display* dpy;
    GC gc;
    Pixmap pm;
    int* pix;
    unsigned char** hist;
    int w, h, mask, i, j, x, y;
    XPoint points[121];
    int year = (CityTime / 48) + StartingYear;
    int month = (CityTime / 4) % 12;
    int do_top_labels = 0;
    int do_right_labels = 0;
    int top_label_height = 30;
    int right_label_width = 65;
    int tx, ty;
    float sx, sy;

    if (!graph->visible)
    {
        return;
    }

    if (graph->range == 10)
    {
        hist = History10;
    }
    else
    {
        hist = History120;
    }

    dpy = graph->x->dpy;
    gc = graph->x->gc;
    pm = graph->pixmap;
    pix = graph->pixels;

    w = graph->w_width;
    h = graph->w_height;

    XSetFont(graph->x->dpy, graph->x->gc, graph->fontPtr->fid);
    XSetLineAttributes(dpy, gc, 3, LineSolid, CapButt, JoinBevel);

    if (graph->x->color)
    {
        XSetForeground(dpy, gc, pix[COLOR_LIGHTGRAY]);
    }
    else
    {
        XSetForeground(dpy, gc, pix[COLOR_WHITE]);
    }

    XFillRectangle(dpy, pm, gc, 0, 0, w, h);

    tx = BORDER; ty = BORDER;

    if ((w -= (2 * BORDER)) < 1) w = 1;
    if ((h -= (2 * BORDER)) < 1) h = 1;

    if (w > (4 * right_label_width))
    {
        w -= right_label_width;
        do_right_labels = 1;
    }

    if (do_right_labels && (h > (3 * top_label_height)))
    {
        ty += top_label_height;
        h -= top_label_height;
        do_top_labels = 1;
    }

    sx = ((float)w) / 120.0; sy = ((float)h) / 256.0;

    mask = graph->mask;
    for (i = 0; i < HISTORIES; i++, mask >>= 1, hist++)
    {
        if (mask & 1)
        {
            int fg = COLOR_WHITE;
            int bg = COLOR_BLACK;
            Pixmap stipple = None;

            for (j = 0; j < 120; j++)
            {
                x = tx + (j * sx);
                y = ty + ((int)(h - (((float)(*hist)[j]) * sy)));
                points[j].x = x; points[j].y = y;
            }
            x = tx + (j * sx);
            points[j].x = x; points[j].y = y;

            if (graph->x->color)
            {
                XSetForeground(dpy, gc, pix[HistColor[i]]);
            }
            else
            {
                switch (i)
                {
                case 0: // res
                    stipple = graph->x->gray50_stipple;
                    break;

                case 1: // com
                    stipple = graph->x->gray25_stipple;
                    break;

                case 2: // ind
                    stipple = graph->x->gray75_stipple;
                    break;

                case 3: // cash
                    fg = COLOR_BLACK;
                    break;

                case 4: // crime
                    stipple = graph->x->horiz_stipple;
                    break;

                case 5: // pol
                    stipple = graph->x->vert_stipple;
                    break;
                }

                if (stipple != None)
                {
                    XSetStipple(graph->x->dpy, gc, stipple);
                    XSetTSOrigin(graph->x->dpy, gc, 0, 0);
                    XSetForeground(graph->x->dpy, gc, pix[fg]);
                    XSetBackground(graph->x->dpy, gc, pix[bg]);
                    XSetFillStyle(graph->x->dpy, gc, FillOpaqueStippled);
                }
                else
                {
                    XSetForeground(graph->x->dpy, gc, pix[fg]);
                }
            }

            XDrawLines(dpy, pm, gc, points, 121, CoordModeOrigin);

            if (!graph->x->color && (stipple != None))
            {
                XSetFillStyle(graph->x->dpy, gc, FillSolid);
            }

            if (do_right_labels)
            {
                if (graph->x->color)
                {
                    XSetForeground(dpy, gc, pix[HistColor[i]]);
                    XDrawString(graph->x->dpy, pm, graph->x->gc, x + 4, y + 5, HistName[i], strlen(HistName[i]));
                    XDrawString(graph->x->dpy, pm, graph->x->gc, x + 5, y + 4, HistName[i], strlen(HistName[i]));

                    XSetForeground(dpy, gc, pix[COLOR_BLACK]);
                    XDrawString(graph->x->dpy, pm, graph->x->gc, x + 5, y + 5, HistName[i], strlen(HistName[i]));
                }
                else
                {
                    XSetForeground(dpy, gc, pix[COLOR_BLACK]);
                    XDrawString(graph->x->dpy, pm, graph->x->gc, x + 5, y + 5, HistName[i], strlen(HistName[i]));
                }
            }
        }
    }

    XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt, JoinMiter);

    XSetForeground(dpy, gc, pix[COLOR_BLACK]);
    XDrawLine(dpy, pm, gc, tx, ty - 1, tx + w, ty - 1);
    XDrawLine(dpy, pm, gc, tx, ty + h, tx + w, ty + h);

    if (graph->range == 10)
    {
        for (x = 120 - month; x >= 0; x -= 12)
        {
            int xx, yy;
            xx = tx + (x * sx);
            XDrawLine(dpy, pm, gc, xx, ty - 1, xx, ty + h);

            if (do_top_labels)
            {
                char buf[256];

                sprintf(buf, "%d", year--);
                xx = tx + (x * sx) + 2;
                yy = ty - ((year & 1) ? 4 : 20);
                XDrawString(graph->x->dpy, pm, graph->x->gc, xx, yy, buf, strlen(buf));
            }
        }
    }
    else
    {
        int past;

        sx /= 10;
        past = 10 * (year % 10);
        year /= 10;

        for (x = 1200 - past; x >= 0; x -= 120)
        {
            int xx, yy;
            xx = tx + (x * sx);
            XDrawLine(dpy, pm, gc, xx, ty - 1, xx, ty + h);
            if (do_top_labels)
            {
                char buf[256];

                sprintf(buf, "%d0", year--);

                xx = tx + (x * sx) + 2;
                yy = ty - ((year & 1) ? 4 : 20);
                XDrawString(graph->x->dpy, pm, graph->x->gc, xx, yy, buf, strlen(buf));
            }
        }
    }

    XCopyArea(graph->x->dpy, graph->pixmap, Tk_WindowId(graph->tkwin), graph->x->gc, 0, 0, graph->w_width, graph->w_height, 0, 0);
    */
}
