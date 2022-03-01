// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright � 2022 Leeor Dicker
//
// Portions Copyright � 1989-2007 Electronic Arts Inc.
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


float roadPercent = 0.0;
float policePercent = 0.0;
float firePercent = 0.0;

int roadValue;
int policeValue;
int fireValue;

int roadMaxValue;
int policeMaxValue;
int fireMaxValue;

int TaxFund;

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
    int fireInt = static_cast<int>(FireFund * firePercent);
    int policeInt = static_cast<int>(PoliceFund * policePercent);
    int roadInt = static_cast<int>(RoadFund * roadPercent);

    int total = fireInt + policeInt + roadInt;

    int yumDuckets = TaxFund + TotalFunds();
    int moreDough{};

    if (yumDuckets > total)
    {
        fireValue = fireInt;
        policeValue = policeInt;
        roadValue = roadInt;
    }
    else if (total > 0)
    {
        if (yumDuckets > roadInt)
        {
            roadValue = roadInt;
            yumDuckets -= roadInt;

            if (yumDuckets > fireInt)
            {
                fireValue = fireInt;
                yumDuckets -= fireInt;

                if (yumDuckets > policeInt)
                {
                    policeValue = policeInt;
                    yumDuckets -= policeInt;
                }
                else
                {
                    policeValue = yumDuckets;
                    if (yumDuckets > 0)
                        policePercent = ((float)yumDuckets) / ((float)PoliceFund);
                    else
                        policePercent = 0.0;
                }
            }
            else
            {
                fireValue = yumDuckets;
                policeValue = 0;
                policePercent = 0.0;
                if (yumDuckets > 0)
                    firePercent = ((float)yumDuckets) / ((float)FireFund);
                else
                    firePercent = 0.0;
            }
        }
        else
        {
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
    }
    else
    {
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
    if ((!autoBudget) || fromMenu)
    {
        if (!autoBudget)
        {
            /* TODO: append the the current year to the budget string */
        }

        ShowBudgetWindowAndStartWaiting();

        if (!fromMenu)
        {
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

    }
    else /* autoBudget & !fromMenu */
    {
        if ((yumDuckets) > total) {
            moreDough = (int)(TaxFund - total);
            Spend(-moreDough);
            FireSpend = FireFund;
            PoliceSpend = PoliceFund;
            RoadSpend = RoadFund;
            drawBudgetWindow();
            drawCurrPercents();
            sim_update_editors();
        }
        else
        {
            autoBudget = 0; /* XXX: force autobudget */
            MustUpdateOptions = 1;
            ClearMes();
            SendMes(NotificationId::CoffersEmpty);
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


void ReallyDrawBudgetWindow(const Budget& budget)
{
    const int cashFlow = TaxFund - fireValue - policeValue - roadValue;

    const std::string cashFlowString = ((cashFlow < 0) ? "-" : "+") + NumberToDollarDecimal(cashFlow);
    const std::string previousFundsString = NumberToDollarDecimal(TotalFunds());
    const std::string currentFundsString = NumberToDollarDecimal(cashFlow + TotalFunds());
    const std::string taxesCollectedString = NumberToDollarDecimal(TaxFund);

    SetBudget(cashFlowString, previousFundsString, currentFundsString, taxesCollectedString, budget.TaxRate());
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


void UpdateBudget()
{
    drawCurrPercents();
    drawBudgetWindow();
    Eval("UIUpdateBudget");
}
