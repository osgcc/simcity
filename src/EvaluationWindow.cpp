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




EvaluationWindow::EvaluationWindow(SDL_Renderer* renderer):
    mRenderer{ renderer }
{
    size({ 100, 200 });
    mTexture.texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, size().x, size().y);
}


void EvaluationWindow::draw()
{
    const SDL_Rect rect{area().x, area().y, area().width, area().height };
    //SDL_RenderCopy(mRenderer, mTexture.texture, &bgRect, &rect);
    
    SDL_RenderDrawRect(mRenderer, &rect);
}


void EvaluationWindow::update()
{
    
}


void EvaluationWindow::injectMouseDown(const Point<int>& position)
{
    
}


void EvaluationWindow::injectMouseUp()
{
    
}
