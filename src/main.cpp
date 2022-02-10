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

#include "Font.h"
#include "Map.h"
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

#include "Texture.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


const std::string MicropolisVersion = "4.0";


std::string CityFileName;
std::string StartupName;
std::string Displays;
std::string FirstDisplay;


Sim Simulation;

int sim_skips = 0;
int sim_skip = 0;

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
    showValves();
    doTimeStuff();
    ReallyUpdateFunds();
    updateOptions();
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
    

    if (!Paused() && !heat_steps)
    {
        TilesAnimated = 0;
    }
    */

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
    SimSpeed(SimulationSpeed::Normal);
    ChangeEval();
    MessagePort = 0;
    MesX = 0;
    MesY = 0;
    
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
    SimSpeed(SimulationSpeed::Paused);
    setSkips(0);
}


constexpr SDL_Color ColorCommercial{ 79, 79, 255, 255 };
constexpr SDL_Color ColorResidential{ 0, 165, 0, 255 };
constexpr SDL_Color ColorIndustrial{ 255, 255, 0, 255 };


SDL_Window* MainWindow = nullptr;
SDL_Renderer* MainWindowRenderer = nullptr;

Texture MainMapTexture{};
Texture MiniMapTexture{};

Texture BigTileset{};
Texture SmallTileset{};
Texture RCI_Indicator{};

Font* MainFont{ nullptr };

namespace
{
    SDL_Rect MiniMapTileRect{ 0, 0, 3, 3 };
    SDL_Rect UiHeaderRect{ 10, 10, 0, 0 };
    SDL_Rect RciDestination{};
    SDL_Rect FullMapViewRect{};
    
    SDL_Rect MiniMapSelector{};
    SDL_Rect MiniMapDestination{ 10, 0, SimWidth * 3, SimHeight * 3 };
    SDL_Rect MiniMapBorder{};

    Vector<int> WindowSize{};

    Point<int> MapViewOffset{};
    Point<int> TilePointedAt{};
    Point<int> MousePosition{};

    SDL_Rect TileHighlight{ 0, 0, 16, 16 };
    SDL_Rect TileMiniHighlight{ 0, 0, 3, 3 };
};


void drawString(Font& font, std::string_view text, Point<int> position, SDL_Color color)
{
    if (text.empty()) { return; }

    //SDL_SetRenderDrawColor(MainWindowRenderer, color.r, color.g, color.b, color.a);

    SDL_SetTextureColorMod(font.texture(), color.r, color.g, color.b);

    const auto& gml = font.metrics();
    if (gml.empty()) { return; }

    int offset = 0;
    for (auto character : text)
    {
        const auto& gm = gml[std::clamp<std::size_t>((uint8_t)(character), 0, 255)];

        const auto glyphCellSize = font.glyphCellSize().to<float>();
        const auto adjustX = (gm.minX < 0) ? gm.minX : 0;

        SDL_Rect srcRect{
            static_cast<int>(gm.uvRect.x),
            static_cast<int>(gm.uvRect.y),
            static_cast<int>(glyphCellSize.x),
            static_cast<int>(glyphCellSize.y)
        };

        SDL_Rect dstRect{
            position.x + offset + adjustX,
            position.y,
            static_cast<int>(glyphCellSize.x),
            static_cast<int>(glyphCellSize.y)
        };

        SDL_RenderCopy(MainWindowRenderer, font.texture(), &srcRect, &dstRect);

        offset += gm.advance;
    }
}


void loadGraphics()
{
    BigTileset = loadTexture(MainWindowRenderer, "images/tiles.xpm");
    RCI_Indicator = loadTexture(MainWindowRenderer, "images/demandg.xpm");
    SmallTileset = loadTexture(MainWindowRenderer, "images/tilessm.xpm");
}


void DrawMiniMap()
{
    SDL_Rect miniMapDrawRect{ 0, 0, 3, 3 };

    SDL_SetRenderTarget(MainWindowRenderer, MiniMapTexture.texture);
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            miniMapDrawRect = { row * 3, col * 3, miniMapDrawRect.w, miniMapDrawRect.h };
            MiniMapTileRect.y = getTileValue(row, col) * 3;
            SDL_RenderCopy(MainWindowRenderer, SmallTileset.texture, &MiniMapTileRect, &miniMapDrawRect);
        }
    }
    SDL_RenderPresent(MainWindowRenderer);
    SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


void updateMapDrawParameters()
{
    FullMapViewRect =
    {
        MapViewOffset.x,
        MapViewOffset.y,
        WindowSize.x,
        WindowSize.y
    };

    MiniMapDestination =
    {
        MiniMapDestination.x,
        WindowSize.y - MiniMapDestination.h - 10,
        MiniMapDestination.w,
        MiniMapDestination.h
    };

    MiniMapBorder =
    {
        MiniMapDestination.x - 2,
        MiniMapDestination.y - 2,
        MiniMapDestination.w + 4,
        MiniMapDestination.h + 4
    };

    MiniMapSelector =
    {
        MiniMapDestination.x + ((MapViewOffset.x / 16) * 3),
        MiniMapDestination.y + ((MapViewOffset.y / 16) * 3),
        MiniMapSelector.w,
        MiniMapSelector.h
    };
}


void getWindowSize()
{
    SDL_GetWindowSize(MainWindow, &WindowSize.x, &WindowSize.y);;
}


void clampViewOffset()
{
    MapViewOffset =
    {
        std::clamp(MapViewOffset.x, 0, std::max(0, MainMapTexture.dimensions.x - WindowSize.x)),
        std::clamp(MapViewOffset.y, 0, std::max(0, MainMapTexture.dimensions.y - WindowSize.y))
    };
}


void setMiniMapSelectorSize()
{
    MiniMapSelector.w = static_cast<int>(std::ceil(WindowSize.x / 16.0f) * 3);
    MiniMapSelector.h = static_cast<int>(std::ceil(WindowSize.y / 16.0f) * 3);
}


void windowResized(const Vector<int>& size)
{
    getWindowSize();
    clampViewOffset();

    setMiniMapSelectorSize();

    updateMapDrawParameters();

    UiHeaderRect.w = WindowSize.x - 20;
}


void calculateMouseToWorld()
{
    Point<int> screenCell =
    {
        (((MousePosition.x) + (MapViewOffset.x % 16)) / 16) % WindowSize.x,
        (((MousePosition.y) + (MapViewOffset.y % 16)) / 16) % WindowSize.y
    };
    
    TilePointedAt =
    {
       screenCell.x + (MapViewOffset.x / 16),
       screenCell.y + (MapViewOffset.y / 16)
    };

    TileHighlight =
    {
        (screenCell.x * 16) - MapViewOffset.x % 16,
        (screenCell.y * 16) - MapViewOffset.y % 16,
        16, 16
    };

    TileMiniHighlight =
    {
        MiniMapDestination.x + TilePointedAt.x * 3,
        MiniMapDestination.y + TilePointedAt.y * 3,
        3, 3
    };
}


void handleKeyEvent(SDL_Event& event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_SPACE:
        DoPlayNewCity();
        DrawMiniMap();
        DrawBigMap();
        break;

    case SDLK_ESCAPE:
        sim_exit();
        break;

    case SDLK_0:
    case SDLK_p:
        Paused() ? Resume() : Pause();
        break;

    case SDLK_1:
        if (Paused()) { Resume(); }
        SimSpeed(SimulationSpeed::Slow);
        break;

    case SDLK_2:
        if (Paused()) { Resume(); }
        SimSpeed(SimulationSpeed::Normal);
        break;

    case SDLK_3:
        if (Paused()) { Resume(); }
        SimSpeed(SimulationSpeed::Fast);
        break;

    default:
        break;

    }
}


void pumpEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            handleKeyEvent(event);
            break;

        case SDL_MOUSEMOTION:
            MousePosition = { event.motion.x, event.motion.y };

            calculateMouseToWorld();

            if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_RMASK) != 0)
            {
                MapViewOffset -= { event.motion.xrel, event.motion.yrel };
                clampViewOffset();
                updateMapDrawParameters();
            }
            break;

        case SDL_QUIT:
            sim_exit();
            break;

        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                windowResized(Vector<int>{event.window.data1, event.window.data2});
                break;
            }
            
        default:
            break;
        }
    }
}


void initRenderer()
{
    MainWindow = SDL_CreateWindow("Micropolis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_MAXIMIZED);
    if (!MainWindow)
    {
        throw std::runtime_error("startGame(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    MainWindowRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!MainWindow)
    {
        throw std::runtime_error("startGame(): Unable to create renderer: " + std::string(SDL_GetError()));
    }
    SDL_SetRenderDrawBlendMode(MainWindowRenderer, SDL_BLENDMODE_BLEND);
}


void initViewParamters()
{
    getWindowSize();

    MiniMapTexture.texture = SDL_CreateTexture(MainWindowRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, SimWidth * 3, SimHeight * 3);
    MainMapTexture.texture = SDL_CreateTexture(MainWindowRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, SimWidth * 16, SimHeight * 16);

    MainMapTexture.dimensions = { SimWidth * 16, SimHeight * 16 };

    setMiniMapSelectorSize();

    UiHeaderRect.w = WindowSize.x - 20;
    UiHeaderRect.h = RCI_Indicator.dimensions.y + 10 + MainFont->height() + 10;

    RciDestination = { UiHeaderRect.x + 5, UiHeaderRect.y + MainFont->height() + 10, RCI_Indicator.dimensions.x, RCI_Indicator.dimensions.y };
}


void drawTopUi()
{
    // Background
    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 65);
    SDL_RenderFillRect(MainWindowRenderer, &UiHeaderRect);
    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(MainWindowRenderer, &UiHeaderRect);

    // RCI
    SDL_RenderCopy(MainWindowRenderer, RCI_Indicator.texture, nullptr, &RciDestination);

    drawString(*MainFont, MonthString(static_cast<Month>(LastCityMonth())), {UiHeaderRect.x + 5, UiHeaderRect.y + 5}, {255, 255, 255, 255});
    drawString(*MainFont, std::to_string(CurrentYear()), { UiHeaderRect.x + 30, UiHeaderRect.y + 5}, {255, 255, 255, 255});
}


void drawMiniMapUi()
{
    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 65);
    SDL_RenderFillRect(MainWindowRenderer, &MiniMapBorder);

    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(MainWindowRenderer, &MiniMapBorder);

    SDL_RenderCopy(MainWindowRenderer, MiniMapTexture.texture, nullptr, &MiniMapDestination);

    SDL_SetRenderDrawColor(MainWindowRenderer, 255, 255, 255, 150);
    SDL_RenderDrawRect(MainWindowRenderer, &MiniMapSelector);
    SDL_RenderDrawRect(MainWindowRenderer, &TileMiniHighlight);

    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 255);
}


void drawDebug()
{
    drawString(*MainFont, "Mouse Coords: " + std::to_string(MousePosition.x) + ", " + std::to_string(MousePosition.y), { 10, 100 }, { 255, 255, 255, 100 });
    drawString(*MainFont, "Tile Pick Coords: " + std::to_string(TilePointedAt.x) + ", " + std::to_string(TilePointedAt.y), { 10, 100 + MainFont->height() }, { 255, 255, 255, 100 });
    drawString(*MainFont, "CityTime: " + std::to_string(CityTime), { 10, 100 + MainFont->height() * 2 }, { 255, 255, 255, 100 });
}


std::array<unsigned int, 4> SpeedModifierTable{ 0, 0, 10, 16 };

unsigned int speedModifier()
{
    return SpeedModifierTable[static_cast<unsigned int>(SimSpeed())];
}

unsigned int currentTick{};
unsigned int lastTick{};
unsigned int accumulator{};


void startGame()
{
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    initRenderer();

    loadGraphics();

    MainFont = new Font("res/open-sans-medium.ttf", 12);

    initViewParamters();
    updateMapDrawParameters();

    sim_init();

    InitGame();

    Startup = -1;

    GameStarted();
        
    DrawMiniMap();
    DrawBigMap();

    bool nextTick{ false };

    unsigned int accumulatorAdjust{};

    while (!Exit)
    {
        lastTick = currentTick;
        currentTick = SDL_GetTicks();

        accumulatorAdjust = 20 - speedModifier();
        accumulator += currentTick - lastTick;
        if (accumulator > accumulatorAdjust)//* speedModifier())
        {
            accumulator -= accumulatorAdjust;
            nextTick = true;
        }
        sim_loop(nextTick);
        nextTick = false;

        pumpEvents();

        SDL_RenderClear(MainWindowRenderer);

        // Map
        SDL_RenderCopy(MainWindowRenderer, MainMapTexture.texture, &FullMapViewRect, nullptr);

        drawTopUi();
        drawMiniMapUi();

        SDL_SetRenderDrawColor(MainWindowRenderer, 255, 255, 255, 100);
        SDL_RenderFillRect(MainWindowRenderer, &TileHighlight);
        
        drawDebug();
        
        SDL_RenderPresent(MainWindowRenderer);
    }

    SDL_DestroyTexture(BigTileset.texture);
    SDL_DestroyTexture(RCI_Indicator.texture);

    delete MainFont;

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
