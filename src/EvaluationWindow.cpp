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
#include "EvaluationWindow.h"


#include "Evaluation.h"


namespace
{
    const SDL_Rect BgRect{ 0, 0, 503, 256 };

    constexpr SDL_Rect YesNoRect{ 12, 50, 234, 70 };
    constexpr SDL_Rect OpinionRect{ 12, 128, 234, 115 };
    constexpr SDL_Rect StatisticsRect{ 256, 50, 234, 193 };

    constexpr Vector<int> ContentPanelPadding{ 2, 2 };
};



EvaluationWindow::EvaluationWindow(SDL_Renderer* renderer):
    mFont{ new Font("res/raleway-medium.ttf", 13) },
    mFontBold{ new Font("res/raleway-bold.ttf", 13) },
    mTexture(loadTexture(renderer, "images/EvalWindow.png")),
    mRenderer{ renderer },
    mStringRenderer{ renderer }
{
    size({ BgRect.w, BgRect.h });

    SDL_SetTextureColorMod(mFont->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(mFontBold->texture(), 0, 0, 0);
}


void EvaluationWindow::setEvaluation(const Evaluation& evaluation)
{
    mEvaluation = evaluation;
}


void EvaluationWindow::draw()
{
    const SDL_Rect rect{ area().x, area().y, area().width, area().height };
    SDL_RenderCopy(mRenderer, mTexture.texture, &BgRect, &rect);

    Point<int> yesNoPanelStart = Point<int>{ area().x + YesNoRect.x, area().y + YesNoRect.y } + ContentPanelPadding;
    mStringRenderer.drawString(*mFontBold, "Is the Mayor doing a good job?", yesNoPanelStart);

    yesNoPanelStart.y += mFontBold->height() + ContentPanelPadding.y * 2;
    std::string tempString = "Yes:  " + mEvaluation.goodyes;
    mStringRenderer.drawString(*mFont, tempString, yesNoPanelStart);

    yesNoPanelStart.y += mFont->height();
    tempString = "No:  " + mEvaluation.goodno;
    mStringRenderer.drawString(*mFont, tempString, yesNoPanelStart);
}


void EvaluationWindow::update()
{
    
}


void EvaluationWindow::onMouseDown(const Point<int>& position)
{
    
}


void EvaluationWindow::onMouseUp()
{
    
}
