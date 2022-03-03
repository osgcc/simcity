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

#include "w_util.h"

#include <algorithm>
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
		{ TextPanelId::TransportRate, {383, 135, 41, 31} },

		{ TextPanelId::PoliceNeeded, {123, 167, 129, 31} },
		{ TextPanelId::PoliceAllocated, {253, 167, 129, 31} },
		{ TextPanelId::PoliceRate, {383, 167, 41, 31} },

		{ TextPanelId::FireNeeded, {123, 199, 129, 31} },
		{ TextPanelId::FireAllocated, {253, 199, 129, 31} },
		{ TextPanelId::FireRate, {383, 199, 41, 31} },

		{ TextPanelId::CashFlow, {138, 257, 285, 21} },
		{ TextPanelId::PreviousFunds, {138, 287, 285, 21} },
		{ TextPanelId::CurrentFunds, {138, 317, 285, 21} }
	};


	std::map<TextPanelId, SDL_Rect> TextPanelRects;
	std::map<TextPanelId, std::string> TextPanelText;
	std::map<TextPanelId, Point<int>> TextPanelAdjust;


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

	std::map<BudgetWindow::ButtonId, SDL_Rect> ButtonRects;
};


BudgetWindow::BudgetWindow(SDL_Renderer* renderer, const StringRender& stringRenderer, Budget& budget) :
	mBudget(budget),
	mRenderer(renderer),
	mStringRenderer(stringRenderer),
	mFont(new Font("res/raleway-medium.ttf", 14)),
	mTexture(loadTexture(renderer, "images/budget.png"))
{
	for (auto id : buttons)
	{
		ButtonRects[id] = ButtonLayout.at(id);
	}

	for (auto id : panels)
	{
		TextPanelRects[id] = TextPanelLayout.at(id);
	}

	SDL_SetTextureColorMod(mFont->texture(), 0, 0, 0);
}


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

	for (auto id : panels)
	{
		TextPanelRects[id] =
		{
			TextPanelLayout.at(id).x + pos.x,
			TextPanelLayout.at(id).y + pos.y,
			TextPanelLayout.at(id).w,
			TextPanelLayout.at(id).h
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
		mBudget.TaxRate(std::clamp(mBudget.TaxRate() + 1, Budget::MinTaxRate, Budget::MaxTaxRate));
		break;

	case ButtonId::TaxRateDown:
		mBudget.TaxRate(std::clamp(mBudget.TaxRate() - 1, Budget::MinTaxRate, Budget::MaxTaxRate));
		break;

	case ButtonId::TransportUp:
		mBudget.RoadPercent(std::clamp(mBudget.RoadPercent() + Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
		break;

	case ButtonId::TransportDown:
		mBudget.RoadPercent(std::clamp(mBudget.RoadPercent() - Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
		break;

	case ButtonId::PoliceUp:
		mBudget.PolicePercent(std::clamp(mBudget.PolicePercent() + Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
		break;

	case ButtonId::PoliceDown:
		mBudget.PolicePercent(std::clamp(mBudget.PolicePercent() - Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
		break;

	case ButtonId::FireUp:
		mBudget.FirePercent(std::clamp(mBudget.FirePercent() + Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
		break;

	case ButtonId::FireDown:
		mBudget.FirePercent(std::clamp(mBudget.FirePercent() - Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
		break;

	case ButtonId::Accept:
		mAccepted = true;
		break;

	default:
		break;
	}

	update();
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

	for (auto id : buttons)
	{
		if (id == mButtonDownId)
		{
			SDL_RenderCopy(mRenderer, mTexture.texture, &ButtonDownTable.at(id), &ButtonRects[id]);
		}
	}

	for (auto id : panels)
	{
		const std::string str = TextPanelText[id];
		SDL_Rect& rect = TextPanelRects[id];
		mStringRenderer.drawString(*mFont, str, { rect.x + TextPanelAdjust[id].x, rect.y + TextPanelAdjust[id].y });
	}
}


void BudgetWindow::update()
{
	TextPanelText[TextPanelId::CashFlow] = NumberToDollarDecimal(mBudget.CashFlow());
	TextPanelText[TextPanelId::CurrentFunds] = NumberToDollarDecimal(mBudget.CurrentFunds());

	TextPanelText[TextPanelId::PreviousFunds] = NumberToDollarDecimal(mBudget.PreviousFunds());
	TextPanelText[TextPanelId::TaxesCollected] = NumberToDollarDecimal(mBudget.TaxIncome());
	TextPanelText[TextPanelId::TaxRate] = std::to_string(mBudget.TaxRate()) + "%";

	TextPanelText[TextPanelId::FireAllocated] = NumberToDollarDecimal(mBudget.FireFundsGranted());
	TextPanelText[TextPanelId::FireNeeded] = NumberToDollarDecimal(mBudget.FireFundsNeeded());
	TextPanelText[TextPanelId::FireRate] = std::to_string(static_cast<int>(mBudget.FirePercent() * 100.0f)) + "%";

	TextPanelText[TextPanelId::PoliceAllocated] = NumberToDollarDecimal(mBudget.PoliceFundsGranted());
	TextPanelText[TextPanelId::PoliceNeeded] = NumberToDollarDecimal(mBudget.PoliceFundsNeeded());
	TextPanelText[TextPanelId::PoliceRate] = std::to_string(static_cast<int>(mBudget.PolicePercent() * 100.0f)) + "%";

	TextPanelText[TextPanelId::TransportAllocated] = NumberToDollarDecimal(mBudget.RoadFundsGranted());
	TextPanelText[TextPanelId::TransportNeeded] = NumberToDollarDecimal(mBudget.RoadFundsNeeded());
	TextPanelText[TextPanelId::TransportRate] = std::to_string(static_cast<int>(mBudget.RoadPercent() * 100.0f)) + "%";

	for (auto id : panels)
	{
		TextPanelAdjust[id] = { TextPanelLayout.at(id).w - mFont->width(TextPanelText[id]), TextPanelLayout.at(id).h / 2 - mFont->height() / 2 };
	}
}
