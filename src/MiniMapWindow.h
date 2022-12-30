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
#include "Texture.h"
#include "Vector.h"

#include <array>
#include <cstdint>
#include <map>
#include <vector>

#include <SDL2/SDL.h>


class EffectMap;


class MiniMapWindow
{
public:

	enum class ButtonId
	{
		Normal,
		LandValue,
		Crime,
		FireProtection,
		PoliceProtection,
		PopulationDensity,
		PopulationGrowth,
		Pollution,
		TrafficDensity,
		TransportationNetwork,
		PowerGrid,
		Residential,
		Commercial,
		Industrial
	};

	static constexpr auto ButtonStateNormal{ 0 };
	static constexpr auto ButtonStatePressed{ 1 };

	static constexpr auto MiniTileSize{ 3 };
	static constexpr auto TileSize{ 16 };
	static constexpr auto ButtonAreaHeight{ 28 };

	using fnPointIntParam = void(*)(const Point<int>&);

public:
	MiniMapWindow() = delete;
	MiniMapWindow(const Point<int>& position, const Vector<int>& size);

	~MiniMapWindow();

	uint32_t id() const;

	void focusOnMapCoordBind(fnPointIntParam);
	void focusOnMapCoordUnbind(fnPointIntParam);

	void updateMapViewPosition(const Point<int>& position);
	void updateViewportSize(const Vector<int>& viewportSize);
	void updateTilePointedAt(const Point<int>& tilePointedAt);

	void linkEffectMap(ButtonId id, const EffectMap& map);

	void resetOverlayButtons();

	void hide();
	void show();

	bool hidden() const;

	void draw();
	void drawUI();

	void injectEvent(const SDL_Event& event);

private:
	void handleMouseEvent(const SDL_Event& event);
	void handleWindowEvent(const SDL_Event& event);
	void handleMouseMotion(const SDL_Event& event);

	void focusViewpoint(const Point<int>& point);

	void setButtonValues();
	void setButtonTextureUv();
	void setButtonPositions();

	bool noButtonsSelected();

	void initTexture(Texture& texture, const Vector<int>& dimensions);
	void initOverlayTextures();

	void drawCurrentOverlay();
    void drawPlainMap();
	void drawResidential();
	void drawCommercial();
	void drawIndustrial();
	void drawPowerMap();
    void drawLilTransMap();

private:
	struct ButtonMeta
	{
		SDL_Rect rect{};
		int state{};
		ButtonId id{ ButtonId::Normal };
	};

private:
	SDL_Window* mWindow{ nullptr };
	SDL_Renderer* mRenderer{ nullptr };

	Texture mTiles{};
	Texture mTexture{};
	Texture mButtonTextures{};

	SDL_Rect mSelector{};
	SDL_Rect mTileHighlight{ 0, 0, MiniTileSize, MiniTileSize };
	SDL_Rect mTileRect{ 0, 0, MiniTileSize, MiniTileSize };
	SDL_Rect mMinimapArea{};
	SDL_Rect mButtonArea{};

	uint32_t mWindowID{};

	Vector<int> mMapSize{};

	std::array<SDL_Rect, 28> mButtonUV{};
	std::array<ButtonMeta, 14> mButtons{};

	std::map<ButtonId, Texture> mOverlayTextures;
	std::map<ButtonId, const EffectMap*> mEffectMaps;

	std::vector<fnPointIntParam> mFocusOnTileCallbacks;

	ButtonId mButtonDownId{ ButtonId::Normal };

	bool mButtonDownInMinimapArea{ false };
	bool mHidden{ true };
};
