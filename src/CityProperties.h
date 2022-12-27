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

#include <string>

class CityProperties
{
public:
	CityProperties() = default;
	~CityProperties() = default;

	const std::string& CityName() const { return mCityName; }
	void CityName(const std::string& name) { mCityName = name; }
	
	int GameLevel() const { return mGameLevel; }
	void GameLevel(const int level) { mGameLevel = level; }

private:
	int mGameLevel{};
	std::string mCityName{};
};
