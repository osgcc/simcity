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

    bool visible() const;

    void toggleVisible();

    void show();
    void hide();

    void move(const Vector<int>& delta);

    void position(const Point<int>& point);
    constexpr Point<int> position() const;

    void size(const Vector<int>& size);

    constexpr Vector<int> size() const;

    const Rectangle<int>& area() const;

    void anchor();

    void unanchor();

    bool anchored() const;

    void injectMouseDown(const Point<int>& position);
    void injectMouseUp();
    void injectMouseMotion(const Vector<int>& delta);

    virtual void draw() = 0;
    virtual void update() = 0;

protected:
    const Rectangle<int>& clientArea() const;

private:
    virtual void onMouseDown(const Point<int>&) {};
    virtual void onMouseUp() {};
    virtual void onMouseMotion(const Vector<int>&) {};

    virtual void onMoved(const Vector<int>&) {};
    virtual void onPositionChanged(const Point<int>&) {};

    Rectangle<int> mArea;
    Rectangle<int> mClientArea;
    Rectangle<int> mTitleBarArea;

    bool mVisible{ false };
    bool mAnchored{ false };
    bool mDragging{ false };
};
