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

#include "Point.h"
#include "Vector.h"

#include <vector>


/**
 * Naive abstraction of a 2D array using a 1D
 * array internally.
 */
class EffectMap
{
public:
    EffectMap() = delete;
    EffectMap(const Vector<int>& size) :
        mDimensions{ size },
        mSize{ size.x * size.y }
    {
        fill(0);
    }

    EffectMap& operator*(int scalar)
    {
        for (size_t i = 0; i < mEffectMap.size(); ++i)
        {
            mEffectMap[i] *= scalar;
        }

        return *this;
    }

    int value(const Point<int>& point) const
    {
        return mEffectMap[(mDimensions.x * point.y) + point.x];
    }

    int& value(const Point<int>& point)
    {
        return mEffectMap[(mDimensions.x * point.y) + point.x];
    }

    void reset()
    {
        mEffectMap.clear();
        fill(0);
    }

    const Vector<int> dimensions() const
    {
        return mDimensions;
    }

    void fill(const int value)
    {
        for (int i{ 0 }; i < mSize; ++i)
        {
            mEffectMap.push_back(value);
        }
    }

private:
    std::vector<int> mEffectMap{};

    Vector<int> mDimensions;
    int mSize{};
};
