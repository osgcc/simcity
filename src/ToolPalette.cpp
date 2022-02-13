#include "ToolPalette.h"


ToolPalette::ToolPalette(SDL_Renderer* renderer) :
    mRenderer(renderer),
    texture(loadTexture(mRenderer, "icons/buttons.png"))
{
    initToolbarUv();

    mButtonStates[10] = DisabledState;
    mButtonStates[11] = DisabledState;
    mButtonStates[18] = DisabledState;
    mButtonStates[19] = DisabledState;
}


void ToolPalette::draw()
{
    drawBackground();

    for (size_t i = 0; i < 20; ++i)
    {
        SDL_RenderCopy(mRenderer, texture.texture, &mToolButtonUV[i + (mButtonStates[i] * 20)], &mToolButtonPosition[i]);
    }
}


void ToolPalette::position(const Point<int>& position)
{
    mRect = { position.x, position.y, 106, 234 };

    for (int i = 0; i < 20; ++i)
    {
        mToolButtonPosition[i] = { (i % 3) * 32 + mRect.x + 5, ((i / 3) * 32) + mRect.y + 5, 32, 32 };
    }
}


void ToolPalette::selectedTool(const int toolIndex)
{
    if (mSelectedIndex != NoSelection)
    {
        setButtonState(mSelectedIndex, NormalState);
    }

    setButtonState(toolIndex, PressedState);
    mSelectedIndex = toolIndex;
}


int ToolPalette::selectedTool() const
{
    return mSelectedIndex;
}


void ToolPalette::injectMouseClickPosition(const Point<int>& mousePosition)
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


const SDL_Rect& ToolPalette::rect() const
{
    return mRect;
}


void ToolPalette::initToolbarUv()
{
    for (int i = 0; i < 80; ++i)
    {
        mToolButtonUV[i] = { (i / 20) * 32, (i % 20) * 32, 32, 32 };
    }
}


void ToolPalette::setButtonState(int buttonIndex, int buttonState)
{
    mButtonStates[buttonIndex] = buttonState;
}


void ToolPalette::drawBackground()
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 150);
    SDL_RenderFillRect(mRenderer, &mRect);

    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(mRenderer, &mRect);
}
