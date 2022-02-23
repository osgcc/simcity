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

#include "w_sound.h"

#include <iostream>
#include <string>

int sim_timer_idle = 0;
int sim_timer_set = 0;

int UpdateDelayed = 0;
int AutoScrollEdge = 16;
int AutoScrollStep = 16;
int AutoScrollDelay = 10;

int earthquake_timer_set = 0;
int earthquake_delay = 3000;
int PerformanceTiming;
double FlushTime;
int NeedRest = 0;


void EventuallyRedrawView(SimView* view)
{
    /*
    if (!(view->flags & VIEW_REDRAW_PENDING))
    {
        Tk_DoWhenIdle(DisplayTileView, (ClientData)view);
        view->flags |= VIEW_REDRAW_PENDING;
    }
    */
}


bool Eval(const std::string& buf)
{
    std::cout << "Eval: " << buf << std::endl;
    /*
    int result = Tcl_Eval(tk_mainInterp, buf, 0, (char**)NULL);
    if (result != TCL_OK)
    {
        char* errorinfo = Tcl_GetVar(tk_mainInterp, "errorInfo", TCL_GLOBAL_ONLY);
        if (errorinfo == NULL) errorinfo = "<no backtrace>";
        fprintf(stderr, "Micropolis: error in TCL code: %s\n%s\n", tk_mainInterp->result, errorinfo);
    }
    */

    return false;
}


void InvalidateMaps()
{
    std::cout << "InvalidateMaps" << std::endl;

    /*
    for (SimView* view = sim->map; view != NULL; view = view->next)
    {
        view->invalid = 1;
        view->skip = 0;
        EventuallyRedrawView(view);
    }
    */
    sim_skip = 0;
}


void InvalidateEditors()
{
    std::cout << "InvalidateEditors" << std::endl;

    /*
    for (SimView* view = sim->editor; view != NULL; view = view->next)
    {
        view->invalid = 1;
        view->skip = 0;
        EventuallyRedrawView(view);
    }
    */

    sim_skip = 0;
}


void RedrawMaps()
{
    std::cout << "RedrawMaps" << std::endl;

    /*
    for (SimView* view = sim->map; view != NULL; view = view->next)
    {
        view->skip = 0;
        EventuallyRedrawView(view);
    }
    */

    sim_skip = 0;
}


void RedrawEditors()
{
    std::cout << "RedrawEditors" << std::endl;

    /*
    for (SimView* view = sim->editor; view != NULL; view = view->next)
    {
        view->skip = 0;
        EventuallyRedrawView(view);
    }
    */

    sim_skip = 0;
}


void CancelRedrawView(SimView* view)
{
    /*
    if (view->flags & VIEW_REDRAW_PENDING)
    {
        Tk_CancelIdleCall(DisplayTileView, (ClientData)view);
    }
    view->flags &= ~VIEW_REDRAW_PENDING;
    */
}


void DidStopPan(SimView* view)
{
    /*
    char buf[256];
    sprintf(buf, "UIDidStopPan %s", Tk_PathName(view->tkwin));

    Eval(buf);
    */
}


void StopEarthquake()
{
    std::cout << "StopEarthquake" << std::endl;
    ShakeNow = 0;
    if (earthquake_timer_set)
    {
        //Tk_DeleteTimerHandler(earthquake_timer_token);
    }
    earthquake_timer_set = 0;
}


void DoEarthQuake()
{
    std::cout << "DoEarthQuake" << std::endl;
    MakeSound("city", "Explosion-Low");
    Eval("UIEarthQuake");
    ShakeNow++;
    if (earthquake_timer_set)
    {
        //Tk_DeleteTimerHandler(earthquake_timer_token);
    }
    //Tk_CreateTimerHandler(earthquake_delay, (void (*)())StopEarthquake, (ClientData)0);
    earthquake_timer_set = 1;
}
