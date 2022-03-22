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

#include "Budget.h"

#include "BudgetWindow.h"
#include "GraphWindow.h"

#include "CityProperties.h"
#include "FileIo.h"
#include "Font.h"
#include "Graph.h"
#include "Map.h"

#include "g_ani.h"
#include "g_map.h"

#include "s_alloc.h"
#include "s_disast.h"
#include "s_fileio.h"
#include "s_gen.h"
#include "s_init.h"
#include "s_msg.h"
#include "s_scan.h"
#include "s_sim.h"

#include "SmallMaps.h"
#include "Sprite.h"
#include "StringRender.h"

#include "w_eval.h"
#include "w_sound.h"
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


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

const std::string MicropolisVersion = "4.0";

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
    bool BudgetWindowShown{ false };
    bool Exit{ false };
    bool RedrawMinimap{ false };
    bool SimulationStep{ false };
    bool AnimationEnabled{ true };
    bool AnimationStep{ false };
    bool AutoBudget{ false };
    bool ShowGraphWindow{ false };

    constexpr unsigned int SimStepDefaultTime{ 100 };
    constexpr unsigned int AnimationStepDefaultTime{ 150 };

    SDL_Rect TileHighlight{ 0, 0, 16, 16 };
    SDL_Rect TileMiniHighlight{ 0, 0, 3, 3 };

    std::array<unsigned int, 5> SpeedModifierTable{ 0, 0, 50, 75, 95 };

    std::string currentBudget{};

    std::vector<SDL_TimerID> Timers;
    std::vector<const SDL_Rect*> UiRects{};

    Budget budget{};

    BudgetWindow* budgetWindow{ nullptr };
    GraphWindow* graphWindow{ nullptr };
    StringRender* stringRenderer{ nullptr };
    ToolPalette* toolPalette{ nullptr };

    CityProperties cityProperties;

    FileIo* fileIo{ nullptr };

    unsigned int speedModifier()
    {
        return SpeedModifierTable[static_cast<unsigned int>(SimSpeed())];
    }


    unsigned int zonePowerBlinkTick(unsigned int interval, void*)
    {
        toggleBlinkFlag();
        return interval;
    }


    unsigned int redrawMiniMapTick(unsigned int interval, void*)
    {
        RedrawMinimap = true;
        return interval;
    }


    unsigned int simulationTick(unsigned int interval, void*)
    {
        SimulationStep = true;
        return SimStepDefaultTime - speedModifier();
    }


    unsigned int animationTick(unsigned int interval, void*)
    {
        AnimationStep = true;
        return interval;
    }

    void initTimers()
    {
        Timers.push_back(SDL_AddTimer(500, zonePowerBlinkTick, nullptr));
        Timers.push_back(SDL_AddTimer(1000, redrawMiniMapTick, nullptr));
        Timers.push_back(SDL_AddTimer(SimStepDefaultTime, simulationTick, nullptr));
        Timers.push_back(SDL_AddTimer(AnimationStepDefaultTime, animationTick, nullptr));
    }

    void deinitTimers()
    {
        for (auto timer : Timers)
        {
            SDL_RemoveTimer(timer);
        }
    }
};


const Point<int>& viewOffset()
{
    return MapViewOffset;
}

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


bool AutoBulldoze{ false };
bool AutoGo{ false };

int InitSimLoad;
int ScenarioID;
bool NoDisasters;


void showBudgetWindow()
{
    BudgetWindowShown = true;
}


bool autoBudget()
{
    return AutoBudget;
}


void autoBudget(const bool b)
{
    AutoBudget = b;
}


bool autoGoto()
{
    return AutoGo;
}


void autoGoto(const bool b)
{
    AutoGo = b;
}


bool animationEnabled()
{
    return AnimationEnabled;
}


void animationEnabled(bool b)
{
    AnimationEnabled = b;
}


SDL_Rect& miniMapTileRect()
{
    return MiniMapTileRect;
}


void sim_exit()
{
    Exit = true;
}


void sim_update()
{
    updateDate();

    if (newMonth() && ShowGraphWindow) { graphWindow->update(); }

    scoreDoer(cityProperties);
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


void sim_loop(bool doSim)
{
    // \fixme Find a better way to do this
    if (BudgetWindowShown) { return; }

    if (doSim)
    {
        SimFrame(cityProperties, budget);
        SimulationStep = false;
    }

    if (AnimationStep)
    {
        AnimationStep = false;

        if (!Paused())
        {
            animateTiles();
            MoveObjects();
        }

        const Point<int> begin{ MapViewOffset.x / 16, MapViewOffset.y / 16 };
        const Point<int> end
        {
            std::clamp((MapViewOffset.x + WindowSize.x) / 16 + 1, 0, SimWidth),
            std::clamp((MapViewOffset.y + WindowSize.y) / 16 + 1, 0, SimHeight)
        };

        DrawBigMapSegment(begin, end);
    }

    if (RedrawMinimap)
    {
        DrawMiniMap();
        RedrawMinimap = false;
    }  

    sim_update();
}


void sim_init()
{
    userSoundOn(true);

    ScenarioID = 0;
    StartingYear = 1900;
    AutoGotoMessageLocation(true);
    CityTime = 50;
    NoDisasters = false;
    AutoBulldoze = true;
    AutoBudget = false;
    MessageId(NotificationId::None);
    ClearMes();
    SimSpeed(SimulationSpeed::Normal);
    ChangeEval();
    MessageLocation({ 0, 0 });
    
    InitSimLoad = 2;
    Exit = false;

    InitializeSound();
    StopEarthquake();
    ResetMapState();
    ResetEditorState();
    ClearMap();
    InitWillStuff();
    budget.CurrentFunds(5000);
    SetGameLevelFunds(0, cityProperties, budget);
    SimSpeed(SimulationSpeed::Paused);
}


void DoPlayNewCity(CityProperties& properties, Budget& budget)
{
    Eval("UIPlayNewCity");

    properties.GameLevel(0);
    properties.CityName("NowHere");
    GenerateNewCity(properties, budget);

    Resume();
    SimSpeed(SimulationSpeed::Normal);
}


void DoStartScenario(int scenario)
{
    Eval("UIStartScenario " + std::to_string(scenario));
}


void PrimeGame(const int startFlag, CityProperties& properties, Budget& budget)
{
    switch (startFlag)
    {
    case -2: // Load a city
        if (LoadCity("filename", properties, budget))
        {
            break;
        }

    case -1:
        properties.CityName("NowHere");
        DoPlayNewCity(properties, budget);
        break;

    case 0:
        throw std::runtime_error("Unexpected startup switch: " + std::to_string(startFlag));
        break;

    default: // scenario number
        DoStartScenario(startFlag);
        break;
    }
}


void ResetGame()
{
    sim_init();
    PrimeGame(-1, cityProperties, budget);
    DrawMiniMap();
    DrawBigMap();
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


void loadFonts()
{
    MainFont = new Font("res/raleway-medium.ttf", 12);
    MainBigFont = new Font("res/raleway-medium.ttf", 14);
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


void centerBudgetWindow()
{
    budgetWindow->position({ WindowSize.x / 2 - budgetWindow->rect().w / 2, WindowSize.y / 2 - budgetWindow->rect().h / 2 });
}


void centerGraphWindow()
{
    graphWindow->position({ WindowSize.x / 2 - graphWindow->rect().w / 2, WindowSize.y / 2 - graphWindow->rect().h / 2 });
}


void windowResized(const Vector<int>& size)
{
    getWindowSize();
    clampViewOffset();

    setMiniMapSelectorSize();

    updateMapDrawParameters();
    centerBudgetWindow();
    centerGraphWindow();

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

    case SDLK_4:
        if (Paused()) { Resume(); }
        SimSpeed(SimulationSpeed::AfricanSwallow);
        break;

    case SDLK_F2:
        if (!fileIo->filePicked() || SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT))
        {
            if (!fileIo->pickSaveFile())
            {
                break;
            }
        }

        SaveCity(fileIo->fullPath(), cityProperties, budget);
        break;

    case SDLK_F3:
        if (fileIo->pickOpenFile())
        {
            ResetGame();
            LoadCity(fileIo->fullPath(), cityProperties, budget);
        }
        break;

    case SDLK_F5:
        //MakeTornado();
        //MakeFlood();
        //MakeMeltdown();
        //MakeFire();
        break;

    case SDLK_F7:
        ResetGame();
        break;

    case SDLK_F9:
        ShowGraphWindow = !ShowGraphWindow;
        if (ShowGraphWindow) { graphWindow->update(); }
        break;

    case SDLK_F10:
        BudgetWindowShown = !BudgetWindowShown;
        if (BudgetWindowShown) { budgetWindow->update(); }
        break;

    default:
        break;

    }
}


void handleMouseEvent(SDL_Event& event)
{
    SDL_Point mouseMotionDelta{};

    switch (event.type)
    {
    case SDL_MOUSEMOTION:
        MousePosition = { event.motion.x, event.motion.y };
        mouseMotionDelta = { event.motion.xrel, event.motion.yrel };

        calculateMouseToWorld();

        if (ShowGraphWindow) { graphWindow->injectMouseMotion(mouseMotionDelta); }

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

            if (SDL_PointInRect(&mp, &budgetWindow->rect()))
            {
                budgetWindow->injectMouseClickPosition(mp);
            }

            if (SDL_PointInRect(&mp, &graphWindow->rect()))
            {
                graphWindow->injectMouseDown(mp);
            }

            if (!BudgetWindowShown)
            {
                ToolDown(TilePointedAt.x, TilePointedAt.y, budget);
            }
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            MouseClicked = true;
            MouseClickPosition = { event.button.x, event.button.y };

            if (BudgetWindowShown) { budgetWindow->injectMouseUp(); }
            if (ShowGraphWindow) { graphWindow->injectMouseUp(); }
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

    SDL_SetRenderDrawColor(MainWindowRenderer, Colors::Green.r, Colors::Green.g, Colors::Green.b, 255);
    SDL_RenderFillRect(MainWindowRenderer, &ResidentialValveRect);

    SDL_SetRenderDrawColor(MainWindowRenderer, Colors::MediumBlue.r, Colors::MediumBlue.g, Colors::MediumBlue.b, 255);
    SDL_RenderFillRect(MainWindowRenderer, &CommercialValveRect);

    SDL_SetRenderDrawColor(MainWindowRenderer, Colors::Gold.r, Colors::Gold.g, Colors::Gold.b, 255);
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

    SDL_SetTextureColorMod(MainBigFont->texture(), 255, 255, 255);

    stringRenderer->drawString(*MainBigFont, MonthString(static_cast<Month>(LastCityMonth())), {UiHeaderRect.x + 5, UiHeaderRect.y + 5});
    stringRenderer->drawString(*MainBigFont, std::to_string(CurrentYear()), { UiHeaderRect.x + 35, UiHeaderRect.y + 5});

    stringRenderer->drawString(*MainBigFont, LastMessage(), {100, UiHeaderRect.y + 5});

    const Point<int> budgetPosition{ UiHeaderRect.x + UiHeaderRect.w - 5 - MainBigFont->width(currentBudget), UiHeaderRect.y + 5 };
    stringRenderer->drawString(*MainBigFont, currentBudget, budgetPosition);
}


void drawMiniMapUi()
{
    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 65);
    SDL_RenderFillRect(MainWindowRenderer, &MiniMapBorder);

    SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(MainWindowRenderer, &MiniMapBorder);

    SDL_RenderCopy(MainWindowRenderer, MiniMapTexture.texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, powerMapTexture().texture, nullptr, &MiniMapDestination);

    // \todo Make this only draw when an overlay flag is set
    //SDL_RenderCopy(MainWindowRenderer, crimeOverlayTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, populationDensityTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, pollutionTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, landValueTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, policeRadiusTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, fireRadiusTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, rateOfGrowthTexture().texture, nullptr, &MiniMapDestination);

    // traffic map
    //SDL_RenderCopy(MainWindowRenderer, transitMapTexture().texture, nullptr, &MiniMapDestination);
    //SDL_RenderCopy(MainWindowRenderer, trafficDensityTexture().texture, nullptr, &MiniMapDestination);

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

    const SDL_Rect toolRect
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


void gameInit()
{
    sim_init();

    PrimeGame(-1, cityProperties, budget);

    initViewParamters();
    updateMapDrawParameters();
    initTimers();

    initOverlayTexture();
    initMapTextures();

    DrawMiniMap();
    DrawBigMap();

    fileIo = new FileIo(*MainWindow);

    stringRenderer = new StringRender(MainWindowRenderer);

    toolPalette = new ToolPalette(MainWindowRenderer);
    toolPalette->position({ UiHeaderRect.x, UiHeaderRect.y + UiHeaderRect.h + 5 });

    budgetWindow = new BudgetWindow(MainWindowRenderer, *stringRenderer, budget);
    centerBudgetWindow();

    graphWindow = new GraphWindow(MainWindowRenderer);
    centerGraphWindow();

    UiRects.push_back(&toolPalette->rect());
    UiRects.push_back(&UiHeaderRect);
}


void cleanUp()
{
    delete MainFont;
    delete MainBigFont;
    delete budgetWindow;
    delete graphWindow;
    delete toolPalette;
    delete stringRenderer;
    delete fileIo;

    deinitTimers();

    SDL_DestroyTexture(BigTileset.texture);
    SDL_DestroyTexture(RCI_Indicator.texture);

    SDL_DestroyRenderer(MainWindowRenderer);
    SDL_DestroyWindow(MainWindow);
}


void GameLoop()
{
    while (!Exit)
    {
        PendingTool = toolPalette->tool();

        sim_loop(SimulationStep);

        pumpEvents();

        currentBudget = NumberToDollarDecimal(budget.CurrentFunds());

        SDL_RenderClear(MainWindowRenderer);
        SDL_RenderCopy(MainWindowRenderer, MainMapTexture.texture, &FullMapViewRect, nullptr);
        DrawObjects();

        if (budget.NeedsAttention() || BudgetWindowShown)
        {
            SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 175);
            SDL_RenderFillRect(MainWindowRenderer, nullptr);
            budgetWindow->draw();

            if (budgetWindow->accepted())
            {
                budgetWindow->reset();
                BudgetWindowShown = false;
            }
        }
        else
        {
            DrawPendingTool(*toolPalette);

            drawTopUi();
            drawMiniMapUi();

            if (MouseClicked)
            {
                MouseClicked = false;
                toolPalette->injectMouseClickPosition(MouseClickPosition);
            }
            toolPalette->draw();

            if (ShowGraphWindow) { graphWindow->draw(); }
        }

        SDL_RenderPresent(MainWindowRenderer);

        NewMap = false;
    }
}


int main(int argc, char* argv[])
{
    std::cout << "Starting Micropolis-SDL2 version " << MicropolisVersion << " originally by Will Wright and Don Hopkins." << std::endl;
    std::cout << "Original code Copyright (C) 2002 by Electronic Arts, Maxis. Released under the GPL v3" << std::endl;
    std::cout << "Modifications Copyright (C) 2022 by Leeor Dicker. Available under the terms of the GPL v3" << std::endl << std::endl;
    
    std::cout << "Micropolis-SDL2 is not afiliated with Electronic Arts." << std::endl << std::endl;

    try
    {
        if (SDL_Init(SDL_INIT_EVERYTHING))
        {
            throw std::runtime_error(std::string("Unable to initialize SDL: ") + SDL_GetError());
        }

        initRenderer();
        loadGraphics();
        loadFonts();

        gameInit();

        GameLoop();

        cleanUp();

        SDL_Quit();
    }
    catch(std::exception e)
    {
        std::string message(std::string(e.what()) + "\n\nMicropolis-SDL2PP will now close.");
        MessageBoxA(nullptr, message.c_str(), "Micropolis-SDL2PP", MB_ICONERROR | MB_OK);
    }

    return 0;
}
