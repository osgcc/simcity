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
        fill();
    }

    int value(const Point<int>& point) const
    {
        return mEffectMap[point.x * point.y];
    }

    void value(const Point<int>& point, int value)
    {
        mEffectMap[point.x * point.y] = value;
    }

    void reset()
    {
        fill();
    }

    const Vector<int> dimensions() const
    {
        return mDimensions;
    }

private:
    void fill()
    {
        for (int i{ 0 }; i < mSize; ++i)
        {
            mEffectMap.push_back(0);
        }
    }

    std::vector<int> mEffectMap{};

    const Vector<int> mDimensions;
    const int mSize{};
};
