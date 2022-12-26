#pragma once

#include "Point.h"


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

	void move(const Point<int>& point)
	{
		onMoved(point);
	}

	void position(const Point<int>& point)
	{
		onPositionChanged(point);
	}

	virtual void injectMouseDown(const Point<int>& position) {};
	virtual void injectMouseUp() {};
	virtual void injectMouseMotion(const Point<int>& delta) {};
	
	virtual void draw() = 0;
	virtual void update() = 0;

private:
	virtual void onMoved(const Point<int>&) {};
	virtual void onPositionChanged(const Point<int>&) {};

	bool mVisible{ false };
};