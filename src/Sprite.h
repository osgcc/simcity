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

#include <string>
#include <vector>

struct SimSprite
{
	enum class Type
	{
		Train,
		Helicopter,
		Airplane,
		Ship,
		Monster,
		Tornado,
		Explosion
	};

	std::string name{};

	Type type{ 0 };
	int frame{ 0 };
	
	Point<int> position{};
	Point<int> origin{};
	Point<int> offset{};
	Vector<int> hot{};
	Point<int> destination{};

	Vector<int> size{};
	
	int count{ 0 };
	int sound_count{ 0 };
	int dir{ 0 };
	int new_dir{ 0 };
	int step{ 0 };
	int flag{ 0 };
	int turn{ 0 };
	int accel{ 0 };
	int speed{ 0 };

	bool active{ false };

	std::vector<Texture> frames;
};


Point<int>& crashPosition();
void crashPosition(const Point<int>& position);

SimSprite* getSprite(SimSprite::Type type);
void drawSprites();
void destroyAllSprites();
void updateSprites();

void generateHelicopter(const Point<int>& position);
void generateAirplane(const Point<int>& position);
void generateShip();
void generateTrain(const Point<int>& position);

void makeExplosionAt(const Point<int>& position);
void generateExplosion(const Point<int>& position);
void generateTornado();
void generateMonster();
