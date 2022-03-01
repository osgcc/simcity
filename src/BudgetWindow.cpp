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

	SDL_Rect TaxRateBox{ 138, 39, 285, 21 };
	const SDL_Rect TaxRateBoxPosition = TaxRateBox;

	SDL_Rect TaxCollectedBox{ 138, 70, 285, 21 };
	const SDL_Rect TaxCollectedBoxPosition = TaxCollectedBox;

	const BudgetWindow::ButtonId ids[] =
	{
		BudgetWindow::ButtonId::None,
		BudgetWindow::ButtonId::TaxRateUp,
		BudgetWindow::ButtonId::TaxRateDown,
		BudgetWindow::ButtonId::TransportUp,
		BudgetWindow::ButtonId::TransportDown,
		BudgetWindow::ButtonId::PoliceUp,
		BudgetWindow::ButtonId::PoliceDown,
		BudgetWindow::ButtonId::FireUp,
		BudgetWindow::ButtonId::FireDown,
		BudgetWindow::ButtonId::Accept
	};

	const SDL_Rect upArrowButtonDown{ 470, 7, 13, 13 };
	const SDL_Rect downArrowButtonDown{ 485, 7, 13, 13 };

	const std::map<BudgetWindow::ButtonId, SDL_Rect> ButtonDownTable
	{
		{ BudgetWindow::ButtonId::None, {} },
		{ BudgetWindow::ButtonId::TaxRateUp, upArrowButtonDown },
		{ BudgetWindow::ButtonId::TaxRateDown, downArrowButtonDown },
		{ BudgetWindow::ButtonId::TransportUp, upArrowButtonDown },
		{ BudgetWindow::ButtonId::TransportDown, downArrowButtonDown },
		{ BudgetWindow::ButtonId::PoliceUp, upArrowButtonDown },
		{ BudgetWindow::ButtonId::PoliceDown, downArrowButtonDown },
		{ BudgetWindow::ButtonId::FireUp, upArrowButtonDown },
		{ BudgetWindow::ButtonId::FireDown, downArrowButtonDown },
		{ BudgetWindow::ButtonId::Accept, mainButtonDown }
	};

	std::map<BudgetWindow::ButtonId, const SDL_Rect> buttonPositions
	{
		{ BudgetWindow::ButtonId::None, {} },
		{ BudgetWindow::ButtonId::TaxRateUp, { 431, 36, 13, 13 } },
		{ BudgetWindow::ButtonId::TaxRateDown, { 431, 50, 13, 13 } },
		{ BudgetWindow::ButtonId::TransportUp, { 431, 137, 13, 13 } },
		{ BudgetWindow::ButtonId::TransportDown, { 431, 151, 13, 13 } },
		{ BudgetWindow::ButtonId::PoliceUp, { 431, 168, 13, 13 } },
		{ BudgetWindow::ButtonId::PoliceDown, { 431, 182, 13, 13 } },
		{ BudgetWindow::ButtonId::FireUp, { 431, 200, 13, 13 } },
		{ BudgetWindow::ButtonId::FireDown, { 431, 214, 13, 13 } },
		{ BudgetWindow::ButtonId::Accept, { 11, 373, 434, 36 } }
	};

	std::map<BudgetWindow::ButtonId, SDL_Rect> buttonRects
	{
		{ BudgetWindow::ButtonId::None, {} },
		{ BudgetWindow::ButtonId::TaxRateUp, buttonPositions[BudgetWindow::ButtonId::TaxRateUp] },
		{ BudgetWindow::ButtonId::TaxRateDown, buttonPositions[BudgetWindow::ButtonId::TaxRateDown] },
		{ BudgetWindow::ButtonId::TransportUp, buttonPositions[BudgetWindow::ButtonId::TransportUp] },
		{ BudgetWindow::ButtonId::TransportDown, buttonPositions[BudgetWindow::ButtonId::TransportDown] },
		{ BudgetWindow::ButtonId::PoliceUp, buttonPositions[BudgetWindow::ButtonId::PoliceUp] },
		{ BudgetWindow::ButtonId::PoliceDown, buttonPositions[BudgetWindow::ButtonId::PoliceDown] },
		{ BudgetWindow::ButtonId::FireUp, buttonPositions[BudgetWindow::ButtonId::FireUp] },
		{ BudgetWindow::ButtonId::FireDown, buttonPositions[BudgetWindow::ButtonId::FireDown] },
		{ BudgetWindow::ButtonId::Accept, buttonPositions[BudgetWindow::ButtonId::Accept] }
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


void BudgetWindow::handleMouseDown(const ButtonId id)
{
	switch (id)
	{
	case ButtonId::None:
		break;

	case ButtonId::TaxRateUp:
		break;

	case ButtonId::TaxRateDown:
		break;

	case ButtonId::TransportUp:
		break;

	case ButtonId::TransportDown:
		break;

	case ButtonId::PoliceUp:
		break;

	case ButtonId::PoliceDown:
		break;

	case ButtonId::FireUp:
		break;

	case ButtonId::FireDown:
		break;

	case ButtonId::Accept:
		mAccepted = true;
		break;

	default:
		break;
	}
}


void BudgetWindow::injectMouseClickPosition(const SDL_Point& pos)
{
	for (auto id : ids)
	{
		if (id != ButtonId::None && SDL_PointInRect(&pos, &buttonRects[id]))
		{
			handleMouseDown(id);
			mButtonDownId = id;
			return;
		}
	}
}


void BudgetWindow::injectMouseUp()
{
	mButtonDownId = ButtonId::None;
}


void BudgetWindow::draw()
{
	SDL_RenderCopy(mRenderer, mTexture.texture, &bgRect, &mRect);

	for (auto id : ids)
	{
		if (id == mButtonDownId)
		{
			SDL_RenderCopy(mRenderer, mTexture.texture, &ButtonDownTable.at(id), &buttonRects[id]);
		}
	}
}
