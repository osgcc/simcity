#pragma once

#include "Point.h"
#include "Vector.h"

#include <vector>

class EffectMap
{
public:
    EffectMap() = delete;
    EffectMap(const EffectMap&) = delete;
    EffectMap(EffectMap&&) noexcept = delete;
    EffectMap& operator=(const EffectMap&) = delete;
    EffectMap& operator=(EffectMap&&) noexcept = delete;

    EffectMap(const Vector<int>& size) :
        mDimensions{ size },
        mSize{ size.x * size.y }
    {
        fill(0);
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

private:
    void fill(const int value)
    {
        for (int i{ 0 }; i < mSize; ++i)
        {
            mEffectMap.push_back(value);
        }
    }

    std::vector<int> mEffectMap{};

    const Vector<int> mDimensions;
    const int mSize{};
};
