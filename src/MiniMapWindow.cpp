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
#include "Colors.h"
#include "EffectMap.h"
#include "Graphics.h"
#include "Map.h"

#include "w_util.h"

#include <SDL2/SDL.h>

#if defined(__APPLE__)
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>


namespace
{
    constexpr auto VAL_NONE = 0;
    constexpr auto VAL_LOW = 1;
    constexpr auto VAL_MEDIUM = 2;
    constexpr auto VAL_HIGH = 3;
    constexpr auto VAL_VERYHIGH = 4;
    constexpr auto VAL_PLUS = 5;
    constexpr auto VAL_VERYPLUS = 6;
    constexpr auto VAL_MINUS = 7;
    constexpr auto VAL_VERYMINUS = 8;

    constexpr auto UNPOWERED = 0;
    constexpr auto POWERED = 1;
    constexpr auto CONDUCTIVE = 2;

    constexpr SDL_Color BackgroundColor{ 126, 163, 217, 255 };

    std::array<SDL_Color, 3> PowerColorTable =
    { {
        Colors::LightGrey,
        Colors::Red,
        Colors::LightGrey
    } };


    std::array<SDL_Color, 9> OverlayColorTable =
    { {
        Colors::Clear,
        Colors::LightGrey,
        Colors::Yellow,
        Colors::Orange,
        Colors::Red,
        Colors::DarkGreen,
        Colors::LightGreen,
        Colors::Orange,
        Colors::Yellow
    } };


    int rateOfGrowthColorIndex(int value)
    {
        if (value > 100)
        {
            return VAL_VERYPLUS;
        }

        if (value > 20)
        {
            return VAL_PLUS;
        }

        if (value < -100)
        {
            return VAL_VERYMINUS;
        }

        if (value < -20)
        {
            return VAL_MINUS;
        }

        return VAL_NONE;
    }


    int GetColorIndex(int x)
    {
        if (x < 50)
        {
            return VAL_NONE;
        }
        if (x < 100)
        {
            return VAL_LOW;
        }
        if (x < 150)
        {
            return VAL_MEDIUM;
        }
        if (x < 200)
        {
            return VAL_HIGH;
        }

        return VAL_VERYHIGH;
    }

    void clearOverlayTexture(SDL_Renderer& renderer)
    {
        SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
        SDL_RenderClear(&renderer);
    }

    void drawOverlayPoints(SDL_Renderer& renderer, Texture& overlay, const EffectMap& map)
    {
        SDL_SetRenderTarget(&renderer, overlay.texture);
        turnOffBlending(renderer, overlay);
        clearOverlayTexture(renderer);

        for (int x = 0; x < map.dimensions().x; x++)
        {
            for (int y = 0; y < map.dimensions().y; y++)
            {
                const auto& color = OverlayColorTable[GetColorIndex(map.value({ x, y }))];
                drawPoint(renderer, { x, y }, color);
            }
        }

        turnOnBlending(renderer, overlay);
        SDL_SetRenderTarget(&renderer, nullptr);
    }

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
            throw std::runtime_error(std::string("MiniMapWindow(): Unable to initialize video subsystem: ") + SDL_GetError());
        }
    }

    mWindow = SDL_CreateWindow("Mini Map",
        position.x, position.y,
        size.x * MiniTileSize, size.y * MiniTileSize + ButtonAreaHeight,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_HIDDEN);

    if (!mWindow)
    {
        throw std::runtime_error("MiniMapWindow(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    #if defined(__APPLE__)
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_SOFTWARE);
    #else
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    #endif
    
    if (!mRenderer)
    {
        throw std::runtime_error("MiniMapWindow(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    mWindowID = SDL_GetWindowID(mWindow);

    mTiles = loadTexture(mRenderer, "images/tilessm.xpm");
    mTexture.texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, size.x * MiniTileSize, size.y * MiniTileSize);
    mButtonTextures = loadTexture(mRenderer, "icons/minimap.png");

    setButtonValues();
    setButtonTextureUv();
    setButtonPositions();
    resetOverlayButtons();

    initOverlayTextures();
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


void MiniMapWindow::linkEffectMap(ButtonId id, const EffectMap& map)
{
    mEffectMaps[id] = &map;
}


void MiniMapWindow::initTexture(Texture& texture, const Vector<int>& dimensions)
{
    texture.texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dimensions.x, dimensions.y);
    SDL_QueryTexture(texture.texture, nullptr, nullptr, &texture.dimensions.x, &texture.dimensions.y);
    texture.area = { 0, 0, texture.dimensions.x, texture.dimensions.y };
}


void MiniMapWindow::initOverlayTextures()
{
    const auto overlayHalfSize = mMapSize.skewInverseBy({ 2, 2 });
    initTexture(mOverlayTextures[ButtonId::Crime], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::LandValue], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::PopulationDensity], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::Pollution], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::TrafficDensity], overlayHalfSize);
    
    // Necessary because of integer math dropping fractional component.
    const Vector<int> overlayEigthSize{ mMapSize.x / 8, (mMapSize.y / 8) + 1 };
    initTexture(mOverlayTextures[ButtonId::PoliceProtection], overlayEigthSize);
    initTexture(mOverlayTextures[ButtonId::FireProtection], overlayEigthSize);
    initTexture(mOverlayTextures[ButtonId::PopulationGrowth], overlayEigthSize);
    
    // Full size textures
    initTexture(mOverlayTextures[ButtonId::TransportationNetwork], mMapSize.skewBy({MiniTileSize, MiniTileSize}));
    initTexture(mOverlayTextures[ButtonId::PowerGrid], mMapSize.skewBy({MiniTileSize, MiniTileSize}));
}


void MiniMapWindow::drawCurrentOverlay()
{
    if (mButtonDownId != ButtonId::Normal)
    {
        auto map = mEffectMaps[mButtonDownId];
        if (map)
        {
            drawOverlayPoints(*mRenderer, mOverlayTextures[mButtonDownId], *map);
        }
    }
}


void MiniMapWindow::resetOverlayButtons()
{
    for (auto& button : mButtons)
    {
        button.state = ButtonStateNormal;
    }

    mButtons[0].state = ButtonStatePressed;
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


void MiniMapWindow::drawPlainMap()
{
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


void MiniMapWindow::drawPowerMap()
{
    SDL_Color tileColor{};
    SDL_Rect miniMapDrawRect{ 0, 0, MiniTileSize, MiniTileSize };
    SDL_SetRenderTarget(mRenderer, mOverlayTextures[ButtonId::PowerGrid].texture);

    for (int row = 0; row < mMapSize.x; row++)
    {
        for (int col = 0; col < mMapSize.y; col++)
        {
            miniMapDrawRect = { row * MiniTileSize, col * MiniTileSize, miniMapDrawRect.w, miniMapDrawRect.h };

            const unsigned int unmaskedTile = tileValue(row, col);
            unsigned int tile = maskedTileValue(unmaskedTile);

            bool colored{ true };

            if (tile <= LASTFIRE)
            {
                colored = false;
            }
            else if (unmaskedTile & ZONEBIT)
            {
                tileColor = (unmaskedTile & PWRBIT) ? Colors::Red : Colors::LightBlue;
            }
            else
            {
                if (unmaskedTile & CONDBIT)
                {
                    tileColor = Colors::LightGrey;
                }
                else
                {
                    tile = DIRT;
                    colored = false;
                }
            }

            if (colored)
            {
                SDL_SetRenderDrawColor(mRenderer, tileColor.r, tileColor.g, tileColor.b, 255);
                SDL_RenderFillRect(mRenderer, &miniMapDrawRect);
            }
            else
            {
                mTileRect.y = maskedTileValue(tileValue(row, col)) * MiniTileSize;
                SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
            }
        }
    }
    SDL_RenderPresent(mRenderer);
    SDL_SetRenderTarget(mRenderer, nullptr);
}


void MiniMapWindow::drawLilTransMap()
{
    SDL_Rect miniMapDrawRect{ 0, 0, MiniTileSize, MiniTileSize };
    SDL_SetRenderTarget(mRenderer, mOverlayTextures[ButtonId::TransportationNetwork].texture);
    
    for (int row = 0; row < mMapSize.x; row++)
    {
        for (int col = 0; col < mMapSize.y; col++)
        {
            miniMapDrawRect = { row * MiniTileSize, col * MiniTileSize, miniMapDrawRect.w, miniMapDrawRect.h };

            unsigned int tile = maskedTileValue(row, col);

            if ((tile >= ResidentialBase) ||
                ((tile >= BRWXXX7) && tile <= 220) ||
                (tile == UNUSED_TRASH6))
            {
                tile = 0;
            }

            mTileRect.y = maskedTileValue(tile) * MiniTileSize;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }

    SDL_RenderPresent(mRenderer);
    SDL_SetRenderTarget(mRenderer, nullptr);
}


void MiniMapWindow::draw()
{
    if(mButtonDownId == ButtonId::PowerGrid)
    {
        drawPowerMap();
        return;
    }
    
    if(mButtonDownId == ButtonId::TransportationNetwork)
    {
        drawLilTransMap();
        return;
    }

    drawPlainMap();
    drawCurrentOverlay();
}


void MiniMapWindow::drawUI()
{
    SDL_SetRenderDrawColor(mRenderer, BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, 255);
    SDL_RenderClear(mRenderer);

    SDL_RenderCopy(mRenderer, mTexture.texture, nullptr, &mMinimapArea);

    if (mButtonDownId != ButtonId::Normal)
    {
        SDL_RenderCopy(mRenderer, mOverlayTextures[mButtonDownId].texture, nullptr, &mMinimapArea);
    }

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 150);
    SDL_RenderDrawRect(mRenderer, &mSelector);
    SDL_RenderDrawRect(mRenderer, &mTileHighlight);

    const int arraySize = static_cast<int>(mButtons.size());
    for (int i{ 0 }; i < arraySize; ++i)
    {
        SDL_RenderCopy(mRenderer, mButtonTextures.texture, &mButtonUV[i + (mButtons[i].state * arraySize)], &mButtons[i].rect);
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
            else if (pointInRect(point, mButtonArea))
            {
                for (auto& button : mButtons)
                {
                    if (pointInRect(point, button.rect))
                    {
                        button.state = ButtonStatePressed;
                        mButtonDownId = button.id;

                        // fixme    Find a better way to do this
                        if (button.id == ButtonId::TransportationNetwork)
                        {
                            drawLilTransMap();
                        }
                        else if (button.id == ButtonId::PowerGrid)
                        {
                            drawPowerMap();
                        }
                        else
                        {
                            drawCurrentOverlay();
                        }
                    }
                    else
                    {
                        button.state = ButtonStateNormal;
                    }
                }

                if (noButtonsSelected())
                {
                    mButtons[0].state = ButtonStatePressed;
                    mButtonDownId = ButtonId::Normal;
                }
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
    mButtons[7].id = ButtonId::Pollution;
    mButtons[8].id = ButtonId::TrafficDensity;
    mButtons[9].id = ButtonId::TransportationNetwork;
    mButtons[10].id = ButtonId::PowerGrid;
}


void MiniMapWindow::setButtonTextureUv()
{
    const int arraySize = static_cast<int>(mButtons.size());
    for (int i = 0; i < arraySize * 2; ++i)
    {
        mButtonUV[i] = { (i / arraySize) * 24, (i % arraySize) * 24, 24, 24 };
    }
}


void MiniMapWindow::setButtonPositions()
{
    constexpr Vector<int> buttonSize{ 24, 24 };
    constexpr Vector<int> buttonTransform{buttonSize.x + 6, 0};
    
    const int arraySize = static_cast<int>(mButtons.size());
    const int startPosition = (mButtonArea.w - (buttonTransform.x * arraySize)) / 2;

    for (int i{ 0 }; i < arraySize; ++i)
    {
        mButtons[i].rect = { startPosition + buttonTransform.x * i, mButtonArea.y + 3, buttonSize.x, buttonSize.y };
    }
}


bool MiniMapWindow::noButtonsSelected()
{
    for (auto& button : mButtons)
    {
        if (button.state == ButtonStatePressed)
        {
            return false;
        }
    }
    
    return true;
}
