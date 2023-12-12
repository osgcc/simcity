#include "WindowBase.h"

#include <SDL2/SDL.h>


namespace
{
    constexpr int TitleBarHeight = 20;
    constexpr int ClientAreaPadding = 8;
}


bool WindowBase::visible() const
{
    return mVisible;
}


void WindowBase::toggleVisible()
{
    mVisible = !mVisible;
}


void WindowBase::show()
{
    mVisible = true;
}


void WindowBase::hide()
{
    mVisible = false;
}


void WindowBase::move(const Vector<int>& delta)
{
    mArea.startPoint(mArea.startPoint() + delta);
    mTitleBarArea.startPoint(mArea.startPoint() + delta);
    mClientArea.startPoint(mArea.startPoint() + Vector<int>{delta.x + ClientAreaPadding, delta.y + TitleBarHeight});
    onMoved(delta);
}


void WindowBase::position(const Point<int>& point)
{
    mArea.startPoint(point);
    mTitleBarArea.startPoint(point);
    mClientArea.startPoint(point + Vector<int>{ClientAreaPadding, TitleBarHeight});
    onPositionChanged(point);
}


constexpr Point<int> WindowBase::position() const
{
    return mArea.startPoint();
}


void WindowBase::size(const Vector<int>& size)
{
    mArea.size(size);
    mTitleBarArea.size({ size.x, TitleBarHeight });
    mClientArea.size(size - Vector{ ClientAreaPadding * 2, TitleBarHeight + ClientAreaPadding });
}


constexpr Vector<int> WindowBase::size() const
{
    return mArea.size();
}


const Rectangle<int>& WindowBase::area() const
{
    return mArea;
}


const Rectangle<int>& WindowBase::clientArea() const
{
    return mClientArea;
}



void WindowBase::anchor()
{
    mAnchored = true;
}


void WindowBase::unanchor()
{
    mAnchored = false;
}


bool WindowBase::anchored() const
{
    return mAnchored;
}


void WindowBase::injectMouseDown(const Point<int>& position)
{
    const SDL_Point& pt{ position.x, position.y };

    const SDL_Rect titlebar{ mTitleBarArea.x, mTitleBarArea.y, mTitleBarArea.width, mTitleBarArea.height };

    if (SDL_PointInRect(&pt, &titlebar) && !anchored())
    {
        mDragging = true;
        return;
    }

    onMouseDown(position);
}


void WindowBase::injectMouseUp()
{
    mDragging = false;
    onMouseUp();
}


void WindowBase::injectMouseMotion(const Vector<int>& delta)
{
    if (!mDragging) { return; }
    move(delta);

    onMouseMotion(delta);
}
