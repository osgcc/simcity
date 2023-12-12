#pragma once

#include "Point.h"

#include <list>


class WindowBase;


class WindowStack final
{
public:
	WindowStack() = default;
	~WindowStack() = default;

	void addWindow(WindowBase* window);
	void removeWindow(WindowBase* window);

	WindowBase* front();

	bool pointInWindow(const Point<int>& point) const;

	void updateStack(const Point<int>& point);

	void bringToFront(WindowBase* window);

	void hide();

	void draw();
	void update();

	void injectMouseUp();
	void injectMouseMotion(const Vector<int>&);

private:

	std::list<WindowBase*> mWindowList;
};
