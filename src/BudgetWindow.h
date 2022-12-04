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

	const SDL_Rect& rect() const { return mRect; }

	void reset();

	void position(const Point<int>& pos);

	bool accepted() const { return mAccepted; }

	void injectMouseClickPosition(const SDL_Point& pos);
	void injectMouseUp();

	void draw() override;
	void update() override;

private:
	void handleMouseDown(const ButtonId id);

	Budget& mBudget;
	bool mAccepted{ false };

	SDL_Rect mRect{ 0, 0, 456, 422 };

	SDL_Renderer* mRenderer{ nullptr };
	const StringRender& mStringRenderer;

	Font* mFont{ nullptr };

	Texture mTexture{};

	ButtonId mButtonDownId{ ButtonId::None };
};