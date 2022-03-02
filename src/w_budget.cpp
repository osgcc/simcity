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

#include "Budget.h"

#include "s_msg.h"

#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"


#include <string>


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
  drawBudgetWindow();
  drawCurrPercents();
}


void ShowBudgetWindowAndStartWaiting()
{
    Eval("UIShowBudgetAndWait");
    Pause();
}


void SetBudget(const std::string& flowStr, const std::string& previousStr, const std::string& currentStr, const std::string& collectedStr, int tax)
{
    Eval("UISetBudget {" + flowStr + "} {" + previousStr + "} {" + currentStr + "} {" + collectedStr + "} {" + std::to_string(tax) + "}");
}


void ReallyDrawBudgetWindow(const Budget& budget)
{
    const int cashFlow = budget.CashFlow();

    const std::string cashFlowString = ((cashFlow < 0) ? "-" : "+") + NumberToDollarDecimal(cashFlow);
    const std::string previousFundsString = NumberToDollarDecimal(budget.CurrentFunds());
    const std::string currentFundsString = NumberToDollarDecimal(cashFlow + budget.CurrentFunds());
    const std::string taxesCollectedString = NumberToDollarDecimal(budget.TaxIncome());

    SetBudget(cashFlowString, previousFundsString, currentFundsString, taxesCollectedString, budget.TaxRate());
}


void ReallyDrawCurrPercents()
{
    /*
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
  */
}


void UpdateBudgetWindow(const Budget& budget)
{
    if (MustDrawCurrPercents)
    {
        ReallyDrawCurrPercents();
        MustDrawCurrPercents = 0;
    }
    if (MustDrawBudgetWindow)
    {
        ReallyDrawBudgetWindow(budget);
        MustDrawBudgetWindow = 0;
    }
}
