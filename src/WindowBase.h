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
    
    const Rectangle<int> area() const
    {
        return mArea;
    }

	virtual void injectMouseDown(const Point<int>& position) {};
	virtual void injectMouseUp() {};
	virtual void injectMouseMotion(const Vector<int>& delta) {};
	
	virtual void draw() = 0;
	virtual void update() = 0;
    
protected:
    Rectangle<int>& area()
    {
        return mArea;
    }

private:
	virtual void onMoved(const Vector<int>&) {};
	virtual void onPositionChanged(const Point<int>&) {};
    
    Rectangle<int>  mArea;

	bool mVisible{ false };
};
