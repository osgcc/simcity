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

    setToolValues();
}


void ToolPalette::draw()
{
    drawBackground();

    for (size_t i = 0; i < 20; ++i)
    {
        SDL_RenderCopy(mRenderer, texture.texture, &mToolButtonUV[i + (mButtonStates[i] * 20)], &mToolButtons[i].rect);
    }
}


void ToolPalette::position(const Point<int>& position)
{
    mRect = { position.x, position.y, 106, 234 };

    for (int i = 0; i < 20; ++i)
    {
        mToolButtons[i].rect = { (i % 3) * 32 + mRect.x + 5, ((i / 3) * 32) + mRect.y + 5, 32, 32 };
    }
}


void ToolPalette::toolIndex(const int toolIndex)
{
    if (mSelectedIndex != NoSelection)
    {
        setButtonState(mSelectedIndex, NormalState);
        mTool = Tool::None;
    }

    setButtonState(toolIndex, PressedState);
    mSelectedIndex = toolIndex;
    mTool = mToolButtons[mSelectedIndex].tool;
}


int ToolPalette::toolIndex() const
{
    return mSelectedIndex;
}



Tool ToolPalette::tool() const
{
    return mTool;
}


void ToolPalette::injectMouseClickPosition(const Point<int>& mousePosition)
{
    for (int i = 0; i < 20; ++i)
    {
        const SDL_Rect& buttonRect = mToolButtons[i].rect;
        if (mousePosition.x >= buttonRect.x && mousePosition.x <= buttonRect.x + buttonRect.w &&
            mousePosition.y >= buttonRect.y && mousePosition.y <= buttonRect.y + buttonRect.h)
        {
            if (mButtonStates[i] == PressedState || mButtonStates[i] == DisabledState)
            {
                continue;
            }

            toolIndex(i);
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


void ToolPalette::setToolValues()
{
    mToolButtons[0].tool = Tool::Residential;
    mToolButtons[1].tool = Tool::Commercial;
    mToolButtons[2].tool = Tool::Industrial;
    mToolButtons[3].tool = Tool::Fire;
    mToolButtons[4].tool = Tool::Query;
    mToolButtons[5].tool = Tool::Police;
    mToolButtons[6].tool = Tool::Wire;
    mToolButtons[7].tool = Tool::Bulldoze;
    mToolButtons[8].tool = Tool::Rail;
    mToolButtons[9].tool = Tool::Road;
    mToolButtons[10].tool = Tool::None; // Unused
    mToolButtons[11].tool = Tool::None; // Unused
    mToolButtons[12].tool = Tool::Stadium;
    mToolButtons[13].tool = Tool::Park;
    mToolButtons[14].tool = Tool::Seaport;
    mToolButtons[15].tool = Tool::Coal;
    mToolButtons[16].tool = Tool::Nuclear;
    mToolButtons[17].tool = Tool::Airport;
    mToolButtons[18].tool = Tool::Network;
    mToolButtons[19].tool = Tool::None; // Unused
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
