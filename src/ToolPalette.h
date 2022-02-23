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
#pragma once

#include <array>

#include <SDL2/SDL.h>

#include "Point.h"
#include "Texture.h"

#include "w_tool.h"


class ToolPalette
{
public:
    ToolPalette() = delete;
    ToolPalette(const ToolPalette&) = delete;
    const ToolPalette& operator=(const ToolPalette&) = delete;

    ~ToolPalette();

    const int NoSelection = -1;
    
    const int NormalState = 0;
    const int PressedState = 1;
    const int DisabledState = 2;

public:
    ToolPalette(SDL_Renderer* renderer);

    Tool tool() const;
    const Texture& toolGost() const;
    const SDL_Rect& rect() const;

    void draw();
    void position(const Point<int>& position);
    void injectMouseClickPosition(const Point<int>& mousePosition);

private:
    struct ButtonMeta
    {
        SDL_Rect rect{};
        Tool tool{};

        int state{};

        Texture ghost{};
    };

private:
    void initToolbarUv();
    void loadToolGhosts();
    void setToolValues();
    void setButtonState(int buttonIndex, int buttonState);
    void drawBackground();

    void toolIndex(const int toolIndex);
    int toolIndex() const;

    std::array<SDL_Rect, 80> mToolButtonUV{};
    std::array<ButtonMeta, 20> mToolButtons{};

    SDL_Rect mRect{};

    SDL_Renderer* mRenderer{ nullptr };
    Texture texture{};

    int mSelectedIndex{ NoSelection };
    Tool mTool{ Tool::None };
};
