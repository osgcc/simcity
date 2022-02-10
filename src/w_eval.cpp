/* w_eval.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
#include "main.h"


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
		strings.cityclass + "} {" +
		strings.citylevel + "} {" +
		strings.goodyes + "} {" +
		strings.goodno + "} {" +
		strings.title + "}";


	Eval(evalMessage);
}


void doScoreCard()
{
	const Strings strings{
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
		NumberToDollarDecimal(CityAssValue),
		cityClassStr[CityClass],
		cityLevelStr[GameLevel],
		std::to_string(CityYes) + "%",
		std::to_string(CityNo) + "%",
		"City Evaluation  " + std::to_string(CurrentYear())
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
