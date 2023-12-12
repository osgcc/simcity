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
#include "Rectangle.h"
#include "Vector.h"


class WindowBase
{
public:
	WindowBase() = default;
	WindowBase(const WindowBase&) = delete;
	const WindowBase& operator=(const WindowBase&) = delete;

	virtual ~WindowBase() = default;

	bool visible() const { return mVisible; }

	void toggleVisible() { mVisible = !mVisible; }
	
	void show() { mVisible = true; }
	void hide() { mVisible = false; }

	void move(const Vector<int>& delta)
    {
        mArea.startPoint(mArea.startPoint() + delta);
		onMoved(delta);
	}

	void position(const Point<int>& point)
	{
        mArea.startPoint(point);
		onPositionChanged(point);
	}
    
    constexpr Point<int> position() const
    {
        return mArea.startPoint();
    }
    
    void size(const Vector<int>& size)
    {
        mArea.size(size);
    }
    
    constexpr Vector<int> size() const
    {
        return mArea.size();
    }
    
    const Rectangle<int>& area() const
    {
        return mArea;
    }

    void anchor()
    {
        mAnchored = true;
    }

    void unanchor()
    {
        mAnchored = false;
    }

    bool anchored() const
    {
        return mAnchored;
    }

	virtual void injectMouseDown(const Point<int>& position) {};
	virtual void injectMouseUp() {};
	virtual void injectMouseMotion(const Vector<int>& delta) {};
	
	virtual void draw() = 0;
	virtual void update() = 0;
    
protected:
    Rectangle<int> mArea;

private:
	virtual void onMoved(const Vector<int>&) {};
	virtual void onPositionChanged(const Point<int>&) {};
    
	bool mVisible{ false };
    bool mAnchored{ false };
};
