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
#pragma once

class Budget
{
public:
	const int MaxTaxRate = 20;
	const int MinTaxRate = 0;

	const int DefaultTaxRate = 7;

public:
	Budget() = default;
	~Budget() = default;

	int CurrentFunds() const { return mCurrentFunds; }
	void CurrentFunds(const int funds);

	int PreviousFunds() const { return mPreviousFunds; }
	void PreviousFunds(const int funds);

	int TaxRate() const { return mTaxRate; }
	void TaxRate(const int rate);

	int AverageTax() const { return mAverageTax; }

	int TaxFund() const { return mTaxFund; }
	void TaxFund(int fund) { mTaxFund = fund; }

	int RoadFund() const { return mRoadFund; }
	int PoliceFund() const { return mPoliceFund; }
	int FireFund() const { return mFireFund; }

private:
	int mCurrentFunds{};
	int mPreviousFunds{};
	int mTaxFund{};
	int mTaxRate{ DefaultTaxRate };
	int mAverageTax{};

	float mRroadPercent{ 0.0f };
	float mRolicePercent{ 0.0f };
	float mFirePercent{ 0.0f };

	int mRoadValue{};
	int mPoliceValue{};
	int mFireValue{};

	int mRoadMaxValue{};
	int mPoliceMaxValue{};
	int mFireMaxValue{};

	int mRoadFund{};
	int mPoliceFund{};
	int mFireFund{};
};
