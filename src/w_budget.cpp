/* w_budget.c
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


#include "s_msg.h"

#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"


#include <string>


float roadPercent = 0.0;
float policePercent = 0.0;
float firePercent = 0.0;
int roadValue;
int policeValue;
int fireValue;
int roadMaxValue;
int policeMaxValue;
int fireMaxValue;
int MustDrawCurrPercents = 0;
int MustDrawBudgetWindow = 0;


void drawBudgetWindow()
{
  MustDrawBudgetWindow = 1;
}


void drawCurrPercents()
{
    MustDrawCurrPercents = 1;
}


void InitFundingLevel()
{
  firePercent = 1.0;
  fireValue = 0;
  policePercent = 1.0;
  policeValue = 0;
  roadPercent = 1.0;
  roadValue = 0;
  drawBudgetWindow();
  drawCurrPercents();
}


void ShowBudgetWindowAndStartWaiting()
{
    Eval("UIShowBudgetAndWait");
    Pause();
}


void DoBudgetNow(int fromMenu)
{
  int yumDuckets;
  int total;
  int moreDough;
  int fireInt, policeInt, roadInt;

  fireInt = (int)(((float)FireFund) * firePercent);
  policeInt = (int)(((float)PoliceFund) * policePercent);
  roadInt = (int)(((float)RoadFund) * roadPercent);

  total = fireInt + policeInt + roadInt;

  yumDuckets = TaxFund + TotalFunds();

  if (yumDuckets > total) {
    fireValue = fireInt;
    policeValue = policeInt;
    roadValue = roadInt;
  } else if (total > 0) {
    if (yumDuckets > roadInt) {
      roadValue = roadInt;
      yumDuckets -= roadInt;

      if (yumDuckets > fireInt) {
	fireValue = fireInt;
	yumDuckets -= fireInt;

	if (yumDuckets > policeInt) {
	  policeValue = policeInt;
	  yumDuckets -= policeInt;
	} else {
	  policeValue = yumDuckets;
	  if (yumDuckets > 0)
	    policePercent = ((float)yumDuckets) / ((float)PoliceFund);
	  else
	    policePercent = 0.0;
	}
      } else {
	fireValue = yumDuckets;
	policeValue = 0;
	policePercent = 0.0;
	if (yumDuckets > 0)
	  firePercent = ((float)yumDuckets) / ((float)FireFund);
	else
	  firePercent = 0.0;
      }
    } else {
      roadValue = yumDuckets;
      if (yumDuckets > 0)
	roadPercent = ((float)yumDuckets) / ((float)RoadFund);
      else
	roadPercent = 0.0;

      fireValue = 0;
      policeValue = 0;
      firePercent = 0.0;
      policePercent = 0.0;
    }
  } else {
    fireValue = 0;
    policeValue = 0;
    roadValue = 0;
    firePercent = 1.0;
    policePercent = 1.0;
    roadPercent = 1.0;
  }

  fireMaxValue = FireFund;
  policeMaxValue = PoliceFund;
  roadMaxValue = RoadFund;

  drawCurrPercents();

 noMoney:	
  if ((!autoBudget) || fromMenu) {
    if (!autoBudget) {
      /* TODO: append the the current year to the budget string */
    }

    ShowBudgetWindowAndStartWaiting();

    if (!fromMenu) {
      FireSpend = fireValue;
      PoliceSpend = policeValue;
      RoadSpend = roadValue;

      total = FireSpend + PoliceSpend + RoadSpend;
      moreDough = (int)(TaxFund - total);
      Spend(-moreDough);
    }
    drawBudgetWindow();
    drawCurrPercents();
    sim_update_editors();

  } else { /* autoBudget & !fromMenu */
    if ((yumDuckets) > total) {
      moreDough = (int)(TaxFund - total);
      Spend(-moreDough);
      FireSpend = FireFund;
      PoliceSpend = PoliceFund;
      RoadSpend = RoadFund;
      drawBudgetWindow();
      drawCurrPercents();
      sim_update_editors();
    } else {
      autoBudget = 0; /* XXX: force autobudget */
      MustUpdateOptions = 1;
      ClearMes();
      SendMes(MessageEnumerator::CoffersEmpty);
      goto noMoney;
    }
  }
}


void DoBudget()
{
    DoBudgetNow(0);
}


void DoBudgetFromMenu()
{
  DoBudgetNow(1);
}


void SetBudget(const std::string& flowStr, const std::string& previousStr, const std::string& currentStr, const std::string& collectedStr, int tax)
{
    Eval("UISetBudget {" + flowStr + "} {" + previousStr + "} {" + currentStr + "} {" + collectedStr + "} {" + std::to_string(tax) + "}");
}


void ReallyDrawBudgetWindow()
{
    const int cashFlow = TaxFund - fireValue - policeValue - roadValue;

    const std::string cashFlowString = ((cashFlow < 0) ? "-" : "+") + NumberToDollarDecimal(cashFlow);
    const std::string previousFundsString = NumberToDollarDecimal(TotalFunds());
    const std::string currentFundsString = NumberToDollarDecimal(cashFlow + TotalFunds());
    const std::string taxesCollectedString = NumberToDollarDecimal(TaxFund);

    SetBudget(cashFlowString, previousFundsString, currentFundsString, taxesCollectedString, CityTax);
}


void ReallyDrawCurrPercents()
{
  const std::string msg = "UISetBudgetValues {" +
      std::to_string(static_cast<int>(roadMaxValue * roadPercent)) + "} {" +
      std::to_string(roadMaxValue) + "} {" +
      std::to_string(static_cast<int>(roadPercent * 100)) + "} {" +

      std::to_string(static_cast<int>(policeMaxValue * policePercent)) + "} {" +
      std::to_string(policeMaxValue) + "} {" +
      std::to_string(static_cast<int>(policePercent * 100)) + "} {" +

      std::to_string(static_cast<int>(fireMaxValue * firePercent)) + "} {" +
      std::to_string(fireMaxValue) + "} {" +
      std::to_string(static_cast<int>(firePercent * 100)) + "}";

  Eval(msg);
}


void UpdateBudgetWindow()
{
    if (MustDrawCurrPercents)
    {
        ReallyDrawCurrPercents();
        MustDrawCurrPercents = 0;
    }
    if (MustDrawBudgetWindow)
    {
        ReallyDrawBudgetWindow();
        MustDrawBudgetWindow = 0;
    }
}


void UpdateBudget()
{
    drawCurrPercents();
    drawBudgetWindow();
    Eval("UIUpdateBudget");
}
