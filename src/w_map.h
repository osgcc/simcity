#pragma once

enum class MiniMapOverlay
{
	All,
	Residential,
	Commercial,
	Industrial,
	Power,
	Road,
	PopulationDensity,
	RateOfGrowth,
	TrafficDensity,
	Pollution,
	Crime,
	LandValue,
	FireRadius,
	PoliceRadius,
	Dynamic
};


int DoUpdateMap();
