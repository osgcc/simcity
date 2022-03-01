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

#include "Font.h"
#include "Point.h"
#include "StringRender.h"
#include "Texture.h"


class BudgetWindow
{
public:
	const std::string Title{ "Annual Fiscal Budget" };

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

	const int ButtonStateNormal = 0;
	const int ButtonStatePressed = 1;

public:
	BudgetWindow() = delete;
	BudgetWindow(const BudgetWindow&) = delete;
	const BudgetWindow& operator=(const BudgetWindow&) = delete;

	BudgetWindow(SDL_Renderer* renderer, const StringRender& stringRenderer);
	~BudgetWindow();

	const SDL_Rect& rect() const { return mRect; }

	void reset();

	void position(const Point<int>& pos);

	bool accepted() const { return mAccepted; }

	void injectMouseClickPosition(const SDL_Point& pos);
	void injectMouseUp();

	void draw();

private:
	void handleMouseDown(const ButtonId id);

	bool mAccepted{ false };
	SDL_Rect mRect{ 0, 0, 456, 422 };

	SDL_Renderer* mRenderer{ nullptr };
	const StringRender& mStringRenderer;

	Font* mTitleFont;

	Texture mTexture{};

	ButtonId mButtonDownId{ ButtonId::None };
};