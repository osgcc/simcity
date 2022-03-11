#pragma once

#include "SDL2/SDL.h"

#include "Texture.h"

class GraphWindow
{
public:
	GraphWindow() = delete;
	GraphWindow(SDL_Renderer*);
	~GraphWindow() = default;

	void position(const SDL_Point& position);
	void injectMouseDown(const SDL_Point& position);

	const SDL_Rect& rect() const { return mArea; }

	void draw();

private:
	SDL_Renderer& mRenderer;
	const Texture mTexture;
	SDL_Rect mArea{ 0, 0, 264, 287 };
};
