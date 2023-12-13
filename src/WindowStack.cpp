#include "WindowStack.h"

#include "WindowBase.h"

#include <algorithm>
#include <stdexcept>


/**
 * Adds a Window to be handled by the WindowStack.
 *
 * \note	Pointer is not owned by WindowStack, it is up to the caller to properly handle memory.
 */
void WindowStack::addWindow(WindowBase* window)
{
	if (find(mWindowList.begin(), mWindowList.end(), window) != mWindowList.end())
	{
		throw std::runtime_error("WindowStack::addWindow(): Attempting to add a Window that's already in this stack.");
	}

	mWindowList.push_back(window);
}


/**
 * Removes a Window from the WindowStack.
 *
 * \note Pointer is not owned by WindowStack, it is up to the caller to properly handle memory.
 */
void WindowStack::removeWindow(WindowBase* window)
{
	mWindowList.remove(window);
}



WindowBase* WindowStack::front()
{
	if (mWindowList.empty())
	{
		return nullptr;
	}

	return mWindowList.front();
}


bool WindowStack::pointInWindow(const Point<int>& point) const
{
	for (auto* window : mWindowList)
	{
		if (window->visible() && window->area().contains(point))
		{
			return true;
		}
	}

	return false;
}


void WindowStack::updateStack(const Point<int>& point)
{
	for (auto* window : mWindowList)
	{
		if (window->visible() && window->area().contains(point))
		{
			bringToFront(window);
			return;
		}
	}
}


void WindowStack::bringToFront(WindowBase* window)
{
	const auto windowPosition = find(mWindowList.begin(), mWindowList.end(), window);
	
	if (windowPosition == mWindowList.end())
	{
		throw std::runtime_error("WindowStack::bringToFront(): Window is not managed by this stack.");
	}

	if (windowPosition == mWindowList.begin())
	{
		return;
	}

	//mWindowList.front()->hasFocus(false);

	mWindowList.remove(window);
	mWindowList.push_front(window);
	//window->hasFocus(true);
}


void WindowStack::hide()
{
	for (auto window : mWindowList)
	{
		if(!window->alwaysVisible())
		{
			window->hide();
		}
	}
}


void WindowStack::draw()
{
	for (auto it = mWindowList.rbegin(); it != mWindowList.rend(); ++it)
	{
		auto window = (*it);
		if(window->visible())
		{
			window->draw();
		}
	}
}


void WindowStack::update()
{
	for (auto it = mWindowList.rbegin(); it != mWindowList.rend(); ++it)
	{
		(*it)->update();
	}
}


void WindowStack::injectMouseUp()
{
	for (auto* window : mWindowList)
	{
		window->injectMouseUp();
	}
}


void WindowStack::injectMouseMotion(const Vector<int>& delta)
{
	for (auto* window : mWindowList)
	{
		if(window->visible())
		{
			window->injectMouseMotion(delta);
		}
	}
}
