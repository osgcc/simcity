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
	static constexpr auto MaxTaxRate = 20;
	static constexpr auto MinTaxRate = 0;

	static constexpr auto DefaultTaxRate = 7;

	static constexpr auto FundingRateStep = 0.10f;

public:
	Budget() = default;
	~Budget() = default;

	int CurrentFunds() const { return mCurrentFunds; }
	void CurrentFunds(const int funds);
	bool Broke() const;
	bool CanAfford(const int amount) const;

	void Spend(const int amount);
	void Deposit(const int amount);

	int PreviousFunds() const { return mPreviousFunds; }
	void PreviousFunds(const int funds);

	int TaxRate() const { return mTaxRate; }
	void TaxRate(const int rate);

	int AverageTax() const { return mAverageTax; }

	int TaxIncome() const { return mTaxIncome; }
	void TaxIncome(const int value) { mTaxIncome = value; }

	int RoadFundsNeeded() const { return mRoadFundsNeeded; }
	void RoadFundsNeeded(const int value) { mRoadFundsNeeded = value; }

	int PoliceFundsNeeded() const { return mPoliceFundsNeeded; }
	void PoliceFundsNeeded(const int value) { mPoliceFundsNeeded = value; }

	int FireFundsNeeded() const { return mFireFundsNeeded; }
	void FireFundsNeeded(const int value) { mFireFundsNeeded = value; }

	int RoadFundsGranted() const { return mRoadFundsGranted; }
	int PoliceFundsGranted() const { return mPoliceFundsGranted; }
	int FireFundsGranted() const { return mFireFundsGranted; }

	int OperatingExpenses() const;

	float RoadPercent() const { return mRoadPercent; }
	void RoadPercent(const float value) { mRoadPercent = value; }

	float PolicePercent() const { return mPolicePercent; }
	void PolicePercent(const float value) { mPolicePercent = value; }

	float FirePercent() const { return mFirePercent; }
	void FirePercent(const float value) { mFirePercent = value; }

	int CashFlow() const;

	bool NeedsAttention() const { return mBudgetNeedsAttention; }

	void update();

private:
	int mCurrentFunds{};
	int mPreviousFunds{};
	int mTaxIncome{};
	int mTaxRate{ DefaultTaxRate };
	int mAverageTax{};

	float mRoadPercent{ 1.0f };
	float mPolicePercent{ 1.0f };
	float mFirePercent{ 1.0f };

	int mRoadFundsGranted{};
	int mPoliceFundsGranted{};
	int mFireFundsGranted{};

	int mRoadFundsNeeded{};
	int mPoliceFundsNeeded{};
	int mFireFundsNeeded{};

	bool mBudgetNeedsAttention{ false };
};
