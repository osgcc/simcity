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

class StringRender final
{
public:
	StringRender() = delete;
	StringRender(const StringRender&) = delete;
	const StringRender& operator=(const StringRender&) = delete;

	StringRender(SDL_Renderer* renderer);
	~StringRender() = default;

	void drawString(Font& font, std::string_view text, const Point<int>& position) const;

private:

	SDL_Renderer* mRenderer;
};
