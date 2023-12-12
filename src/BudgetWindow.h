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
#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include "Budget.h"
#include "Font.h"
#include "Point.h"
#include "StringRender.h"
#include "Texture.h"
#include "WindowBase.h"


class BudgetWindow : public WindowBase
{
public:
	enum class ButtonId
	{
		None,
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

	static constexpr auto ButtonStateNormal = 0;
	static constexpr auto ButtonStatePressed = 1;

public:
	BudgetWindow() = delete;
	BudgetWindow(const BudgetWindow&) = delete;
	const BudgetWindow& operator=(const BudgetWindow&) = delete;

	BudgetWindow(SDL_Renderer* renderer, const StringRender& stringRenderer, Budget& budget);
	~BudgetWindow() override;

	void reset();

	bool accepted() const { return mAccepted; }

	void draw() override;
	void update() override;

private:
	void onMouseDown(const Point<int>& pos) override;
	void onMouseUp() override;

	void handleMouseDown(const ButtonId id);

	void onPositionChanged(const Point<int>& pos) override;

	Budget& mBudget;
	bool mAccepted{ false };

	SDL_Renderer* mRenderer{ nullptr };
	const StringRender& mStringRenderer;

	std::unique_ptr<Font> mFont{ nullptr };

	Texture mTexture{};

	ButtonId mButtonDownId{ ButtonId::None };
};
