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


namespace
{
	const SDL_Rect bgRect{ 0, 0, 456, 422 };

	const SDL_Rect mainButtonArea{ 11, 373, 432, 35 };
	const SDL_Rect mainButtonDown{ 2, 426, 434, 36 };
	const SDL_Rect mainButtonUp{ 0, 438, 456, 60 };

	const SDL_Rect upButtonDown{ 470, 7, 13, 13 };
	const SDL_Rect downButtonDown{ 485, 7, 13, 13 };
};


BudgetWindow::BudgetWindow(SDL_Renderer* renderer, const StringRender& stringRenderer) :
	mRenderer(renderer),
	mStringRenderer(stringRenderer),
	mTitleFont(new Font("res/raleway-medium.ttf", 14)),
	mTexture(loadTexture(renderer, "images/budget.png"))
{}


BudgetWindow::~BudgetWindow()
{
	delete mTitleFont;
}

void BudgetWindow::reset()
{
	mAccepted = false;
}


void BudgetWindow::position(const Point<int> pos)
{
	mRect = { pos.x, pos.y, mRect.w, mRect.h };
}


void BudgetWindow::draw()
{
	SDL_RenderCopy(mRenderer, mTexture.texture, &bgRect, &mRect);

	SDL_Point mousePosition{};
	Uint32 leftButtonDown = SDL_GetMouseState(&mousePosition.x, &mousePosition.y) & SDL_BUTTON_LMASK;

	const SDL_Rect buttonArea{ mainButtonArea.x + mRect.x, mainButtonArea.y + mRect.y, mainButtonDown.w, mainButtonDown.h };

	if (SDL_PointInRect(&mousePosition, &buttonArea) && leftButtonDown)
	{
		SDL_RenderCopy(mRenderer, mTexture.texture, &mainButtonDown, &buttonArea);
		mAccepted = true;
	}
}
