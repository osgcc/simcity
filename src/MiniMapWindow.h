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

#include "Point.h"
#include "Vector.h"

#include <cstdint>


struct SDL_Window;
struct SDL_Renderer;


class MiniMapWindow
{
public:
	MiniMapWindow() = delete;
	MiniMapWindow(const Point<int>& position, const Vector<int>& size);

	~MiniMapWindow();

	uint32_t id() const;

	void hide();
	void show();

	void draw();

private:
	SDL_Window* mWindow{ nullptr };
	SDL_Renderer* mRenderer{ nullptr };

	uint32_t mWindowID{};
};