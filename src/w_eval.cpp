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
#include "main.h"

#include "w_stubs.h"
#include "w_tk.h"
#include "w_util.h"


#include <string>


extern int ProblemVotes[PROBNUM];

const std::string cityClassStr[6] =
{
  "VILLAGE",
  "TOWN",
  "CITY",
  "CAPITAL",
  "METROPOLIS",
  "MEGALOPOLIS"
};


const std::string cityLevelStr[3] =
{
  "Easy",
  "Medium",
  "Hard"
};


const std::string probStr[10] =
{
  "CRIME",
  "POLLUTION",
  "HOUSING COSTS",
  "TAXES",
  "TRAFFIC",
  "UNEMPLOYMENT",
  "FIRES"
};


struct Strings
{
	const std::string changed{};
	const std::string score{};

	const std::string ps0{};
	const std::string ps1{};
	const std::string ps2{};
	const std::string ps3{};

	const std::string pv0{};
	const std::string pv1{};
	const std::string pv2{};
	const std::string pv3{};

	const std::string pop{};
	const std::string delta{};
	const std::string assessed_dollars{};

	const std::string cityclass{};
	const std::string citylevel{};

	const std::string goodyes{};
	const std::string goodno{};

	const std::string title{};
};


void SetEvaluation(const Strings& strings)
{
	const std::string evalMessage = "UISetEvaluation {" +
		strings.changed + "} {" +
		strings.score + "} {" +
		strings.ps0 + "} {" +
		strings.ps1 + "} {" +
		strings.ps2 + "} {" +
		strings.ps3 + "} {" +
		strings.pv0 + "} {" +
		strings.pv1 + "} {" +
		strings.pv2 + "} {" +
		strings.pv3 + "} {" +
		strings.pop + "} {" +
		strings.delta + "} {" +
		strings.assessed_dollars + "} {" +
		"City Class: " + strings.cityclass + "} {" +
		"City Level: " + strings.citylevel + "} {" +
		"GoodYes: " + strings.goodyes + "} {" +
		"GoodNo: " + strings.goodno + "} {" +
		"Title: " + strings.title + "}";


	Eval(evalMessage);
}


void doScoreCard()
{
	const Strings strings
	{
		std::to_string(deltaCityScore),
		std::to_string(CityScore),
		ProblemVotes[ProblemOrder[0]] ? probStr[ProblemOrder[0]] : " ",
		ProblemVotes[ProblemOrder[1]] ? probStr[ProblemOrder[1]] : " ",
		ProblemVotes[ProblemOrder[2]] ? probStr[ProblemOrder[2]] : " ",
		ProblemVotes[ProblemOrder[3]] ? probStr[ProblemOrder[3]] : " ",
		ProblemVotes[ProblemOrder[0]] ? std::to_string(ProblemVotes[ProblemOrder[0]]) + "%" : " ",
		ProblemVotes[ProblemOrder[1]] ? std::to_string(ProblemVotes[ProblemOrder[1]]) + "%" : " ",
		ProblemVotes[ProblemOrder[2]] ? std::to_string(ProblemVotes[ProblemOrder[2]]) + "%" : " ",
		ProblemVotes[ProblemOrder[3]] ? std::to_string(ProblemVotes[ProblemOrder[3]]) + "%" : " ",
		std::to_string(CityPop),
		std::to_string(deltaCityPop),
		NumberToDollarDecimal(CityAssessedValue),
		cityClassStr[CityClass],
		cityLevelStr[GameLevel()],
		std::to_string(CityYes) + "%",
		std::to_string(CityNo) + "%",
		std::to_string(CurrentYear())
	};

	SetEvaluation(strings);
}


void ChangeEval()
{
	EvalChanged = 1;
}


void scoreDoer()
{
	if (EvalChanged)
	{
		doScoreCard();
		EvalChanged = 0;
	}
}
