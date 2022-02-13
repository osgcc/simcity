#pragma once

#include <array>

#include <SDL2/SDL.h>

#include "Point.h"
#include "Texture.h"


class ToolPalette
{
public:
    ToolPalette() = delete;
    ToolPalette(const ToolPalette&) = delete;
    const ToolPalette& operator=(const ToolPalette&) = delete;

    const int NoSelection = -1;
    
    const int NormalState = 0;
    const int PressedState = 1;
    const int DisabledState = 2;

public:
    ToolPalette(SDL_Renderer* renderer);

    void draw();

    void position(const Point<int>& position);

    void selectedTool(const int toolIndex);

    int selectedTool() const;

    void injectMouseClickPosition(const Point<int>& mousePosition);

    const SDL_Rect& rect() const;

private:
    void initToolbarUv();

    void setButtonState(int buttonIndex, int buttonState);

    void drawBackground();

    std::array<SDL_Rect, 80> mToolButtonUV{};
    std::array<SDL_Rect, 20> mToolButtonPosition{};
    std::array<int, 20> mButtonStates{};

    SDL_Rect mRect{};

    SDL_Renderer* mRenderer{ nullptr };
    Texture texture{};

    int mSelectedIndex{ NoSelection };
};