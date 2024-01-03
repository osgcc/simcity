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
};



EvaluationWindow::EvaluationWindow(SDL_Renderer* renderer):
    mFont{ new Font("res/raleway-medium.ttf", 14) },
    mTexture(loadTexture(renderer, "images/EvalWindow.png")),
    mRenderer{ renderer }
{
    size({ BgRect.w, BgRect.h });
}


void EvaluationWindow::setEvaluation(const Evaluation& evaluation)
{
    mEvaluation = evaluation;
}


void EvaluationWindow::draw()
{
    const SDL_Rect rect{ area().x, area().y, area().width, area().height };
    SDL_RenderCopy(mRenderer, mTexture.texture, &BgRect, &rect);


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
