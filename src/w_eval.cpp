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

#include "CityProperties.h"

#include "Evaluation.h"
#include "w_tk.h"
#include "w_util.h"


#include <array>
#include <string>


namespace
{
	bool EvalChanged{ false };
}


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


struct EvaulationStrings
{
	const std::string changed{};
	const std::string score{};
    
    const std::array<std::string, 4> problemString;
    const std::array<std::string, 4> problemVote;

	const std::string pop{};
	const std::string delta{};
	const std::string assessed_dollars{};

	const std::string cityclass{};
	const std::string citylevel{};

	const std::string goodyes{};
	const std::string goodno{};

	const std::string title{};
};


void SetEvaluation(const EvaulationStrings& strings)
{
	const std::string evalMessage = "UISetEvaluation {" +
		strings.changed + "} {" +
		strings.problemString[0] + "} {" +
        strings.problemString[1] + "} {" +
        strings.problemString[2] + "} {" +
        strings.problemString[3] + "} {" +
        strings.problemVote[0] + "} {" +
		strings.problemVote[1] + "} {" +
		strings.problemVote[2] + "} {" +
		strings.problemVote[3] + "} {" +
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


void doScoreCard(const CityProperties& properties)
{
	const EvaulationStrings strings
	{
		std::to_string(deltaCityScore()),
		std::to_string(cityScore()),
        std::array<std::string, 4>
        {
            problemVotes()[problemOrder()[0]] ? probStr[problemOrder()[0]] : " ",
            problemVotes()[problemOrder()[1]] ? probStr[problemOrder()[1]] : " ",
            problemVotes()[problemOrder()[2]] ? probStr[problemOrder()[2]] : " ",
            problemVotes()[problemOrder()[3]] ? probStr[problemOrder()[3]] : " "
        },
        std::array<std::string, 4>
        {
            problemVotes()[problemOrder()[0]] ? std::to_string(problemVotes()[problemOrder()[0]]) + "%" : " ",
            problemVotes()[problemOrder()[1]] ? std::to_string(problemVotes()[problemOrder()[1]]) + "%" : " ",
            problemVotes()[problemOrder()[2]] ? std::to_string(problemVotes()[problemOrder()[2]]) + "%" : " ",
            problemVotes()[problemOrder()[3]] ? std::to_string(problemVotes()[problemOrder()[3]]) + "%" : " "
        },
		std::to_string(cityPopulation()),
		std::to_string(deltaCityPopulation()),
		NumberToDollarDecimal(cityAssessedValue()),
		cityClassStr[cityClass()],
		cityLevelStr[properties.GameLevel()],
		std::to_string(cityYes()) + "%",
		std::to_string(cityNo()) + "%",
		std::to_string(CurrentYear())
	};

	SetEvaluation(strings);
}


void ChangeEval()
{
	EvalChanged = true;
}


void scoreDoer(const CityProperties& properties)
{
	if (EvalChanged)
	{
		doScoreCard(properties);
		EvalChanged = false;
	}
}
