#pragma once

#include "Date.h"
#include "Graph.h"
#include "Sprite.h"


struct Sim
{
	Graph graph{};
	SimDate date{};
	SimSprite sprite{};

	int editors{ 0 };
	int maps{ 0 };
	int graphs{ 0 };
	int dates{ 0 };
	int sprites{ 0 };
};
