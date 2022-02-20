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

#include "g_ani.h"
#include "g_map.h"

#include "s_alloc.h"
#include "s_disast.h"
#include "s_gen.h"
#include "s_init.h"
#include "s_msg.h"
#include "s_sim.h"

#include "Sprite.h"

#include "w_budget.h"
#include "w_eval.h"
#include "w_graph.h"
#include "w_map.h"
#include "w_sound.h"
#include "w_stubs.h"
#include "w_tool.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include "Texture.h"
#include "ToolPalette.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


const std::string MicropolisVersion = "4.0";


std::string CityFileName;
std::string StartupName;
std::string Displays;
std::string FirstDisplay;


int sim_skips = 0;
int sim_skip = 0;

int Startup = 0;
int StartupGameLevel = 0;
int WireMode = 0;
int MultiPlayerMode = 0;
int TilesAnimated = 0;
int DoAnimation = 1;
int DoMessages = 1;
int DoNotices = 1;
bool Exit = false;


namespace
{
    SDL_Rect MiniMapTileRect{ 0, 0, 3, 3 };
    SDL_Rect UiHeaderRect{ 10, 10, 0, 0 };
    SDL_Rect RciDestination{};
    SDL_Rect FullMapViewRect{};

    SDL_Rect MiniMapSelector{};
    SDL_Rect MiniMapDestination{ 10, 0, SimWidth * 3, SimHeight * 3 };
    SDL_Rect MiniMapBorder{};

    SDL_Rect ResidentialValveRect{ 0, 0, 4, 0 };
    SDL_Rect CommercialValveRect{ 0, 0, 4, 0 };
    SDL_Rect IndustrialValveRect{ 0, 0, 4, 0 };

    Vector<int> WindowSize{};

    Point<int> MapViewOffset{};
    Point<int> TilePointedAt{};
    Point<int> MousePosition{};
    Point<int> MouseClickPosition{};

    bool MouseClicked{};

    SDL_Rect TileHighlight{ 0, 0, 16, 16 };
    SDL_Rect TileMiniHighlight{ 0, 0, 3, 3 };

    std::array<unsigned int, 4> SpeedModifierTable{ 0, 0, 20, 37 };


    std::string currentBudget{};

    std::vector<const SDL_Rect*> UiRects{};

    namespace timing
    {
        unsigned int currentTick{};
        unsigned int lastTick{};
        unsigned int tickDelta{};

        unsigned int accumulator{};
        unsigned int accumulatorAdjust{};

        void updateTiming()
        {
            lastTick = currentTick;
            currentTick = SDL_GetTicks();
            tickDelta = currentTick - lastTick;
        }
    };
};


const Point<int>& viewOffset()
{
    return MapViewOffset;
}

constexpr SDL_Color ColorResidential{ 0, 165, 0, 255 };
constexpr SDL_Color ColorCommercial{ 79, 79, 255, 255 };
constexpr SDL_Color ColorIndustrial{ 255, 255, 0, 255 };
constexpr auto RciValveHeight = 20;


SDL_Window* MainWindow = nullptr;
SDL_Renderer* MainWindowRenderer = nullptr;

Texture MainMapTexture{};
Texture MiniMapTexture{};

Texture BigTileset{};
Texture SmallTileset{};
Texture RCI_Indicator{};

Font* MainFont{ nullptr };
Font* MainBigFont{ nullptr };


void sim_exit()
{
    Exit = true;
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
    updateDate();
    UpdateFunds();
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


void sim_update()
{
    /* -- blink speed of 0.5 seconds */
    //gettimeofday(&now_time, nullptr);

    //flagBlink = (TickCount() % 500) ? 1 : -1;
    

    sim_update_editors();

    sim_update_maps();
    sim_update_graphs();
    sim_update_budgets();
    sim_update_evaluations();
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
            MiniMapTileRect.y = maskedTileValue(tileValue(row, col)) * 3;
            SDL_RenderCopy(MainWindowRenderer, SmallTileset.texture, &MiniMapTileRect, &miniMapDrawRect);
        }
    }
    SDL_RenderPresent(MainWindowRenderer);
    SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


unsigned int animationAccumulator{};
unsigned int animationSpeed{ 125 };

bool animationTick()
{
    animationAccumulator += timing::tickDelta;
    if (animationAccumulator > animationSpeed)
    {
        animationAccumulator -= animationSpeed;
        return true;
    }

    return false;
}


void sim_loop(bool doSim)
{
    if (doSim)
    {
        SimFrame();
    }

    const int tick = TickCount();

    if (animationTick())
    {
        animateTiles();

        const Point<int> begin{ MapViewOffset.x / 16, MapViewOffset.y / 16 };
        const Point<int> end
        {
            std::clamp((MapViewOffset.x + WindowSize.x) / 16 + 1, 0, SimWidth),
            std::clamp((MapViewOffset.y + WindowSize.y) / 16 + 1, 0, SimHeight)
        };

        MoveObjects();

        DrawBigMapSegment(begin, end);
    }

    if (tick % 1000 == 0)
    {
        DrawMiniMap();
    }

    sim_update();
}


void sim_init()
{
    signal_init();

    userSoundOn(true);

    MustUpdateOptions = 1;
    ScenarioID = 0;
    StartingYear = 1900;
    tileSynch = 0x01;
    sim_skips = sim_skip = 0;
    AutoGotoMessageLocation(true);
    CityTax = 7;
    CityTime = 50;
    NoDisasters = 0;
    PunishCnt = 0;
    autoBulldoze = 1;
    autoBudget = 1;
    MessageId(NotificationId::None);
    ClearMes();
    flagBlink = true;
    SimSpeed(SimulationSpeed::Normal);
    ChangeEval();
    MessageLocation({ 0, 0 });
    
    InitSimLoad = 2;
    Exit = false;

    InitializeSound();
    initMapArrays();
    initGraphs();
    InitFundingLevel();
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


void drawString(Font& font, std::string_view text, Point<int> position, SDL_Color color)
{
    if (text.empty()) { return; }

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


void buildBigTileset()
{
    SDL_Surface* srcSurface = IMG_Load("images/tiles.xpm");
    SDL_Surface* dstSurface = SDL_CreateRGBSurface(srcSurface->flags,
        512, 512, 24,
        srcSurface->format->Rmask,
        srcSurface->format->Gmask,
        srcSurface->format->Bmask,
        srcSurface->format->Amask);

    SDL_Rect srcRect{ 0, 0, 16, 16 };
    SDL_Rect dstRect{ 0, 0, 16, 16 };

    for (int i = 0; i < TILE_COUNT; ++i)
    {
        srcRect.y = i * 16;
        dstRect = { (i % 32) * 16, (i / 32) * 16, 16, 16 };
        SDL_BlitSurface(srcSurface, &srcRect, dstSurface, &dstRect);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(MainWindowRenderer, dstSurface);

    SDL_FreeSurface(srcSurface);
    SDL_FreeSurface(dstSurface);

    if (!texture)
    {
        std::cout << SDL_GetError() << std::endl;
        throw std::runtime_error(std::string("loadTexture(): ") + SDL_GetError());
    }

    int width = 0, height = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

    BigTileset = { texture, SDL_Rect{ 0, 0, width, height }, { width, height } };
}


void loadGraphics()
{
    buildBigTileset();
    RCI_Indicator = loadTexture(MainWindowRenderer, "images/demandg.xpm");
    SmallTileset = loadTexture(MainWindowRenderer, "images/tilessm.xpm");
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
    auto screenCell = PositionToCell(MousePosition, MapViewOffset);
    
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

    case SDLK_F5:
        //MakeTornado();
        //MakeFlood();
        //MakeMeltdown();
        //MakeFire();
        break;

    default:
        break;

    }
}


void handleMouseEvent(SDL_Event& event)
{
    switch (event.type)
    {
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

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            SDL_Point mp{ event.button.x, event.button.y };
            for (auto rect : UiRects)
            {
                if (SDL_PointInRect(&mp, rect))
                {
                    return;
                }
            }

            ToolDown(TilePointedAt.x, TilePointedAt.y);
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            MouseClicked = true;
            MouseClickPosition = { event.button.x, event.button.y };
        }
        break;

    default:
        break;
    }
}


void handleWindowEvent(SDL_Event& event)
{
    switch (event.window.event)
    {
    case SDL_WINDOWEVENT_RESIZED:
        windowResized(Vector<int>{event.window.data1, event.window.data2});
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
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            handleMouseEvent(event);
            break;

        case SDL_QUIT:
            sim_exit();
            break;

        case SDL_WINDOWEVENT:
            handleWindowEvent(event);
            break;
            
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
    UiHeaderRect.h = RCI_Indicator.dimensions.y + 10 + MainBigFont->height() + 10;

    RciDestination = { UiHeaderRect.x + 5, UiHeaderRect.y + MainBigFont->height() + 10, RCI_Indicator.dimensions.x, RCI_Indicator.dimensions.y };

    ResidentialValveRect = { RciDestination.x + 9, RciDestination.y + 24, 4, 0 };
    CommercialValveRect = { RciDestination.x + 18, RciDestination.y + 24, 4, 0 };
    IndustrialValveRect = { RciDestination.x + 27, RciDestination.y + 24, 4, 0 };
}


void drawValve()
{
    double residentialPercent = static_cast<double>(RValve) / 1500.0;
    double commercialPercent = static_cast<double>(CValve) / 1500.0;
    double industrialPercent = static_cast<double>(IValve) / 1500.0;

    ResidentialValveRect.h = -static_cast<int>(RciValveHeight * residentialPercent);
    CommercialValveRect.h = -static_cast<int>(RciValveHeight * commercialPercent);
    IndustrialValveRect.h = -static_cast<int>(RciValveHeight * industrialPercent);

    SDL_SetRenderDrawColor(MainWindowRenderer, ColorResidential.r, ColorResidential.g, ColorResidential.b, ColorResidential.a);
    SDL_RenderFillRect(MainWindowRenderer, &ResidentialValveRect);

    SDL_SetRenderDrawColor(MainWindowRenderer, ColorCommercial.r, ColorCommercial.g, ColorCommercial.b, ColorCommercial.a);
    SDL_RenderFillRect(MainWindowRenderer, &CommercialValveRect);

    SDL_SetRenderDrawColor(MainWindowRenderer, ColorIndustrial.r, ColorIndustrial.g, ColorIndustrial.b, ColorIndustrial.a);
    SDL_RenderFillRect(MainWindowRenderer, &IndustrialValveRect);

    // not a huge fan of this
    SDL_Rect rciSrc{ 4, 19, 32, 11 };
    SDL_Rect rciDst{ RciDestination.x + 4, RciDestination.y + 19, 32, 11 };
    SDL_RenderCopy(MainWindowRenderer, RCI_Indicator.texture, &rciSrc, &rciDst);
}


void drawTopUi()
{
    // Background
    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 150);
    SDL_RenderFillRect(MainWindowRenderer, &UiHeaderRect);
    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(MainWindowRenderer, &UiHeaderRect);

    // RCI
    SDL_RenderCopy(MainWindowRenderer, RCI_Indicator.texture, nullptr, &RciDestination);
    drawValve();

    drawString(*MainBigFont, MonthString(static_cast<Month>(LastCityMonth())), {UiHeaderRect.x + 5, UiHeaderRect.y + 5}, {255, 255, 255, 255});
    drawString(*MainBigFont, std::to_string(CurrentYear()), { UiHeaderRect.x + 35, UiHeaderRect.y + 5}, {255, 255, 255, 255});

    drawString(*MainBigFont, LastMessage(), {100, UiHeaderRect.y + 5}, {255, 255, 255, 255});

    const Point<int> budgetPosition{ UiHeaderRect.x + UiHeaderRect.w - 5 - MainBigFont->width(currentBudget), UiHeaderRect.y + 5 };
    drawString(*MainBigFont, currentBudget, budgetPosition, { 255, 255, 255, 255 });
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


void DrawPendingTool(const ToolPalette& palette)
{
    if (palette.tool() == Tool::None)
    {
        return;
    }

    SDL_Rect toolRect =
    {
        TileHighlight.x - (Tools.at(PendingTool).offset * 16),
        TileHighlight.y - (Tools.at(PendingTool).offset * 16),
        Tools.at(PendingTool).size * 16,
        Tools.at(PendingTool).size * 16
    };

    if (palette.toolGost().texture)
    {
        SDL_RenderCopy(MainWindowRenderer, palette.toolGost().texture, &palette.toolGost().area, &toolRect);
        return;
    }

    SDL_SetRenderDrawColor(MainWindowRenderer, 255, 255, 255, 100);
    SDL_RenderFillRect(MainWindowRenderer, &toolRect);

    SDL_SetRenderDrawColor(MainWindowRenderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(MainWindowRenderer, &toolRect);
}


void drawDebug()
{
    drawString(*MainFont, "Mouse Coords: " + std::to_string(MousePosition.x) + ", " + std::to_string(MousePosition.y), { 200, 100 }, { 255, 255, 255, 100 });
    drawString(*MainFont, "Tile Pick Coords: " + std::to_string(TilePointedAt.x) + ", " + std::to_string(TilePointedAt.y), { 200, 100 + MainFont->height() }, { 255, 255, 255, 150 });
    
    drawString(*MainFont, "Speed: " + SpeedString(SimSpeed()), { 200, 100 + MainFont->height() * 3}, {255, 255, 255, 100});
    drawString(*MainFont, "CityTime: " + std::to_string(CityTime), { 200, 100 + MainFont->height() * 4 }, { 255, 255, 255, 100 });

    drawString(*MainFont, "RValve: " + std::to_string(RValve), { 200, 100 + MainFont->height() * 6 }, { 255, 255, 255, 100 });
    drawString(*MainFont, "CValve: " + std::to_string(CValve), { 200, 100 + MainFont->height() * 7 }, { 255, 255, 255, 100 });
    drawString(*MainFont, "IValve: " + std::to_string(IValve), { 200, 100 + MainFont->height() * 8 }, { 255, 255, 255, 100 });

    drawString(*MainFont, "TotalPop: " + std::to_string(TotalPop), { 200, 100 + MainFont->height() * 10 }, { 255, 255, 255, 100 });
    
    
    std::stringstream sstream;

    sstream << "Res: " << ResPop << " Com: " << ComPop << " Ind: " << IndPop << " Tot: " << TotalPop << " LastTot: " << LastTotalPop;
    drawString(*MainFont, sstream.str(), { 200, 100 + MainFont->height() * 11 }, { 255, 255, 255, 100 });

    sstream.str("");
    sstream << "TotalZPop: " << TotalZPop << " ResZ: " << ResZPop << " ComZ: " << ComZPop << " IndZ: " << IndZPop;
    drawString(*MainFont, sstream.str(), { 200, 100 + MainFont->height() * 12 }, { 255, 255, 255, 100 });

    sstream.str("");
    sstream << "PolicePop: " << PolicePop << " FireStPop: " << FireStPop;
    drawString(*MainFont, sstream.str(), { 200, 100 + MainFont->height() * 13 }, { 255, 255, 255, 100 });


}


unsigned int speedModifier()
{
    return SpeedModifierTable[static_cast<unsigned int>(SimSpeed())];
}


bool simulationTick()
{
    timing::accumulatorAdjust = 40 - speedModifier();
    timing::accumulator += timing::tickDelta;
    if (timing::accumulator > timing::accumulatorAdjust)
    {
        timing::accumulator -= timing::accumulatorAdjust;
        return true;
    }

    return false;
}


unsigned int zonePowerBlinkFlag(unsigned int interval, void* param)
{
    flagBlink = !flagBlink;
    return interval;
}


void startGame()
{
    sim_init();
    InitGame();

    Startup = -1;

    GameStarted();

    DrawMiniMap();
    DrawBigMap();

    SDL_TimerID zonePowerBlink = SDL_AddTimer(500, zonePowerBlinkFlag, nullptr);

    ToolPalette toolPalette(MainWindowRenderer);
    toolPalette.position({ UiHeaderRect.x, UiHeaderRect.y + UiHeaderRect.h + 5 });

    UiRects.push_back(&toolPalette.rect());
    UiRects.push_back(&UiHeaderRect);

    while (!Exit)
    {
        timing::updateTiming();

        PendingTool = toolPalette.tool();
        sim_loop(simulationTick());
        pumpEvents();

        currentBudget = NumberToDollarDecimal(TotalFunds());

        SDL_RenderClear(MainWindowRenderer);

        // Map
        SDL_RenderCopy(MainWindowRenderer, MainMapTexture.texture, &FullMapViewRect, nullptr);
        DrawObjects();

        DrawPendingTool(toolPalette);

        drawTopUi();

        drawMiniMapUi();

        if (MouseClicked)
        {
            MouseClicked = false;
            toolPalette.injectMouseClickPosition(MouseClickPosition);
        }
        toolPalette.draw();

        drawDebug();

        SDL_RenderPresent(MainWindowRenderer);
    }

    SDL_RemoveTimer(zonePowerBlink);
}


void cleanUp()
{
    delete MainFont;
    delete MainBigFont;

    SDL_DestroyTexture(BigTileset.texture);
    SDL_DestroyTexture(RCI_Indicator.texture);

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

    initRenderer();
    loadGraphics();

    MainFont = new Font("res/open-sans-medium.ttf", 12);
    MainBigFont = new Font("res/open-sans-medium.ttf", 14);

    initViewParamters();
    updateMapDrawParameters();

    env_init();
    startGame();

    cleanUp();

    SDL_Quit();

    return 0;
}
