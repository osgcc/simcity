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


int Budget::CashFlow() const
{
	return TaxFund() - (PoliceFund() + FireFund() + RoadFund());
}


void Budget::Spend(const int amount)
{
	mCurrentFunds -= amount;
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

}
