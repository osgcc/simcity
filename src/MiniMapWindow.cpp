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
#include "MiniMapWindow.h"

#include "BindFunction.h"
#include "Map.h"

#include "w_util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>


namespace
{
    constexpr SDL_Color BackgroundColor{ 126, 163, 217, 255 };
};


/**
 * Constructs a minimap window
 * 
 * \param position  Position to open the window at
 * \param size      Width/Height of the map in tiles
 */
MiniMapWindow::MiniMapWindow(const Point<int>& position, const Vector<int>& size):
    mMapSize{ size },
    mMinimapArea{ 0, 0, size.x * MiniTileSize, size.y * MiniTileSize },
    mButtonArea{ 0, mMinimapArea.h, mMinimapArea.w, ButtonAreaHeight }
{
    if (!SDL_WasInit(SDL_INIT_VIDEO))
    {
        std::cout << "MiniMapWindow::c'tor: SDL Video subsystem was not initialized. Initializing now." << std::endl;
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            throw std::runtime_error(std::string("MiniMapWindow():: Unable to initialize video subsystem: ") + SDL_GetError());
        }
    }

    mWindow = SDL_CreateWindow("Mini Map",
        position.x, position.y,
        size.x * MiniTileSize, size.y * MiniTileSize + ButtonAreaHeight,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_HIDDEN);

    if (!mWindow)
    {
        throw std::runtime_error("MiniMapWindow::MiniMapWindow(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mRenderer)
    {
        throw std::runtime_error("MiniMapWindow::MiniMapWindow(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    mWindowID = SDL_GetWindowID(mWindow);

    mTiles = loadTexture(mRenderer, "images/tilessm.xpm");
    mTexture.texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, size.x * MiniTileSize, size.y * MiniTileSize);
    mButtonTextures = loadTexture(mRenderer, "icons/minimap.png");

    setButtonValues();
    setButtonTextureUv();
    setButtonPositions();
}


MiniMapWindow::~MiniMapWindow()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
}


Uint32 MiniMapWindow::id() const
{
    return mWindowID;
}


void MiniMapWindow::focusOnMapCoordBind(fnPointIntParam fn)
{
    BindFuncPtr<std::vector<fnPointIntParam>, fnPointIntParam>(mFocusOnTileCallbacks, fn);
}


void MiniMapWindow::focusOnMapCoordUnbind(fnPointIntParam fn)
{
    UnbindFuncPtr<std::vector<fnPointIntParam>, fnPointIntParam>(mFocusOnTileCallbacks, fn);
}


void MiniMapWindow::updateMapViewPosition(const Point<int>& position)
{
    mSelector.x = std::clamp((position.x / TileSize) * MiniTileSize, 0, (mMapSize.x * MiniTileSize) - mSelector.w);
    mSelector.y = std::clamp((position.y / TileSize) * MiniTileSize, 0, (mMapSize.y * MiniTileSize) - mSelector.h);
}


void MiniMapWindow::updateViewportSize(const Vector<int>& viewportSize)
{
    mSelector.w = static_cast<int>(std::ceil(viewportSize.x / static_cast<float>(TileSize)) * MiniTileSize);
    mSelector.h = static_cast<int>(std::ceil(viewportSize.y / static_cast<float>(TileSize)) * MiniTileSize);
}


void MiniMapWindow::updateTilePointedAt(const Point<int>& tilePointedAt)
{
    mTileHighlight.x = tilePointedAt.x * MiniTileSize;
    mTileHighlight.y = tilePointedAt.y * MiniTileSize;
}


void MiniMapWindow::hide()
{
    SDL_HideWindow(mWindow);
}


void MiniMapWindow::show()
{
    SDL_RestoreWindow(mWindow);
    SDL_ShowWindow(mWindow);
}


void MiniMapWindow::draw()
{
    SDL_SetRenderDrawColor(mRenderer, BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, 255);
    SDL_RenderClear(mRenderer);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 150);

    SDL_Rect miniMapDrawRect{ 0, 0, MiniTileSize, MiniTileSize };

    SDL_SetRenderTarget(mRenderer, mTexture.texture);
    for (int row = 0; row < mMapSize.x; row++)
    {
        for (int col = 0; col < mMapSize.y; col++)
        {
            miniMapDrawRect = { row * MiniTileSize, col * MiniTileSize, miniMapDrawRect.w, miniMapDrawRect.h };
            mTileRect.y = maskedTileValue(tileValue(row, col)) * MiniTileSize;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }

    SDL_RenderPresent(mRenderer);
    SDL_SetRenderTarget(mRenderer, nullptr);
}


void MiniMapWindow::drawUI()
{
    SDL_RenderCopy(mRenderer, mTexture.texture, nullptr, &mMinimapArea);

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

    SDL_RenderDrawRect(mRenderer, &mSelector);
    SDL_RenderDrawRect(mRenderer, &mTileHighlight);

    for (int i{ 0 }; i < mButtons.size(); ++i)
    {
        SDL_RenderCopy(mRenderer, mButtonTextures.texture, &mButtonUV[i + (mButtons[i].state * 10)], &mButtons[i].rect);
    }

    SDL_RenderPresent(mRenderer);
}


void MiniMapWindow::injectEvent(const SDL_Event& event)
{
    if (event.window.windowID != id())
    {
        return;
    }

    switch (event.type)
    {
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        handleMouseEvent(event);
        break;

    case SDL_WINDOWEVENT:
        handleWindowEvent(event);
        break;

    default:
        break;
    }
}


void MiniMapWindow::handleMouseEvent(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            const Point<int> point{ event.button.x, event.button.y };
            if (pointInRect(point, mMinimapArea))
            {
                mButtonDownInMinimapArea = true;
                focusViewpoint(point);
            }
        }
        break;

    case SDL_MOUSEMOTION:
        handleMouseMotion(event);
        break;

    case SDL_MOUSEBUTTONUP:
        mButtonDownInMinimapArea = false;
        break;

    default:
        break;
    }
}


void MiniMapWindow::handleWindowEvent(const SDL_Event& event)
{
    switch (event.window.event)
    {
    case SDL_WINDOWEVENT_MINIMIZED:
        hide();
        mButtonDownInMinimapArea = false;
        break;

    case SDL_WINDOWEVENT_FOCUS_LOST:
    case SDL_WINDOWEVENT_HIDDEN:
    case SDL_WINDOWEVENT_CLOSE:
        mButtonDownInMinimapArea = false;
        break;

    default:
        break;
    }
}


void MiniMapWindow::handleMouseMotion(const SDL_Event& event)
{
    if (!mButtonDownInMinimapArea)
    {
        return;
    }

    if (event.motion.state & SDL_BUTTON_LMASK)
    {
        focusViewpoint({ event.motion.x, event.motion.y });
    }
}


void MiniMapWindow::focusViewpoint(const Point<int>& point)
{   
    const Point<int> adjustedPosition{ point - Vector<int>{mSelector.w / 2, mSelector.h / 2} };

    updateMapViewPosition(adjustedPosition.skewBy({ TileSize, TileSize }).skewInverseBy({ MiniTileSize,MiniTileSize }));

    for (auto callback : mFocusOnTileCallbacks)
    {
        callback({ mSelector.x, mSelector.y });
    }
}


void MiniMapWindow::setButtonValues()
{
    mButtons[0].id = ButtonId::Normal;
    mButtons[1].id = ButtonId::LandValue;
    mButtons[2].id = ButtonId::Crime;
    mButtons[3].id = ButtonId::FireProtection;
    mButtons[4].id = ButtonId::PoliceProtection;
    mButtons[5].id = ButtonId::PopulationDensity;
    mButtons[6].id = ButtonId::PopulationGrowth;
    mButtons[7].id = ButtonId::PoliceProtection;
    mButtons[8].id = ButtonId::TransportationNetwork;
    mButtons[9].id = ButtonId::PowerGrid;
}


void MiniMapWindow::setButtonTextureUv()
{
    for (int i = 0; i < 20; ++i)
    {
        mButtonUV[i] = { (i / 10) * 24, (i % 10) * 24, 24, 24 };
    }
}


void MiniMapWindow::setButtonPositions()
{
    constexpr Vector<int> buttonSize{ 24, 24 };
    constexpr Vector<int> buttonTransform{buttonSize.x + 3, 0};
    
    const int startPosition = (mButtonArea.w - (buttonTransform.x * 10)) / 2;

    for (int i{ 0 }; i < 10; ++i)
    {
        mButtons[i].rect = { startPosition + buttonTransform.x * i, mButtonArea.y + 3, buttonSize.x, buttonSize.y };
    }
}
