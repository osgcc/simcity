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
#include "StringRender.h"
#include "Texture.h"

#include <memory>
#include <SDL2/SDL.h>


#include "Evaluation.h"


class EvaluationWindow : public WindowBase
{
public:
    EvaluationWindow() = delete;
    EvaluationWindow(const EvaluationWindow&) = delete;
    const EvaluationWindow operator=(const EvaluationWindow&) = delete;
    
    EvaluationWindow(SDL_Renderer* renderer);

    void setEvaluation(const Evaluation& evaluation);
    
    void draw() override;
    void update() override;

private:
    void drawYesNoPanel();
    void drawOpinionPanel();
    void drawStatsPanel();
    void drawScorePanel();
    
private:
    Font* mFont{ nullptr };
    Font* mFontBold{ nullptr };
    Font* mFontSemiBold{ nullptr };

    const int mLineSpacing{ 0 };
    const int mTitleSpacing{ 0 };

    Texture mTexture;
    Texture mTextTexture;

    SDL_Renderer* mRenderer{ nullptr };

    Evaluation mEvaluation;
    StringRender mStringRenderer;
};
