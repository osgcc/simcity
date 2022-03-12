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

#include "Graph.h"
#include "Texture.h"


class GraphWindow
{
public:
	GraphWindow() = delete;
	GraphWindow(SDL_Renderer*);
	~GraphWindow() = default;

	void move(const SDL_Point& position);
	void injectMouseDown(const SDL_Point& position);
	void injectMouseUp();
	void injectMouseMotion(const SDL_Point& delta);

	const SDL_Rect& rect() const { return mArea; }

	void draw();

	void update();

private:
	void fillGraphPoints(Graph::PointsList&, const GraphHistory&);

	SDL_Renderer& mRenderer;
	const Texture mTexture;
	Texture mGraphTexture;
	SDL_Rect mArea{ 0, 0, 264, 287 };

	bool mDragging{ false };
};
