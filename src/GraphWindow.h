// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright � 2022 Leeor Dicker
//
// Portions Copyright � 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include <SDL2/SDL.h>

#include "Graph.h"
#include "Texture.h"
#include "WindowBase.h"


class GraphWindow : public WindowBase
{
public:
	GraphWindow() = delete;
	GraphWindow(const GraphWindow&) = delete;
	const GraphWindow& operator=(const GraphWindow&) = delete;

	GraphWindow(SDL_Renderer*);
	~GraphWindow() override = default;

	const SDL_Rect& rect() const { return mArea; }

	void injectMouseDown(const Point<int>& position) override;
	void injectMouseUp() override;
	void injectMouseMotion(const Vector<int>& delta) override;

	void draw() override;
	void update() override;

private:
	void fillGraphPoints(Graph::PointsList&, const GraphHistory&);

	void onMoved(const Vector<int>&) override;
	void onPositionChanged(const Point<int>&) override;


	SDL_Renderer& mRenderer;
	const Texture mTexture;
	Texture mGraphTexture;
	SDL_Rect mArea{ 0, 0, 264, 287 };

	bool mDragging{ false };
};
