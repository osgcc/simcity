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
#include "Evaluation.h"
#include "EvaluationWindow.h"
#include "GraphWindow.h"
#include "MiniMapWindow.h"

#include "CityProperties.h"
#include "Colors.h"
#include "Connection.h"
#include "FileIo.h"
#include "Font.h"
#include "Graph.h"
#include "Map.h"
#include "Tool.h"

#include "g_ani.h"

#include "s_alloc.h"
#include "s_disast.h"
#include "s_fileio.h"
#include "s_gen.h"
#include "s_msg.h"
#include "s_sim.h"

#include "Scan.h"
#include "Sprite.h"
#include "StringRender.h"

#include "w_sound.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include "Texture.h"
#include "ToolPalette.h"

#include "WindowStack.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#if defined(__APPLE__)
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#endif

const std::string MicropolisVersion = "4.0";

SDL_Window* MainWindow = nullptr;
SDL_Renderer* MainWindowRenderer = nullptr;

uint32_t MainWindowId{};

Texture MainMapTexture{};

Texture BigTileset{};
Texture RCI_Indicator{};


bool AutoBulldoze{ false };
bool AutoGo{ false };

int InitSimLoad;
int ScenarioID;
bool NoDisasters;


namespace
{
    constexpr auto RciValveHeight = 20;
    constexpr auto TileSize = 16;
    constexpr auto MiniTileSize = 3;
    constexpr auto MiniMapTileMultiplier = (TileSize + MiniTileSize - 1) / MiniTileSize;

    SDL_Rect UiHeaderRect{ 10, 10, 0, 0 };
    SDL_Rect RciDestination{};
    SDL_Rect FullMapViewRect{};

    SDL_Rect ResidentialValveRect{ 0, 0, 4, 0 };
    SDL_Rect CommercialValveRect{ 0, 0, 4, 0 };
    SDL_Rect IndustrialValveRect{ 0, 0, 4, 0 };

    Vector<int> WindowSize{};
    Vector<int> DraggableToolVector{};

    Point<int> MapViewOffset{};
    Point<int> TilePointedAt{};

    bool Exit{ false };
    bool RedrawMinimap{ false };
    bool SimulationStep{ false };
    bool AnimationEnabled{ true };
    bool AnimationStep{ false };
    bool AutoBudget{ false };
    bool RightButtonDrag{ false };

    constexpr unsigned int SimStepDefaultTime{ 100 };
    constexpr unsigned int AnimationStepDefaultTime{ 150 };

    SDL_Rect TileHighlight{ 0, 0, TileSize, TileSize };

    std::array<unsigned int, 5> SpeedModifierTable{ 0, 0, 50, 75, 95 };

    std::string currentBudget{};

    std::vector<SDL_TimerID> Timers;
    std::vector<const SDL_Rect*> UiRects{};

    Budget budget{};
    CityProperties cityProperties{};

    std::unique_ptr<BudgetWindow> budgetWindow;
    std::unique_ptr<GraphWindow> graphWindow;
    std::unique_ptr<EvaluationWindow> evaluationWindow;
    std::unique_ptr<MiniMapWindow> miniMapWindow;
    std::unique_ptr<ToolPalette> toolPalette;
    std::unique_ptr<StringRender> stringRenderer;

    std::unique_ptr<FileIo> fileIo;

    std::unique_ptr<Font> MainFont;
    std::unique_ptr<Font> MainBigFont;


    WindowStack GuiWindowStack;


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


namespace EventHandling
{
    Point<int> MouseDownPosition{};
    Point<int> MouseClickPosition{};
    Point<int> MousePosition{};

    bool MouseLeftDown{ false };
};


const Point<int>& viewOffset()
{
    return MapViewOffset;
}


void showBudgetWindow()
{
    budgetWindow->show();
    budgetWindow->update();
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


void simExit()
{
    Exit = true;
}


void simUpdate()
{
    updateDate();

    if (newMonth() && graphWindow->visible()) { graphWindow->update(); }

    scoreDoer(cityProperties);
}


void simLoop(bool doSim)
{
    // \fixme Find a better way to do this
    if (budgetWindow->visible()) { return; }

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
            updateSprites();
        }

        const Point<int> begin{ MapViewOffset.x / TileSize, MapViewOffset.y / TileSize };
        const Point<int> end
        {
            std::clamp((MapViewOffset.x + WindowSize.x) / TileSize + 1, 0, SimWidth),
            std::clamp((MapViewOffset.y + WindowSize.y) / TileSize + 1, 0, SimHeight)
        };

        DrawBigMapSegment(begin, end);
    }

    if (RedrawMinimap)
    {
        miniMapWindow->draw();
        RedrawMinimap = false;
    }

    simUpdate();
}


void initWillStuff()
{
    RoadEffect = 32;
    PoliceEffect = 1000;
    FireEffect = 1000;
    cityScore(500);
    cityPopulation(-1);
    LastCityTime(-1);
    LastCityYear(1);
    LastCityMonth(0);
    pendingTool(Tool::None);
    MessageId(NotificationId::None);
    destroyAllSprites();
    DisasterEvent = 0;
    initMapArrays();
    DoNewGame();
}


void simInit()
{
    userSoundOn(true);

    ScenarioID = 0;
    StartingYear = 1900;
    AutoGotoMessageLocation(true);
    CityTime = 50;
    NoDisasters = false;
    AutoBulldoze = true;
    AutoBudget = true;
    MessageId(NotificationId::None);
    ClearMes();
    SimSpeed(SimulationSpeed::Normal);
    ChangeEval();
    MessageLocation({ 0, 0 });
    
    InitSimLoad = 2;
    Exit = false;

    InitializeSound();
    StopEarthquake();
    ClearMap();
    initWillStuff();
    budget.CurrentFunds(5000);
    SetGameLevelFunds(0, cityProperties, budget);
    SimSpeed(SimulationSpeed::Paused);
}


void doPlayNewCity(CityProperties& properties, Budget& budget)
{
    GenerateNewCity(properties, budget);
    Resume();
    SimSpeed(SimulationSpeed::Normal);
}


void doStartScenario(int scenario)
{
    Eval("UIStartScenario " + std::to_string(scenario));
}


void primeGame(const int startFlag, CityProperties& properties, Budget& budget)
{
    switch (startFlag)
    {
    case -2: // Load a city
        if (LoadCity("filename", properties, budget))
        {
            break;
        }
        // If load fails, simply create a new city
        [[fallthrough]];

    case -1:
        properties.GameLevel(0);
        properties.CityName("NowHere");
        doPlayNewCity(properties, budget);
        break;

    case 0:
        throw std::runtime_error("Unexpected startup switch: " + std::to_string(startFlag));
        break;

    default: // scenario number
        doStartScenario(startFlag);
        break;
    }
}


void resetGame()
{
    simInit();
    primeGame(-1, cityProperties, budget);
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

    SDL_Rect srcRect{ 0, 0, TileSize, TileSize };
    SDL_Rect dstRect{ 0, 0, TileSize, TileSize };

    for (int i = 0; i < TILE_COUNT; ++i)
    {
        srcRect.y = i * TileSize;
        dstRect = { (i % (TileSize * 2)) * TileSize, (i / (TileSize * 2)) * TileSize, TileSize, TileSize };
        SDL_BlitSurface(srcSurface, &srcRect, dstSurface, &dstRect);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(MainWindowRenderer, dstSurface);

    SDL_FreeSurface(srcSurface);
    SDL_FreeSurface(dstSurface);

    if (!texture)
    {
        const std::string message(std::string("buildBigTileset(): ") + SDL_GetError());
        std::cout << message << std::endl;
        throw std::runtime_error(message);
    }

    Vector<int> size{};
    SDL_QueryTexture(texture, nullptr, nullptr, &size.x, &size.y);

    BigTileset = { texture, SDL_Rect{ 0, 0, size.x, size.y }, { size.x, size.y } };
}


void loadGraphics()
{
    buildBigTileset();
    RCI_Indicator = loadTexture(MainWindowRenderer, "images/demandg.xpm");
}


void loadFonts()
{
    MainFont = std::make_unique<Font>("res/Raleway-Medium.ttf", 12);
    MainBigFont = std::make_unique<Font>("res/Raleway-Medium.ttf", 14);
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

    miniMapWindow->updateMapViewPosition(MapViewOffset);
}


void windowSize()
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


void minimapViewUpdated(const Point<int>& newOffset)
{
    MapViewOffset = newOffset.skewBy({ MiniMapTileMultiplier, MiniMapTileMultiplier });
    clampViewOffset();
    updateMapDrawParameters();
}


void centerWindow(WindowBase& window)
{
    window.position({ WindowSize.x / 2 - window.area().width / 2, WindowSize.y / 2 - window.area().height / 2 });
}


void windowResized(const Vector<int>& size)
{
    windowSize();
    clampViewOffset();

    miniMapWindow->updateViewportSize(WindowSize);

    updateMapDrawParameters();
    centerWindow(*budgetWindow);
    centerWindow(*graphWindow);
    centerWindow(*evaluationWindow);

    UiHeaderRect.w = WindowSize.x - 20;
}


void calculateMouseToWorld()
{
    const auto screenCell = PositionToCell(EventHandling::MousePosition, MapViewOffset);
    
    TilePointedAt =
    {
       std::clamp(screenCell.x + (MapViewOffset.x / TileSize), 0, SimWidth - 1),
       std::clamp(screenCell.y + (MapViewOffset.y / TileSize), 0, SimHeight - 1)
    };

    TileHighlight =
    {
        (screenCell.x * TileSize) - MapViewOffset.x % TileSize,
        (screenCell.y * TileSize) - MapViewOffset.y % TileSize,
        TileSize, TileSize
    };

    miniMapWindow->updateTilePointedAt(TilePointedAt);
}


bool IgnoreToolMouseUp(Point<int>& mousePosition)
{
    for (auto rect : UiRects)
    {
        if (pointInRect(mousePosition, *rect))
        {
            return true;
        }
    }

    if (GuiWindowStack.pointInWindow(EventHandling::MousePosition))
    {
        return true;
    }

    if (EventHandling::MouseDownPosition != EventHandling::MousePosition &&
        GuiWindowStack.pointInWindow(EventHandling::MouseDownPosition))
    {
        return true;
    }

    return false;
}


void ShowWindowAndBringToFront(WindowBase& window)
{
    window.toggleVisible();
    GuiWindowStack.bringToFront(&window);
    if (window.visible()) { window.update(); }
}


void SetSpeed(SimulationSpeed speed)
{
    if (Paused())
    {
        Resume();
    }

    SimSpeed(speed);
}


void TogglePause()
{
    Paused() ? Resume() : Pause();
}


void ToggleMiniMapVisibility()
{
    miniMapWindow->hidden() ? miniMapWindow->show() : miniMapWindow->hide();
}


void showEvaluationWindow()
{
    evaluationWindow->setEvaluation(currentEvaluation());
    currentEvaluationSeen();
    ShowWindowAndBringToFront(*evaluationWindow.get());
}


void handleKeyEvent(SDL_Event& event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_ESCAPE:
        GuiWindowStack.hide();
        break;

    case SDLK_0:
    case SDLK_p:
    case SDLK_SPACE:
        TogglePause();
        break;

    case SDLK_1:
        SetSpeed(SimulationSpeed::Slow);
        break;

    case SDLK_2:
        SetSpeed(SimulationSpeed::Normal);
        break;

    case SDLK_3:
        SetSpeed(SimulationSpeed::Fast);
        break;

    case SDLK_4:
        SetSpeed(SimulationSpeed::AfricanSwallow);
        break;

    case SDLK_F2:
        if (!fileIo->filePicked() || SDL_GetModState() & KMOD_SHIFT)
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
            resetGame();
            LoadCity(fileIo->fullPath(), cityProperties, budget);
            DrawBigMap();
        }
        break;

    case SDLK_F4:
        ToggleMiniMapVisibility();
        break;

    case SDLK_F5:
        //generateMonster();
        //generateTornado();
        //MakeFlood();
        //MakeMeltdown();
        //MakeFire();
        break;

    case SDLK_F7:
        resetGame();
        DrawBigMap();
        break;

    case SDLK_F9:
        ShowWindowAndBringToFront(*graphWindow.get());
        break;

    case SDLK_F10:
        ShowWindowAndBringToFront(*budgetWindow.get());
        break;
            
    case SDLK_F1:
        showEvaluationWindow();
        break;

    default:
        break;

    }
}


void handleMouseEvent(SDL_Event& event)
{
    if (event.window.windowID != MainWindowId) { return; }

    Vector<int> mouseMotionDelta{};
    Point<int> mousePosition = EventHandling::MousePosition;

    switch (event.type)
    {
    case SDL_MOUSEMOTION:
        EventHandling::MousePosition = { event.motion.x, event.motion.y };
        mouseMotionDelta = { event.motion.xrel, event.motion.yrel };

        DraggableToolVector = {};
        if (pendingToolProperties().draggable && EventHandling::MouseLeftDown && toolStart() != TilePointedAt)
        {
            DraggableToolVector = vectorFromPoints(toolStart(), TilePointedAt);
            validateDraggableToolVector(DraggableToolVector, budget);
        }

        calculateMouseToWorld();

        GuiWindowStack.injectMouseMotion(mouseMotionDelta);

        if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_RMASK) != 0)
        {
            MapViewOffset -= mouseMotionDelta;
            clampViewOffset();
            updateMapDrawParameters();
            RightButtonDrag = true;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            EventHandling::MouseLeftDown = true;
            EventHandling::MouseDownPosition = { event.motion.x, event.motion.y };

            for (auto rect : UiRects)
            {
                if (pointInRect(mousePosition, *rect))
                {
                    return;
                }
            }

            if (GuiWindowStack.pointInWindow(EventHandling::MousePosition))
            {
                GuiWindowStack.updateStack(EventHandling::MousePosition);
                GuiWindowStack.front()->injectMouseDown(EventHandling::MousePosition);
                return;
            }

            toolStart(TilePointedAt);
            
            if (!budgetWindow->visible() && !pendingToolProperties().draggable)
            {
                ToolDown(TilePointedAt, budget);
            }
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            EventHandling::MouseLeftDown = false;
            EventHandling::MouseClickPosition = { event.button.x, event.button.y };

            GuiWindowStack.injectMouseUp();

            if (IgnoreToolMouseUp(mousePosition))
            {
                return;
            }

            toolEnd(TilePointedAt);
            
            if (pendingToolProperties().draggable)
            {
                executeDraggableTool(DraggableToolVector, TilePointedAt, budget);
            }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            if (!RightButtonDrag)
            {
                pendingTool(Tool::None);
                toolPalette->cancelTool();
            }

            RightButtonDrag = false;
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

    case SDL_WINDOWEVENT_CLOSE:
        if (event.window.windowID == miniMapWindow->id())
        {
            miniMapWindow->hide();
        }

        if (event.window.windowID == MainWindowId)
        {
            simExit();
        }

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
        miniMapWindow->injectEvent(event);

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
            simExit();
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
        throw std::runtime_error("initRenderer(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    SDL_SetWindowMinimumSize(MainWindow, 800, 600);

    #if defined(__APPLE__)
    MainWindowRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_SOFTWARE);
    #else
    MainWindowRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_ACCELERATED);
    #endif
    
    if (!MainWindowRenderer)
    {
        throw std::runtime_error("initRenderer(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    SDL_SetRenderDrawBlendMode(MainWindowRenderer, SDL_BLENDMODE_BLEND);

    MainWindowId = SDL_GetWindowID(MainWindow);
}


void initViewParamters()
{
    windowSize();

    MainMapTexture.texture = SDL_CreateTexture(MainWindowRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, SimWidth * 16, SimHeight * 16);
    MainMapTexture.dimensions = { SimWidth * 16, SimHeight * 16 };

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

    stringRenderer->drawString(*MainBigFont, MonthString(static_cast<Month>(LastCityMonth())), {UiHeaderRect.x + 5, UiHeaderRect.y + 5});
    stringRenderer->drawString(*MainBigFont, std::to_string(CurrentYear()), { UiHeaderRect.x + 35, UiHeaderRect.y + 5});

    stringRenderer->drawString(*MainBigFont, LastMessage(), {100, UiHeaderRect.y + 5});

    const Point<int> budgetPosition{ UiHeaderRect.x + UiHeaderRect.w - 5 - MainBigFont->width(currentBudget), UiHeaderRect.y + 5 };
    stringRenderer->drawString(*MainBigFont, currentBudget, budgetPosition);
}


void DrawPendingTool(const ToolPalette& palette)
{
    if (palette.tool() == Tool::None || (pendingToolProperties().draggable && EventHandling::MouseLeftDown))
    {
        return;
    }

    const SDL_Rect toolRect
    {
        TileHighlight.x - (pendingToolProperties().offset * TileSize),
        TileHighlight.y - (pendingToolProperties().offset * TileSize),
        pendingToolProperties().size * TileSize,
        pendingToolProperties().size * TileSize
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


void drawDraggableToolVector()
{
    if (!EventHandling::MouseLeftDown) { return; }
    if (GuiWindowStack.pointInWindow(EventHandling::MouseDownPosition))
    {
        return;
    }
    
    SDL_Rect toolRect
    {
        (toolStart().x * TileSize) - MapViewOffset.x,
        (toolStart().y * TileSize) - MapViewOffset.y,
        TileSize, TileSize
    };

    const int axis = longestAxis(DraggableToolVector);
    const int size = (std::abs(axis) * TileSize) + TileSize;

    const bool xAxisLarger = std::abs(DraggableToolVector.x) > std::abs(DraggableToolVector.y);
    xAxisLarger ? toolRect.w = size : toolRect.h = size;

    if (axis < 0)
    {
        const int startValue = size - TileSize;
        xAxisLarger ? toolRect.x -= startValue : toolRect.y -= startValue;
    }

    SDL_SetRenderDrawColor(MainWindowRenderer, 255, 255, 255, 100);
    SDL_RenderFillRect(MainWindowRenderer, &toolRect);

    SDL_SetRenderDrawColor(MainWindowRenderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(MainWindowRenderer, &toolRect);
}


void gameInit()
{
    simInit();

    primeGame(-1, cityProperties, budget);

    updateMapDrawParameters();
    initTimers();
}


void initUI()
{
    Point<int> mainWindowPosition{};
    SDL_GetWindowPosition(MainWindow, &mainWindowPosition.x, &mainWindowPosition.y);

    SDL_DisplayMode mode{};
    SDL_GetDesktopDisplayMode(0, &mode);

    const Point<int> miniMapWindowPosition
    {
        std::clamp(mainWindowPosition.x - (SimWidth * MiniTileSize) - 10, 10, mode.w),
        std::clamp(mainWindowPosition.y, 10, mode.h)
    };

    miniMapWindow = std::make_unique<MiniMapWindow>(miniMapWindowPosition, Vector<int>{ SimWidth, SimHeight });
    miniMapWindow->updateViewportSize(WindowSize);
    miniMapWindow->focusOnMapCoordBind(&minimapViewUpdated);

    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::Crime, CrimeMap);
    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::FireProtection, FireProtectionMap);
    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::LandValue, LandValueMap);
    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::PoliceProtection, PoliceProtectionMap);
    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::Pollution, PollutionMap);
    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::PopulationDensity, PopulationDensityMap);
    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::PopulationGrowth, RateOfGrowthMap);
    miniMapWindow->linkEffectMap(MiniMapWindow::ButtonId::TrafficDensity, TrafficDensityMap);

    fileIo = std::make_unique<FileIo>(*MainWindow);

    stringRenderer = std::make_unique<StringRender>(MainWindowRenderer);
    toolPalette = std::make_unique<ToolPalette>(MainWindowRenderer);
    toolPalette->position({ UiHeaderRect.x, UiHeaderRect.y + UiHeaderRect.h + 5 });

    budgetWindow = std::make_unique<BudgetWindow>(MainWindowRenderer, *stringRenderer, budget);
    centerWindow(*budgetWindow);

    graphWindow = std::make_unique<GraphWindow>(MainWindowRenderer);
    centerWindow(*graphWindow);
    
    evaluationWindow = std::make_unique<EvaluationWindow>(MainWindowRenderer);
    centerWindow(*evaluationWindow);

    GuiWindowStack.addWindow(budgetWindow.get());
    GuiWindowStack.addWindow(evaluationWindow.get());
    GuiWindowStack.addWindow(graphWindow.get());
    GuiWindowStack.addWindow(toolPalette.get());

    UiRects.push_back(&UiHeaderRect);
}


void cleanUp()
{
    deinitTimers();

    SDL_DestroyTexture(BigTileset.texture);
    SDL_DestroyTexture(RCI_Indicator.texture);

    SDL_DestroyRenderer(MainWindowRenderer);
    SDL_DestroyWindow(MainWindow);
}


void GameLoop()
{
    miniMapWindow->draw();
    DrawBigMap();

    while (!Exit)
    {
        pendingTool(toolPalette->tool());

        simLoop(SimulationStep);

        pumpEvents();

        currentBudget = NumberToDollarDecimal(budget.CurrentFunds());

        SDL_RenderClear(MainWindowRenderer);
        SDL_RenderCopy(MainWindowRenderer, MainMapTexture.texture, &FullMapViewRect, nullptr);
        drawSprites();

        if (budget.NeedsAttention() || budgetWindow->visible())
        {
            SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 175);
            SDL_RenderFillRect(MainWindowRenderer, nullptr);
            budgetWindow->draw();

            if (budgetWindow->accepted())
            {
                budgetWindow->reset();
                budgetWindow->hide();
            }
        }
        else
        {
            if (!GuiWindowStack.pointInWindow(EventHandling::MousePosition))
            {
                DrawPendingTool(*toolPalette);
                drawDraggableToolVector();
            }

            drawTopUi();

            if (currentEvaluation().needsAttention)
            {
                evaluationWindow->setEvaluation(currentEvaluation());
                currentEvaluationSeen();
            }

            GuiWindowStack.draw();
        }

        SDL_RenderPresent(MainWindowRenderer);
        miniMapWindow->drawUI();

        newMap(false);
    }
}


int main(int argc, char* argv[])
{
    std::cout << "Starting Micropolis-SDL2 version " << MicropolisVersion << " originally by Will Wright and Don Hopkins." << std::endl;
    std::cout << "Original code Copyright © 2002 by Electronic Arts, Maxis. Released under the GPL v3" << std::endl;
    std::cout << "Modifications Copyright © 2022 - 2023 by Leeor Dicker. Available under the terms of the GPL v3" << std::endl << std::endl;
    
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

        initViewParamters();
        initUI();

        gameInit();

        LoadScenario(Scenario::Dullsville, cityProperties, budget);

        GameLoop();

        cleanUp();

        SDL_Quit();
    }
    catch(std::exception e)
    {
        std::string message(std::string(e.what()) + "\n\nMicropolis-SDL2PP will now close.");
        
        #if defined(WIN32)
        MessageBoxA(nullptr, message.c_str(), "Micropolis-SDL2PP", MB_ICONERROR | MB_OK);
        #else
        std::cout << message << std::endl;
        #endif
    }

    return 0;
}
