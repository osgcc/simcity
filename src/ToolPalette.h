#pragma once
#include <SDL2/SDL.h>

#include "Texture.h"


class ToolPalette
{
public:
    ToolPalette() = delete;
    ToolPalette(const ToolPalette&) = delete;
    const ToolPalette& operator=(const ToolPalette&) = delete;

public:
    ToolPalette(SDL_Renderer* renderer) :
        mRenderer(renderer),
        texture(loadTexture(mRenderer, "icons/buttons.png"))
    {
        initToolbarUv();
    }


    void draw()
    {
        for (size_t i = 0; i < 20; ++i)
        {
            SDL_RenderCopy(mRenderer, texture.texture, &mToolButtonUV[i], &mToolButtonPosition[i]);
        }
    }


    void position(const Point<int>& position)
    {
        for (int i = 0; i < 20; ++i)
        {
            mToolButtonPosition[i] = { (i % 3) * 33 + position.x, ((i / 3) * 33) + position.y, 32, 32 };
        }
    }


    void injectMouse(const Point<int>& mousePosition)
    {

    }

private:

    void initToolbarUv()
    {
        for (int i = 0; i < 80; ++i)
        {
            mToolButtonUV[i] = { (i / 20) * 32, (i % 20) * 32, 32, 32 };
        }
    }

    std::array<SDL_Rect, 80> mToolButtonUV{};
    std::array<SDL_Rect, 20> mToolButtonPosition{};

    SDL_Renderer* mRenderer{ nullptr };
    Texture texture{};
};