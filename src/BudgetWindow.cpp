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
#include "BudgetWindow.h"

BudgetWindow::BudgetWindow(SDL_Renderer* renderer, const StringRender& stringRenderer) :
	mRenderer(renderer),
	mStringRenderer(stringRenderer),
	mTitleFont(new Font("res/raleway-black.ttf", 18)),
	mTexture(loadTexture(renderer, "images/budget.png"))
{}


BudgetWindow::~BudgetWindow()
{
	delete mTitleFont;
}


void BudgetWindow::position(const Point<int> pos)
{
	mRect = { pos.x, pos.y, mRect.w, mRect.h };
}


void BudgetWindow::draw()
{
	const SDL_Rect bgRect{ 0, 0, 456, 344 };
	SDL_RenderCopy(mRenderer, mTexture.texture, &bgRect, &mRect);

	const Point<int> drawPoint{ mRect.x + mRect.w / 2 - mTitleFont->width(Title) / 2, mRect.y + 5 };

	mStringRenderer.drawString(*mTitleFont, Title, drawPoint, { 12, 64, 154, 255 });
}
