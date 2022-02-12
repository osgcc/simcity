#pragma once

#include "Date.h"
#include "Graph.h"
#include "Sprite.h"
#include "view.h"

struct Sim
{
	SimView editor{};
	SimView map{};
	Graph graph{};
	SimDate date{};
	SimSprite sprite{};
	Ink overlay{};

	int editors{ 0 };
	int maps{ 0 };
	int graphs{ 0 };
	int dates{ 0 };
	int sprites{ 0 };
};
