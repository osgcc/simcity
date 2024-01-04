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
#include "EvaluationWindow.h"


#include "Evaluation.h"


namespace
{
    const SDL_Rect BgRect{ 0, 0, 503, 256 };

    constexpr SDL_Rect YesNoRect{ 12, 50, 234, 70 };
    constexpr SDL_Rect OpinionRect{ 12, 128, 234, 115 };
    constexpr SDL_Rect StatisticsRect{ 256, 50, 234, 193 };

    constexpr Vector<int> ContentPanelPadding{ 2, 2 };

    int DualColumnOffset = 0;

    auto stringLength = [](std::string a, std::string b)
        {
            return a.length() < b.length();
        };


    struct TextColumnMeta
    {
        StringRender& stringRenderer;
        Font& font;
        const Point<int> startPoint;
        int lineSpacing;
        const std::vector<std::string> columns;
    };

    void drawTextColumn(const TextColumnMeta& textColumnMeta)
    {
        int offset = 0;
        for (auto& columnText : textColumnMeta.columns)
        {
            textColumnMeta.stringRenderer.drawString(textColumnMeta.font, columnText, textColumnMeta.startPoint + Vector<int>{ 0, offset });
            offset += textColumnMeta.font.height();
        }
    }

    /**
     * \todo This function is pretty useful, move it into a higher
             level so other areas of the code can use it
     */
    void flushTexture(SDL_Renderer* renderer, Texture& texture)
    {
        SDL_SetRenderTarget(renderer, texture.texture);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_RenderFillRect(renderer, &texture.area);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
};



EvaluationWindow::EvaluationWindow(SDL_Renderer* renderer):
    mFont{ new Font("res/raleway-medium.ttf", 13) },
    mFontBold{ new Font("res/raleway-bold.ttf", 13) },
    mFontSemiBold{ new Font("res/Raleway-BoldItalic.ttf", 13) },
    mLineSpacing{ mFont->height() + 2 },
    mTitleSpacing{ mFontBold->height() + 15 },
    mTexture(loadTexture(renderer, "images/EvalWindow.png")),
    mTextTexture(newTexture(renderer, mTexture.dimensions)),
    mRenderer{ renderer },
    mStringRenderer{ renderer }
{
    size({ BgRect.w, BgRect.h });

    SDL_SetTextureColorMod(mFont->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(mFontBold->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(mFontSemiBold->texture(), 0, 0, 0);

    SDL_SetTextureBlendMode(mTextTexture.texture, SDL_BLENDMODE_BLEND);
}


void EvaluationWindow::drawYesNoPanel()
{
    Point<int> panelStart =
    {
        YesNoRect.x + ContentPanelPadding.x,
        YesNoRect.y + ContentPanelPadding.y
    };

    mStringRenderer.drawString(*mFontBold, "Is the Mayor doing a good job?", panelStart);

    const TextColumnMeta yesNoLabels
    {
        mStringRenderer,
        * mFontSemiBold,
        panelStart + Vector<int>{ 0, mTitleSpacing },
        mLineSpacing,
        { "Yes", "No" }
    };

    drawTextColumn(yesNoLabels);

    const TextColumnMeta yesNoValues
    {
        mStringRenderer,
        *mFont,
        panelStart + Vector<int>{ DualColumnOffset + 50, mTitleSpacing },
        mLineSpacing,
        { mEvaluation.goodyes, mEvaluation.goodno }
    };

    drawTextColumn(yesNoValues);
}


void EvaluationWindow::drawOpinionPanel()
{
    const Point<int> panelStart =
    {
        OpinionRect.x + ContentPanelPadding.x,
        OpinionRect.y + ContentPanelPadding.y
    };

    mStringRenderer.drawString(*mFontBold, "What are the biggest issues?", panelStart);

    const TextColumnMeta opinionLabels
    {
        mStringRenderer,
        *mFontSemiBold,
        panelStart + Vector<int>{ 0, mTitleSpacing },
        mLineSpacing,
        {
            mEvaluation.problemString[0],
            mEvaluation.problemString[1],
            mEvaluation.problemString[2],
            mEvaluation.problemString[3]
        }
    };

    drawTextColumn(opinionLabels);

    const TextColumnMeta opinionValues
    {
        mStringRenderer,
        *mFont,
        panelStart + Vector<int>{ DualColumnOffset + 50, mTitleSpacing },
        mLineSpacing,
        {
            mEvaluation.problemVote[0],
            mEvaluation.problemVote[1],
            mEvaluation.problemVote[2],
            mEvaluation.problemVote[3]
        }
    };

    drawTextColumn(opinionValues);
}


void EvaluationWindow::drawStatsPanel()
{
    const Point<int> panelStart =
    {
        StatisticsRect.x + ContentPanelPadding.x,
        StatisticsRect.y + ContentPanelPadding.y
    };

    const TextColumnMeta statLabels
    {
        mStringRenderer,
        *mFontSemiBold,
        panelStart,
        mLineSpacing,
        {
            "Population",
            "Net Migration",
            "Assessed Value",
            "City Class",
            "Game Level"
        }
    };

    drawTextColumn(statLabels);

    const auto it = std::max_element(statLabels.columns.begin(), statLabels.columns.end(), stringLength);
    DualColumnOffset = statLabels.font.width(*it) + 45;

    const TextColumnMeta statValues
    {
        mStringRenderer,
        *mFont,
        {
            panelStart.x + DualColumnOffset,
            StatisticsRect.y + ContentPanelPadding.y
        },
        mLineSpacing,
        {
            mEvaluation.pop,
            mEvaluation.delta,
            mEvaluation.assessed_dollars,
            mEvaluation.cityclass,
            mEvaluation.citylevel
        }
    };

    drawTextColumn(statValues);
}


void EvaluationWindow::drawScorePanel()
{
    Point<int> scorePanelStart =
    {
        StatisticsRect.x + ContentPanelPadding.x,
        StatisticsRect.y + ContentPanelPadding.y + mLineSpacing * 6
    };

    mStringRenderer.drawString(*mFontBold, "Overall City Score", scorePanelStart);
    mStringRenderer.drawString(*mFont, "Range 0 - 1000", scorePanelStart + Vector<int>{ 0, mLineSpacing });

    const TextColumnMeta scoreLabels
    {
        mStringRenderer,
        *mFontSemiBold,
        scorePanelStart + Vector<int>{ 0, mLineSpacing * 2 + 10 },
        mLineSpacing,
        { "Current Score", "Annual Change" }
    };

    drawTextColumn(scoreLabels);

    const TextColumnMeta scoreValues
    {
        mStringRenderer,
        *mFont,
        scorePanelStart + Vector<int>{ DualColumnOffset, mLineSpacing * 2 + 10 },
        mLineSpacing,
        { mEvaluation.score, mEvaluation.changed }
    };

    drawTextColumn(scoreValues);

    SDL_SetRenderTarget(mRenderer, nullptr);
}


void EvaluationWindow::setEvaluation(const Evaluation& evaluation)
{
    mEvaluation = evaluation;

    flushTexture(mRenderer, mTextTexture);
    
    drawYesNoPanel();
    drawOpinionPanel();
    drawStatsPanel();
    drawScorePanel();
}


void EvaluationWindow::draw()
{
    const SDL_Rect rect{ area().x, area().y, area().width, area().height };
    SDL_RenderCopy(mRenderer, mTexture.texture, &BgRect, &rect);
    SDL_RenderCopy(mRenderer, mTextTexture.texture, &BgRect, &rect);
}


void EvaluationWindow::update()
{
    
}

