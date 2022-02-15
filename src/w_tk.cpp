/* w_tk.c
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
