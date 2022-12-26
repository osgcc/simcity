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
        mArea.x += delta.x;
        mArea.y += delta.y;

		onMoved(delta);
	}

	void position(const Point<int>& point)
	{
        mArea.startPoint(point);
		onPositionChanged(point);
	}

	virtual void injectMouseDown(const Point<int>& position) {};
	virtual void injectMouseUp() {};
	virtual void injectMouseMotion(const Vector<int>& delta) {};
	
	virtual void draw() = 0;
	virtual void update() = 0;

private:
	virtual void onMoved(const Vector<int>&) {};
	virtual void onPositionChanged(const Point<int>&) {};
    
    Rectangle<int>  mArea;

	bool mVisible{ false };
};
