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
