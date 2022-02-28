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

#include <map>

namespace
{
	const SDL_Rect bgRect{ 0, 0, 456, 422 };

	const SDL_Rect mainButtonDown{ 2, 426, 434, 36 };
	const SDL_Rect mainButtonUp{ 0, 438, 456, 60 };

	const SDL_Rect upArrowButtonDown{ 470, 7, 13, 13 };
	const SDL_Rect downArrowButtonDown{ 485, 7, 13, 13 };

	SDL_Rect TaxRateBox{ 138, 39, 285, 21 };
	const SDL_Rect TaxRateBoxPosition = TaxRateBox;

	SDL_Rect TaxCollectedBox{ 138, 70, 285, 21 };
	const SDL_Rect TaxCollectedBoxPosition = TaxCollectedBox;

	enum class ButtonId
	{
		TaxRateUp,
		TaxRateDown,
		TransportUp,
		TransportDown,
		PoliceUp,
		PoliceDown,
		FireUp,
		FireDown,
		Accept
	};

	const ButtonId ids[] =
	{
		ButtonId::TaxRateUp,
		ButtonId::TaxRateDown,
		ButtonId::TransportUp,
		ButtonId::TransportDown,
		ButtonId::PoliceUp,
		ButtonId::PoliceDown,
		ButtonId::FireUp,
		ButtonId::FireDown,
		ButtonId::Accept
	};

	std::map<ButtonId, const SDL_Rect> buttonPositions
	{
		{ ButtonId::TaxRateUp, { 431, 36, 13, 13 } },
		{ ButtonId::TaxRateDown, { 431, 50, 13, 13 } },
		{ ButtonId::TransportUp, { 431, 137, 13, 13 } },
		{ ButtonId::TransportDown, { 431, 151, 13, 13 } },
		{ ButtonId::PoliceUp, { 431, 168, 13, 13 } },
		{ ButtonId::PoliceDown, { 431, 182, 13, 13 } },
		{ ButtonId::FireUp, { 431, 200, 13, 13 } },
		{ ButtonId::FireDown, { 431, 214, 13, 13 } },
		{ ButtonId::Accept, { 11, 373, 434, 36 } }
	};

	std::map<ButtonId, SDL_Rect> buttonRects
	{
		{ ButtonId::TaxRateUp, buttonPositions[ButtonId::TaxRateUp] },
		{ ButtonId::TaxRateDown, buttonPositions[ButtonId::TaxRateDown] },
		{ ButtonId::TransportUp, buttonPositions[ButtonId::TransportUp] },
		{ ButtonId::TransportDown, buttonPositions[ButtonId::TransportDown] },
		{ ButtonId::PoliceUp, buttonPositions[ButtonId::PoliceUp] },
		{ ButtonId::PoliceDown, buttonPositions[ButtonId::PoliceDown] },
		{ ButtonId::FireUp, buttonPositions[ButtonId::FireUp] },
		{ ButtonId::FireDown, buttonPositions[ButtonId::FireDown] },
		{ ButtonId::Accept, buttonPositions[ButtonId::Accept] }
	};
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


void BudgetWindow::position(const Point<int>& pos)
{
	mRect = { pos.x, pos.y, mRect.w, mRect.h };

	for (auto id : ids)
	{
		buttonRects[id] =
		{
			buttonPositions[id].x + pos.x,
			buttonPositions[id].y + pos.y,
			buttonPositions[id].w,
			buttonPositions[id].h
		};
	}
}


void BudgetWindow::injectMouseDown(const SDL_Point& pos)
{
	if (SDL_PointInRect(&pos, &buttonRects[ButtonId::Accept]))
	{
		SDL_RenderCopy(mRenderer, mTexture.texture, &mainButtonDown, &buttonRects[ButtonId::Accept]);
		mAccepted = true;
	}
}


void BudgetWindow::draw()
{
	SDL_RenderCopy(mRenderer, mTexture.texture, &bgRect, &mRect);

	for (auto id : ids)
	{
		SDL_RenderDrawRect(mRenderer, &buttonRects[id]);
	}
}
