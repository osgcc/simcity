/* sim.c
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

#include "g_map.h"

#include "s_alloc.h"
#include "s_gen.h"
#include "s_init.h"
#include "s_sim.h"

#include "Simulation.h"
#include "Sprite.h"

#include "w_budget.h"
#include "w_editor.h"
#include "w_eval.h"
#include "w_graph.h"
#include "w_map.h"
#include "w_sound.h"
#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"
#include "w_x.h"

#include <iostream>
#include <stdexcept>
#include <string>


#include <SDL2/SDL.h>


const std::string MicropolisVersion = "4.0";


std::string CityFileName;
std::string StartupName;
std::string Displays;
std::string FirstDisplay;


Sim Simulation;

int sim_loops = 0;
int sim_delay = 50;
int sim_skips = 0;
int sim_skip = 0;
int sim_paused = 0;
int sim_paused_speed = 3;
int heat_steps = 0;
int heat_flow = -7;
int heat_rule = 0;
int heat_wrap = 3;
//struct timeval now_time;
int Startup = 0;
int StartupGameLevel = 0;
int WireMode = 0;
int MultiPlayerMode = 0;
int TilesAnimated = 0;
int DoAnimation = 1;
int DoMessages = 1;
int DoNotices = 1;
bool Exit = false;


void sim_exit()
{
    Exit = true;
}


void sim_really_exit(int val)
{
  DoStopMicropolis();

  exit(val);
}


/**
 * Init filesystem environment/path
 */
void env_init()
{
    // todo: set up search paths here
}


void signal_init()
{
}


void sim_update_editors()
{
    /*
    for (SimView* view = Simulation.editor; view != nullptr; view = view->next)
    {
        CancelRedrawView(view);
        view->invalid = 1;
        DoUpdateEditor(view);
    }
    */
    DoUpdateHeads();
}


void sim_update_maps()
{
    /*
    for (SimView* view = Simulation.map; view != nullptr; view = view->next)
    {
        bool mustUpdateMap = NewMapFlags[view->map_state] || NewMap || ShakeNow;
        if (mustUpdateMap)
        {
            view->invalid = 1;
        }
        if (view->invalid)
        {
            if (mustUpdateMap)
            {
                //fprintf(stderr, "sim_update_maps mustUpdateMap\n");
                view->skip = 0;
            }
            if (DoUpdateMap(view))
            {
                CancelRedrawView(view);
                view->invalid = 1;
            }
        }
    }
    */

    NewMap = false;
    for (int i = 0; i < NMAPS; i++)
    {
        NewMapFlags[i] = 0;
    }
}


void sim_update_graphs()
{
  graphDoer();
}


void sim_update_budgets()
{
    if ((sim_skips != 0) && (sim_skip != 0))
    {
        return;
    }

    UpdateBudgetWindow();
}


void sim_update_evaluations()
{
    if ((sim_skips != 0) && (sim_skip != 0))
    {
        return;
    }

    scoreDoer();
}


int *CellSrc = nullptr;
int *CellDst = nullptr;

#define SRCCOL (SimHeight + 2)
#define DSTCOL SimHeight

#define CLIPPER_LOOP_BODY(CODE) \
    src = CellSrc; dst = CellDst; \
    for (int x = 0; x < SimWidth;) { \
      int nw, n, ne, w, c, e, sw, s, se; \
      \
      src = CellSrc + (x * SRCCOL); dst = CellDst + (x * DSTCOL); \
      w = src[0]; c = src[SRCCOL]; e = src[2 * SRCCOL]; \
      sw = src[1]; s = src[SRCCOL + 1]; se = src[(2 * SRCCOL) + 1]; \
      \
      for (int y = 0; y < SimHeight; y++) { \
        nw = w; w = sw; sw = src[2]; \
	n = c; c = s; s = src[SRCCOL + 2]; \
	ne = e; e = se; se = src[(2 * SRCCOL) + 2]; \
	{ CODE } \
	src++; dst++; \
      } \
      x++; \
      src = CellSrc + ((x + 1) * SRCCOL) - 3; dst = CellDst + ((x + 1) * DSTCOL) - 1; \
      \
      nw = src[1]; n = src[SRCCOL + 1]; ne = src[(2 * SRCCOL) + 1]; \
      w = src[2]; c = src[SRCCOL + 2]; e = src[(2 * SRCCOL) + 2]; \
      \
      for (int y = SimHeight - 1; y >= 0; y--) { \
        sw = w; w = nw; nw = src[0]; \
        s = c; c = n; n = src[SRCCOL]; \
        se = e; e = ne; ne = src[2 * SRCCOL]; \
	{ CODE } \
	src--; dst--; \
      } \
      x++; /* src += SRCCOL + 3; dst += DSTCOL + 1; */ \
    }

constexpr auto ECOMASK = 0x3fc;

void sim_heat()
{
    static int a = 0;
    int* src, * dst;
    int fl = heat_flow;

    if (CellSrc == nullptr)
    {
        CellSrc = (int*)malloc((SimWidth + 2) * (SimHeight + 2) * sizeof(int));
        CellDst = &Map[0][0];
    }

    src = CellSrc + SRCCOL + 1;
    dst = CellDst;

    /*
     * Copy wrapping edges:
     *
     *	0	ff	f0 f1 ... fe ff		f0
     *
     *	1	0f	00 01 ... 0e 0f		00
     *	2	1f	10 11 ... 1e 1f		10
     *		..	.. ..     .. ..		..
     *		ef	e0 e1 ... ee ef		e0
     *	h	ff	f0 f1 ... fe ff		f0
     *
     *	h+1	0f	00 01 ... 0e 0f		00
     *
     * wrap value:	effect:
     *	0	no effect
     *	1	copy future=>past, no wrap
     *	2	no copy, wrap edges
     *	3	copy future=>past, wrap edges
     *	4	copy future=>past, same edges
     */

    switch (heat_wrap)
    {
    case 0:
        break;
    case 1:
        for (int x = 0; x < SimWidth; x++)
        {
            memcpy(src, dst, SimHeight * sizeof(int));
            src += SRCCOL;
            dst += DSTCOL;
        }
        break;
    case 2:
        for (int x = 0; x < SimWidth; x++)
        {
            src[-1] = src[SimHeight - 1];
            src[SimHeight] = src[0];
            src += SRCCOL;
            dst += DSTCOL;
        }
        memcpy(CellSrc, CellSrc + (SRCCOL * SimWidth), SRCCOL * sizeof(int));
        memcpy(CellSrc + SRCCOL * (SimWidth + 1), CellSrc + SRCCOL, SRCCOL * sizeof(int));
        break;
    case 3:
        for (int x = 0; x < SimWidth; x++)
        {
            memcpy(src, dst, SimHeight * sizeof(int));
            src[-1] = src[SimHeight - 1];
            src[SimHeight] = src[0];
            src += SRCCOL;
            dst += DSTCOL;
        }
        memcpy(CellSrc, CellSrc + (SRCCOL * SimWidth), SRCCOL * sizeof(int));
        memcpy(CellSrc + SRCCOL * (SimWidth + 1), CellSrc + SRCCOL, SRCCOL * sizeof(int));
        break;
    case 4:
        src[0] = dst[0];
        src[1 + SimHeight] = dst[SimHeight - 1];
        src[(1 + SimWidth) * SRCCOL] = dst[(SimWidth - 1) * DSTCOL];
        src[((2 + SimWidth) * SRCCOL) - 1] = dst[(SimWidth * SimHeight) - 1];
        for (int x = 0; x < SimWidth; x++)
        {
            memcpy(src, dst, SimHeight * sizeof(int));
            src[-1] = src[0];
            src[SimHeight] = src[SimHeight - 1];
            src += SRCCOL;
            dst += DSTCOL;
        }
        memcpy(CellSrc + (SRCCOL * (SimWidth + 1)), CellSrc + (SRCCOL * SimWidth), SRCCOL * sizeof(int));
        memcpy(CellSrc, CellSrc + SRCCOL, SRCCOL * sizeof(int));
        break;
    }


    switch (heat_rule)
    {

    case 0:
#define HEAT \
	a += nw + n + ne + w + e + sw + s + se + fl; \
	dst[0] = ((a >> 3) & LOMASK) | \
		     (ANIMBIT | BURNBIT | BULLBIT); \
	a &= 7;

        CLIPPER_LOOP_BODY(HEAT);
        break;

    case 1:
#define ECO \
      c -= fl; n -= fl; s -= fl; e -= fl; w -= fl; \
      ne -= fl; nw -= fl; se -= fl; sw -= fl; \
      \
      /* anneal */ \
      { int sum = (c&1) + (n&1) + (s&1) + (e&1) + (w&1) + \
		  (ne&1) + (nw&1) + (se&1) + (sw&1), cell; \
	if (((sum > 5) || (sum == 4))) { \
	  /* brian's brain */ \
	  cell = ((c <<1) & (0x3fc)) | \
		 (((((c >>1)&3) == 0) && \
		   (((n&2) + (s&2) + (e&2) + (w&2) + \
		     (ne&2) + (nw&2) + (se&2) + (sw&2)) == (2 <<1)) \
		  ) ? 2 : 0) | \
		 1; \
	} else { \
	  /* anti-life */ \
	  sum = ((n&2) + (s&2) + (e&2) + (w&2) + \
		 (ne&2) + (nw&2) + (se&2) + (sw&2)) >>1; \
	  cell = (((c ^ 2) <<1) & ECOMASK) | \
		 ((c&2) ? ((sum != 5) ? 2 : 0) \
			: (((sum != 5) && (sum != 6)) ? 2 : 0)); \
	} \
	dst[0] = ((fl + cell) & LOMASK) | \
		 (ANIMBIT | BURNBIT | BULLBIT); \
      } \
      c += fl; n += fl; s += fl; e += fl; w += fl; \
      ne += fl; nw += fl; se += fl; sw += fl;

        CLIPPER_LOOP_BODY(ECO);
        break;
    }
}



void sim_update()
{
    /* -- blink speed of 0.5 seconds
    gettimeofday(&now_time, nullptr);
    flagBlink = (now_time.tv_usec < 500000) ? 1 : -1;
    */

    if (SimSpeed && !heat_steps)
    {
        TilesAnimated = 0;
    }

    sim_update_editors();

    sim_update_maps();
    sim_update_graphs();
    sim_update_budgets();
    sim_update_evaluations();

    UpdateFlush();
}


void sim_loop(bool doSim)
{

    if (heat_steps)
    {
        int j;

        for (j = 0; j < heat_steps; j++)
        {
            sim_heat();
        }

        MoveObjects();
        /*
              InvalidateMaps();
        */
        NewMap = true;
    }
    else
    {
        if (doSim)
        {
            SimFrame();
        }
        MoveObjects();
    }

    sim_loops++;
    sim_update();
}


void sim_init()
{
    signal_init();

    userSoundOn(true);

    MustUpdateOptions = 1;
    HaveLastMessage = false;
    ScenarioID = 0;
    StartingYear = 1900;
    tileSynch = 0x01;
    sim_skips = sim_skip = 0;
    AutoGo = true;
    CityTax = 7;
    CityTime = 50;
    NoDisasters = 0;
    PunishCnt = 0;
    autoBulldoze = 1;
    autoBudget = 1;
    MesNum = 0;
    LastMesTime = 0;
    flagBlink = 1;
    SimSpeed = 3;
    ChangeEval();
    MessagePort = 0;
    MesX = 0;
    MesY = 0;
    sim_paused = 0;
    sim_loops = 0;
    InitSimLoad = 2;
    Exit = 0;

    InitializeSound();
    initMapArrays();
    initGraphs();
    InitFundingLevel();
    setUpMapProcs();
    StopEarthquake();
    ResetMapState();
    ResetEditorState();
    ClearMap();
    InitWillStuff();
    SetFunds(5000);
    SetGameLevelFunds(StartupGameLevel);
    setSpeed(0);
    setSkips(0);
}


SDL_Window* MainWindow = nullptr;
SDL_Renderer* MainWindowRenderer = nullptr;


SDL_Texture* TilesetTexture = nullptr;

#include "g_bigmap.h"
#include "w_sound.h"

void startGame()
{
    MainWindow = SDL_CreateWindow("Micropolis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_MAXIMIZED);
    if (!MainWindow)
    {
        throw std::runtime_error("tk_main(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    MainWindowRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!MainWindow)
    {
        throw std::runtime_error("tk_main(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    sim_init();

    // set resource directories here

    SDL_Texture* bitmapTex = NULL;
    SDL_Surface* bitmapSurface = NULL;

    SDL_Init(SDL_INIT_VIDEO);

    bitmapSurface = SDL_LoadBMP("images/airport.bmp");
    if (!bitmapSurface)
    {
        std::cout << "Failed to load BMP: " << SDL_GetError() << std::endl;
    }

    bitmapTex = SDL_CreateTextureFromSurface(MainWindowRenderer, bitmapSurface);
    SDL_FreeSurface(bitmapSurface);


    bitmapSurface = SDL_LoadBMP("images/tiles.bmp");
    TilesetTexture = SDL_CreateTextureFromSurface(MainWindowRenderer, bitmapSurface);
    SDL_FreeSurface(bitmapSurface);


    InitGame();

    Startup = -1;

    GameStarted();

    SDL_Rect drawRect{ 0, 0, 64, 64 };
    SDL_Rect tileRect{ 0, 0, 16, 16 };


    while (!Exit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    DoPlayNewCity();
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    sim_exit();
                }
                break;

            case SDL_QUIT:
                sim_exit();
                break;
            }
        }
        SDL_RenderClear(MainWindowRenderer);
        MemDrawBeegMapRect(nullptr, 0, 0, SimWidth, SimHeight);
        SDL_RenderPresent(MainWindowRenderer);
    }

    SDL_DestroyTexture(bitmapTex);
    SDL_DestroyRenderer(MainWindowRenderer);
    SDL_DestroyWindow(MainWindow);
}


int main(int argc, char* argv[])
{
    std::cout << "Starting Micropolis-SDL2 version " << MicropolisVersion << " originally by Will Wright and Don Hopkins." << std::endl;
    std::cout << "Original code Copyright (C) 2002 by Electronic Arts, Maxis. Released under the GPL v3" << std::endl;
    std::cout << "Modifications Copyright (C) 2022 by Leeor Dicker. Available under the terms of the GPL v3" << std::endl << std::endl;
    std::cout << "Micropolis-SDL2 is not afiliated with Electronic Arts." << std::endl;

    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cout << "Unable to initialize SDL: " << SDL_GetError();
        return 1;
    }

    env_init();
    startGame();

    SDL_Quit();

    return 0;
}
