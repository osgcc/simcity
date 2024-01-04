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


void EvaluationWindow::setEvaluation(const Evaluation& evaluation)
{
    mEvaluation = evaluation;

    flushTexture(mRenderer, mTextTexture);
    
    const auto titlePadding = mFontBold->height() + 15;
    const auto lineSpacing = mFont->height() + 2;

    // YesNo Panel
    Point<int> yesNoPanelStart =
    {
        YesNoRect.x + ContentPanelPadding.x,
        YesNoRect.y + ContentPanelPadding.y
    };

    mStringRenderer.drawString(*mFontBold, "Is the Mayor doing a good job?", yesNoPanelStart);

    const TextColumnMeta yesno
    {
        mStringRenderer,
        *mFont,
        yesNoPanelStart + Vector<int>{ 0, titlePadding },
        lineSpacing,
        {
            "Yes:  " + mEvaluation.goodyes,
            "No:  " + mEvaluation.goodno
        }
    };

    drawTextColumn(yesno);

    // Opinion Panel
    const Point<int> opinionPanelStart =
    {
        OpinionRect.x + ContentPanelPadding.x,
        OpinionRect.y + ContentPanelPadding.y
    };

    mStringRenderer.drawString(*mFontBold, "What are the biggest issues?", opinionPanelStart);


    const TextColumnMeta opinions
    {
        mStringRenderer,
        *mFont,
        opinionPanelStart + Vector<int>{ 0, titlePadding },
        lineSpacing,
        {
            mEvaluation.problemVote[0] + "  " + mEvaluation.problemString[0],
            mEvaluation.problemVote[1] + "  " + mEvaluation.problemString[1],
            mEvaluation.problemVote[2] + "  " + mEvaluation.problemString[2],
            mEvaluation.problemVote[3] + "  " + mEvaluation.problemString[3]
        }
    };

    drawTextColumn(opinions);


    // Statistics Panel
    const Point<int> statsPanelStart =
    {
        StatisticsRect.x + ContentPanelPadding.x,
        StatisticsRect.y + ContentPanelPadding.y
    };

    const TextColumnMeta statLabels
    {
        mStringRenderer,
        *mFontSemiBold,
        statsPanelStart,
        lineSpacing,
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
    const auto statsColumn2x = statLabels.font.width(*it);

    const TextColumnMeta statValues
    {
        mStringRenderer,
        *mFont,
        {
            statsPanelStart.x + statsColumn2x + 45,
            StatisticsRect.y + ContentPanelPadding.y
        },
        lineSpacing,
        {
            mEvaluation.pop,
            mEvaluation.delta,
            mEvaluation.assessed_dollars,
            mEvaluation.cityclass,
            mEvaluation.citylevel
        }
    };

    drawTextColumn(statValues);

    // City Score
    Point<int> scorePanelStart =
    {
        StatisticsRect.x + ContentPanelPadding.x,
        StatisticsRect.y + ContentPanelPadding.y + lineSpacing * 6
    };

    mStringRenderer.drawString(*mFontBold, "Overall City Score", scorePanelStart);
    mStringRenderer.drawString(*mFont, "Range 0 - 1000", scorePanelStart + Vector<int>{ 0, lineSpacing});

    const TextColumnMeta scoreLabels
    {
        mStringRenderer,
        *mFontSemiBold,
        scorePanelStart + Vector<int>{ 0, lineSpacing * 2 + 10 },
        lineSpacing,
        { "Current Score", "Annual Change" }
    };

    drawTextColumn(scoreLabels);

    const TextColumnMeta scoreValues
    {
        mStringRenderer,
        *mFont,
        scorePanelStart + Vector<int>{ statsColumn2x + 45, lineSpacing * 2 + 10 },
        lineSpacing,
        { mEvaluation.score, mEvaluation.changed }
    };

    drawTextColumn(scoreValues);
    
    SDL_SetRenderTarget(mRenderer, nullptr);
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


void EvaluationWindow::onMouseDown(const Point<int>& position)
{
    
}


void EvaluationWindow::onMouseUp()
{
    
}
