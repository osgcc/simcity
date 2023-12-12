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
#include "ToolPalette.h"

#include "Vector.h"

#include <stdexcept>
#include <string>


namespace
{
    constexpr Vector<int> ToolPaletteSize{ 118, 235 };
    const SDL_Rect bgRect{ 0, 0, ToolPaletteSize.x, ToolPaletteSize.y };
};



ToolPalette::ToolPalette(SDL_Renderer* renderer) :
    mRenderer(renderer),
    mIcons(loadTexture(mRenderer, "icons/buttons.png")),
    mBackground(loadTexture(mRenderer, "images/ToolPalette.png"))
{
    size(ToolPaletteSize);
    initToolbarUv();

    mToolButtons[10].state = DisabledState;
    mToolButtons[11].state = DisabledState;
    mToolButtons[18].state = DisabledState;
    mToolButtons[19].state = DisabledState;

    setToolValues();
    loadToolGhosts();
    show();
}


ToolPalette::~ToolPalette()
{
    for (auto& item : mToolButtons)
    {
        if (item.ghost.texture)
        {
            SDL_DestroyTexture(item.ghost.texture);
        }
    }

    if (mIcons.texture)
    {
        SDL_DestroyTexture(mIcons.texture);
    }
}


void ToolPalette::draw()
{
    drawBackground();

    for (size_t i = 0; i < 20; ++i)
    {
        if (mToolButtons[i].tool == Tool::None) { continue; }
        SDL_RenderCopy(mRenderer, mIcons.texture, &mToolButtonUV[i + (mToolButtons[i].state * 20)], &mToolButtons[i].rect);
    }
}


void ToolPalette::update()
{}


void ToolPalette::onMoved(const Vector<int>&)
{
    updateButtonPositions();
}


void ToolPalette::onPositionChanged(const Point<int>& position)
{
    updateButtonPositions();
}


void ToolPalette::updateButtonPositions()
{
    for (int i = 0; i < 20; ++i)
    {
        mToolButtons[i].rect =
        {
            (((i % 3) * 32) + (i % 3) * 2) + area().x + 8,
            (((i / 3) * 32) + (i / 3) * 2) + area().y + 25,
            32, 32
        };
    }
}


void ToolPalette::toolIndex(const int toolIndex)
{
    if (mSelectedIndex != NoSelection)
    {
        setButtonState(mSelectedIndex, NormalState);
        mTool = Tool::None;
    }

    setButtonState(toolIndex, PressedState);
    mSelectedIndex = toolIndex;
    mTool = mToolButtons[mSelectedIndex].tool;
}


int ToolPalette::toolIndex() const
{
    return mSelectedIndex;
}



Tool ToolPalette::tool() const
{
    return mTool;
}


const Texture& ToolPalette::toolGost() const
{
    return mToolButtons[mSelectedIndex].ghost;
}


void ToolPalette::onMouseDown(const Point<int>& position)
{
    for (int i = 0; i < 20; ++i)
    {
        const SDL_Rect& buttonRect = mToolButtons[i].rect;
        if (position.x >= buttonRect.x && position.x <= buttonRect.x + buttonRect.w &&
            position.y >= buttonRect.y && position.y <= buttonRect.y + buttonRect.h)
        {
            const int buttonState = mToolButtons[i].state;
            if (buttonState == PressedState || buttonState == DisabledState)
            {
                continue;
            }

            toolIndex(i);
            return;
        }
    }
}


void ToolPalette::initToolbarUv()
{
    for (int i = 0; i < 80; ++i)
    {
        mToolButtonUV[i] = { (i / 20) * 32, (i % 20) * 32, 32, 32 };
    }
}


void ToolPalette::loadToolGhosts()
{
    mToolButtons[0].ghost = loadTexture(mRenderer, "images/res.xpm");
    mToolButtons[1].ghost = loadTexture(mRenderer, "images/com.xpm");
    mToolButtons[2].ghost = loadTexture(mRenderer, "images/ind.xpm");
    mToolButtons[3].ghost = loadTexture(mRenderer, "images/fire.xpm");
    //mToolButtons[4].ghost = loadTexture(mRenderer, ""); // Query
    mToolButtons[5].ghost = loadTexture(mRenderer, "images/police.xpm");
    //mToolButtons[6].ghost = loadTexture(mRenderer, ""); // Wire
    //mToolButtons[7].ghost = loadTexture(mRenderer, ""); // Bulldoze
    //mToolButtons[8].ghost = loadTexture(mRenderer, ""); // Rail
    //mToolButtons[9].ghost = loadTexture(mRenderer, ""); // Road
    //mToolButtons[10].ghost = loadTexture(mRenderer, ""); // Unused
    //mToolButtons[11].ghost = loadTexture(mRenderer, ""); // Unused
    mToolButtons[12].ghost = loadTexture(mRenderer, "images/stadium.xpm");
    //mToolButtons[13].ghost = loadTexture(mRenderer, ""); // Park
    mToolButtons[14].ghost = loadTexture(mRenderer, "images/seaport.xpm");
    mToolButtons[15].ghost = loadTexture(mRenderer, "images/coal.xpm");
    mToolButtons[16].ghost = loadTexture(mRenderer, "images/nuclear.xpm");
    mToolButtons[17].ghost = loadTexture(mRenderer, "images/airport.xpm");
    //mToolButtons[18].ghost = loadTexture(mRenderer, ""); // Network
    //mToolButtons[19].ghost = loadTexture(mRenderer, ""); // Unused

    for (auto& item : mToolButtons)
    {
        if (item.ghost.texture)
        {
            SDL_SetTextureBlendMode(item.ghost.texture, SDL_BLENDMODE_BLEND);
            if(SDL_SetTextureAlphaMod(item.ghost.texture, 125))
            {
                throw std::runtime_error(std::string("Unable to set alpha mod: ") + SDL_GetError());
            }
        }
    }
}


void ToolPalette::setToolValues()
{
    mToolButtons[0].tool = Tool::Residential;
    mToolButtons[1].tool = Tool::Commercial;
    mToolButtons[2].tool = Tool::Industrial;
    mToolButtons[3].tool = Tool::Fire;
    mToolButtons[4].tool = Tool::Query;
    mToolButtons[5].tool = Tool::Police;
    mToolButtons[6].tool = Tool::Wire;
    mToolButtons[7].tool = Tool::Bulldoze;
    mToolButtons[8].tool = Tool::Rail;
    mToolButtons[9].tool = Tool::Road;
    mToolButtons[10].tool = Tool::None; // Unused
    mToolButtons[11].tool = Tool::None; // Unused
    mToolButtons[12].tool = Tool::Stadium;
    mToolButtons[13].tool = Tool::Park;
    mToolButtons[14].tool = Tool::Seaport;
    mToolButtons[15].tool = Tool::Coal;
    mToolButtons[16].tool = Tool::Nuclear;
    mToolButtons[17].tool = Tool::Airport;
    mToolButtons[18].tool = Tool::None;
    mToolButtons[19].tool = Tool::None; // Unused
}


void ToolPalette::setButtonState(int buttonIndex, int buttonState)
{
    mToolButtons[buttonIndex].state = buttonState;
}


void ToolPalette::drawBackground()
{
    const SDL_Rect rect{ area().x, area().y, area().width, area().height };
    SDL_RenderCopy(mRenderer, mBackground.texture, &bgRect, &rect);
}


void ToolPalette::cancelTool()
{
    for (auto& item : mToolButtons)
    {
        if (item.state == DisabledState) { continue; }
        item.state = NormalState;
    }

    mSelectedIndex = NoSelection;
    mTool = Tool::None;
}
