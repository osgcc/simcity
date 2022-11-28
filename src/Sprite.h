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
	Point<int> hot{};
	Point<int> destination{};

	Vector<int> size{};
	
	int count{ 0 };
	int sound_count{ 0 };
	int dir{ 0 };
	int new_dir{ 0 };
	int step{ 0 };
	int flag{ 0 };
	int control{ 0 };
	int turn{ 0 };
	int accel{ 0 };
	int speed{ 0 };

	bool active{ false };

	std::vector<Texture> frames;
};

extern std::vector<SimSprite> Sprites;

SimSprite* GetSprite(SimSprite::Type type);

void MoveObjects();
void DoTrainSprite(SimSprite& sprite);
void DestroyAllSprites();

void GenerateCopter(int x, int y);
void GeneratePlane(int x, int y);
void GenerateShip();
void GenerateTrain(int x, int y);

void MakeExplosionAt(int x, int y);
void MakeExplosion(int x, int y);
void MakeTornado();
void MakeMonster();
void DrawObjects();
