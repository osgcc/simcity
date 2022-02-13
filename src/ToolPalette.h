#pragma once
#include <SDL2/SDL.h>

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
    ToolPalette(SDL_Renderer* renderer) :
        mRenderer(renderer),
        texture(loadTexture(mRenderer, "icons/buttons.png"))
    {
        initToolbarUv();

        mButtonStates[10] = DisabledState;
        mButtonStates[11] = DisabledState;
        mButtonStates[18] = DisabledState;
        mButtonStates[19] = DisabledState;
    }

    void draw()
    {
        drawBackground();

        for (size_t i = 0; i < 20; ++i)
        {
            SDL_RenderCopy(mRenderer, texture.texture, &mToolButtonUV[i + (mButtonStates[i] * 20)], &mToolButtonPosition[i]);
        }
    }

    void position(const Point<int>& position)
    {
        mRect = { position.x, position.y, 106, 234 };

        for (int i = 0; i < 20; ++i)
        {
            mToolButtonPosition[i] = { (i % 3) * 32 + mRect.x + 5, ((i / 3) * 32) + mRect.y + 5, 32, 32 };
        }
    }

    void selectedTool(const int toolIndex)
    {
        if (mSelectedIndex != NoSelection)
        {
            setButtonState(mSelectedIndex, NormalState);
        }

        setButtonState(toolIndex, PressedState);
        mSelectedIndex = toolIndex;
    }

    int selectedTool() const
    {
        return mSelectedIndex;
    }

    void injectMouseClickPosition(const Point<int>& mousePosition)
    {
        for (int i = 0; i < 20; ++i)
        {
            const SDL_Rect& buttonRect = mToolButtonPosition[i];
            if (mousePosition.x >= buttonRect.x && mousePosition.x <= buttonRect.x + buttonRect.w &&
                mousePosition.y >= buttonRect.y && mousePosition.y <= buttonRect.y + buttonRect.h)
            {
                if (mButtonStates[i] == PressedState || mButtonStates[i] == DisabledState)
                {
                    continue;
                }

                selectedTool(i);
                return;
            }
        }
    }

    const SDL_Rect& rect() const
    {
        return mRect;
    }

private:

    void initToolbarUv()
    {
        for (int i = 0; i < 80; ++i)
        {
            mToolButtonUV[i] = { (i / 20) * 32, (i % 20) * 32, 32, 32 };
        }
    }

    void setButtonState(int buttonIndex, int buttonState)
    {
        mButtonStates[buttonIndex] = buttonState;
    }

    void drawBackground()
    {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 150);
        SDL_RenderFillRect(mRenderer, &mRect);

        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(mRenderer, &mRect);
    }

    std::array<SDL_Rect, 80> mToolButtonUV{};
    std::array<SDL_Rect, 20> mToolButtonPosition{};
    std::array<int, 20> mButtonStates{};

    SDL_Rect mRect{};

    SDL_Renderer* mRenderer{ nullptr };
    Texture texture{};

    int mSelectedIndex{ NoSelection };
};