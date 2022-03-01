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
