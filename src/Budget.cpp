#include "Budget.h"

#include <algorithm>


void Budget::CurrentFunds(const int funds)
{
	mCurrentFunds = funds;
}


void Budget::PreviousFunds(const int funds)
{
	mPreviousFunds = funds;
}


void Budget::TaxRate(const int rate)
{
	mTaxRate = std::clamp(rate, MinTaxRate, MaxTaxRate);
}


int Budget::OperatingExpenses() const
{
    return mFireFundsGranted + mPoliceFundsGranted + mRoadFundsGranted;
}


int Budget::CashFlow() const
{
    return TaxIncome() - OperatingExpenses();
}


void Budget::Spend(const int amount)
{
	mCurrentFunds -= amount;
}


void Budget::Deposit(const int amount)
{
    mCurrentFunds += amount;
}


bool Budget::Broke() const
{
	return mCurrentFunds <= 0;
}


bool Budget::CanAfford(const int amount) const
{
	return mCurrentFunds >= amount;
}


void Budget::update()
{
    PreviousFunds(CurrentFunds());
    mBudgetNeedsAttention = false;

    int fireInt = static_cast<int>(FireFundsNeeded() * mFirePercent);
    int policeInt = static_cast<int>(PoliceFundsNeeded() * mPolicePercent);
    int roadInt = static_cast<int>(RoadFundsNeeded() * mRoadPercent);

    int total = fireInt + policeInt + roadInt;

    int yumDuckets = TaxIncome() + CurrentFunds();
    if (yumDuckets > total)
    {
        mFireFundsGranted = fireInt;
        mPoliceFundsGranted = policeInt;
        mRoadFundsGranted = roadInt;
    }
    else if (total > 0)
    {
        if (yumDuckets > roadInt)
        {
            mRoadFundsGranted = roadInt;
            yumDuckets -= roadInt;

            if (yumDuckets > fireInt)
            {
                mFireFundsGranted = fireInt;
                yumDuckets -= fireInt;

                if (yumDuckets > policeInt)
                {
                    mPoliceFundsGranted = policeInt;
                    yumDuckets -= policeInt;
                }
                else
                {
                    mPoliceFundsGranted = yumDuckets;
                    if (yumDuckets > 0)
                    {
                        mPolicePercent = ((float)yumDuckets) / ((float)PoliceFundsNeeded());
                    }
                    else
                    {
                        mPolicePercent = 0.0f;
                    }
                }
            }
            else
            {
                mFireFundsGranted = yumDuckets;
                mPoliceFundsGranted = 0;
                mPolicePercent = 0.0f;
                if (yumDuckets > 0)
                {
                    mFirePercent = ((float)yumDuckets) / ((float)FireFundsNeeded());
                }
                else
                {
                    mFirePercent = 0.0f;
                }
            }
        }
        else
        {
            mRoadFundsGranted = yumDuckets;
            if (yumDuckets > 0)
            {
                mRoadPercent = ((float)yumDuckets) / ((float)RoadFundsNeeded());
            }
            else
            {
                mRoadPercent = 0.0f;
            }

            mFirePercent = 0.0f;
            mFireFundsGranted = 0;

            mPolicePercent = 0.0f;
            mPoliceFundsGranted = 0;
        }
    }
    else
    {
        mFireFundsGranted = 0;
        mPoliceFundsGranted = 0;
        mRoadFundsGranted = 0;
        mFirePercent = 1.0;
        mPolicePercent = 1.0;
        mRoadPercent = 1.0;
    }

    Deposit(CashFlow());

    if (yumDuckets <= total)
    {
        mBudgetNeedsAttention = true;
    }
}
