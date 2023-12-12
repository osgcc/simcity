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

#include "WindowBase.h"

#include "Font.h"
#include "Texture.h"

#include <memory>
#include <SDL2/SDL.h>


class EvaluationWindow : public WindowBase
{
public:
    EvaluationWindow() = delete;
    EvaluationWindow(const EvaluationWindow&) = delete;
    const EvaluationWindow operator=(const EvaluationWindow&) = delete;
    
    EvaluationWindow(SDL_Renderer* renderer);
    
    void draw() override;
    void update() override;
    
    void onMouseDown(const Point<int>& position) override;
    void onMouseUp() override;
    
private:
    Font* mFont{ nullptr };
    Texture mTexture;
    SDL_Renderer* mRenderer{ nullptr };
};
