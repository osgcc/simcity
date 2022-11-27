#pragma once


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
	
	virtual void draw() = 0;
	virtual void update() = 0;

private:

	bool mVisible{ false };
};