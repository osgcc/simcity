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

	enum class TextPanelId
	{
		TaxRate,
		TaxesCollected,
		TransportNeeded,
		TransportAllocated,
		TransportRate,
		PoliceNeeded,
		PoliceAllocated,
		PoliceRate,
		FireNeeded,
		FireAllocated,
		FireRate,
		CashFlow,
		PreviousFunds,
		CurrentFunds
	};


	const TextPanelId panels[]
	{
		TextPanelId::TaxRate,
		TextPanelId::TaxesCollected,
		TextPanelId::TransportNeeded,
		TextPanelId::TransportAllocated,
		TextPanelId::TransportRate,
		TextPanelId::PoliceNeeded,
		TextPanelId::PoliceAllocated,
		TextPanelId::PoliceRate,
		TextPanelId::FireNeeded,
		TextPanelId::FireAllocated,
		TextPanelId::FireRate,
		TextPanelId::CashFlow,
		TextPanelId::PreviousFunds,
		TextPanelId::CurrentFunds
	};


	const std::map<TextPanelId, SDL_Rect> TextPanelLayout
	{
		{ TextPanelId::TaxRate, {138, 39, 285, 21} },
		{ TextPanelId::TaxesCollected, {138, 70, 285, 21} },

		{ TextPanelId::TransportNeeded, {123, 135, 129, 31} },
		{ TextPanelId::TransportAllocated, {253, 135, 129, 31} },
		{ TextPanelId::TransportRate, {383, 135, 129, 31} },

		{ TextPanelId::PoliceNeeded, {123, 167, 129, 31} },
		{ TextPanelId::PoliceAllocated, {253, 167, 129, 31} },
		{ TextPanelId::PoliceRate, {383, 167, 129, 31} },

		{ TextPanelId::FireNeeded, {123, 199, 129, 3} },
		{ TextPanelId::FireAllocated, {253, 199, 129, 31} },
		{ TextPanelId::FireRate, {383, 199, 129, 31} },

		{ TextPanelId::CashFlow, {138, 257, 285, 21} },
		{ TextPanelId::PreviousFunds, {138, 287, 285, 21} },
		{ TextPanelId::CurrentFunds, {138, 317, 285, 21} }
	};


	const BudgetWindow::ButtonId buttons[]
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

	const std::map<BudgetWindow::ButtonId, const SDL_Rect> ButtonLayout
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

	std::map<BudgetWindow::ButtonId, SDL_Rect> ButtonRects
	{
		{ BudgetWindow::ButtonId::None, {} },
		{ BudgetWindow::ButtonId::TaxRateUp, ButtonLayout.at(BudgetWindow::ButtonId::TaxRateUp) },
		{ BudgetWindow::ButtonId::TaxRateDown, ButtonLayout.at(BudgetWindow::ButtonId::TaxRateDown) },
		{ BudgetWindow::ButtonId::TransportUp, ButtonLayout.at(BudgetWindow::ButtonId::TransportUp) },
		{ BudgetWindow::ButtonId::TransportDown, ButtonLayout.at(BudgetWindow::ButtonId::TransportDown) },
		{ BudgetWindow::ButtonId::PoliceUp, ButtonLayout.at(BudgetWindow::ButtonId::PoliceUp) },
		{ BudgetWindow::ButtonId::PoliceDown, ButtonLayout.at(BudgetWindow::ButtonId::PoliceDown) },
		{ BudgetWindow::ButtonId::FireUp, ButtonLayout.at(BudgetWindow::ButtonId::FireUp) },
		{ BudgetWindow::ButtonId::FireDown, ButtonLayout.at(BudgetWindow::ButtonId::FireDown) },
		{ BudgetWindow::ButtonId::Accept, ButtonLayout.at(BudgetWindow::ButtonId::Accept) }
	};
};


BudgetWindow::BudgetWindow(SDL_Renderer* renderer, const StringRender& stringRenderer, Budget& budget) :
	mBudget(budget),
	mRenderer(renderer),
	mStringRenderer(stringRenderer),
	mFont(new Font("res/raleway-medium.ttf", 14)),
	mTexture(loadTexture(renderer, "images/budget.png"))
{}


BudgetWindow::~BudgetWindow()
{
	delete mFont;
}


void BudgetWindow::reset()
{
	mAccepted = false;
}


void BudgetWindow::position(const Point<int>& pos)
{
	mRect = { pos.x, pos.y, mRect.w, mRect.h };

	for (auto id : buttons)
	{
		ButtonRects[id] =
		{
			ButtonLayout.at(id).x + pos.x,
			ButtonLayout.at(id).y + pos.y,
			ButtonLayout.at(id).w,
			ButtonLayout.at(id).h
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
	for (auto id : buttons)
	{
		if (id != ButtonId::None && SDL_PointInRect(&pos, &ButtonRects[id]))
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

	mStringRenderer.drawString(*mFont, std::to_string(mBudget.CurrentFunds()), { mRect.x + 10, mRect.y + 20 }, { 0, 0, 0, 255 });

	for (auto id : buttons)
	{
		if (id == mButtonDownId)
		{
			SDL_RenderCopy(mRenderer, mTexture.texture, &ButtonDownTable.at(id), &ButtonRects[id]);
		}
	}
}


void BudgetWindow::update()
{

}
