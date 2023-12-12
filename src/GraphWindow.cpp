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
#include "GraphWindow.h"

#include "Colors.h"
#include "Graphics.h"

#include <array>
#include <map>

namespace
{
	const SDL_Rect ButtonUp{ 285, 0, 36, 36 };
	const SDL_Rect ButtonDown{ 326, 0, 36, 36 };
	const SDL_Rect Bg{ 0, 0, 264, 287 };

	const SDL_Rect GraphLayout{ 10, 71, 242, 202 };
	SDL_Rect GraphPosition = GraphLayout;

	const SDL_Rect TitleBarLayout{ 2, 2, 261, 19 };
	SDL_Rect TitleBarPosition = TitleBarLayout;

	const SDL_Rect CloseButtonLayout{ 4, 4, 13, 13 };
	SDL_Rect CloseButtonPosition = CloseButtonLayout;


	enum class ButtonId
	{
		Residential,
		Commercial,
		Industrial,
		Pollution,
		Crime,
		Money
	};


	std::map<ButtonId, Graph> HistoryGraphTable
	{
		{ ButtonId::Residential, { ResHis, "Residential", Colors::LightGreen, { 0 } } },
		{ ButtonId::Commercial, { ComHis, "Commercial", Colors::DarkBlue, { 0 } } },
		{ ButtonId::Industrial, { IndHis, "Industrial", Colors::Gold, { 0 } } },
		{ ButtonId::Money, { MoneyHis, "Cash Flow", Colors::Turquoise, { 0 } } },
		{ ButtonId::Crime, { CrimeHis, "Crime", Colors::Red, { 0 } } },
		{ ButtonId::Pollution, { PollutionHis, "Pollution", Colors::Olive, { 0 } } }
	};


	struct ButtonMeta
	{
		ButtonId id{ ButtonId::Residential };
		SDL_Rect area{ 0 };
		SDL_Rect iconPosition{ 0 };
		bool toggled{ false };
	};


	const std::map<ButtonId, SDL_Rect> ButtonLayout
	{
		{ ButtonId::Residential, { 13, 31, 36, 36 } },
		{ ButtonId::Commercial, { 53, 31, 36, 36 } },
		{ ButtonId::Industrial, { 93, 31, 36, 36 } },
		{ ButtonId::Pollution, { 133, 31, 36, 36 } },
		{ ButtonId::Crime, { 173, 31, 36, 36 } },
		{ ButtonId::Money, { 213, 31, 36, 36 } },
	};


	std::array<ButtonMeta, 6> Buttons
	{
		ButtonMeta{ ButtonId::Residential, ButtonLayout.at(ButtonId::Residential), { 0 }, true },
		ButtonMeta{ ButtonId::Commercial, ButtonLayout.at(ButtonId::Commercial), { 0 }, true },
		ButtonMeta{ ButtonId::Industrial, ButtonLayout.at(ButtonId::Industrial), { 0 }, true },
		ButtonMeta{ ButtonId::Pollution, ButtonLayout.at(ButtonId::Pollution), { 0 }, true },
		ButtonMeta{ ButtonId::Crime, ButtonLayout.at(ButtonId::Crime), { 0 }, true },
		ButtonMeta{ ButtonId::Money, ButtonLayout.at(ButtonId::Money), { 0 }, true }
	};


	const std::map<ButtonId, SDL_Rect> IconRects
	{
		{ ButtonId::Residential, { 285, 59, 22, 22 } },
		{ ButtonId::Commercial, { 285, 82, 22, 22 } },
		{ ButtonId::Industrial, { 285, 105, 22, 22 } },
		{ ButtonId::Pollution, { 285, 128, 22, 22 } },
		{ ButtonId::Crime, { 285, 151, 22, 22 } },
		{ ButtonId::Money, { 285, 174, 22, 22 } }
	};


	bool ButtonToggled(const ButtonId id)
	{
		for (auto& button : Buttons)
		{
			if (button.id == id)
			{
				return button.toggled;
			}
		}

		return false;
	}
};


GraphWindow::GraphWindow(SDL_Renderer* renderer) :
	mRenderer(*renderer),
	mTexture(loadTexture(renderer, "images/graph.png"))
{
    size({264, 287});
	initTexture(*MainWindowRenderer, mGraphTexture, { GraphLayout.w, GraphLayout.h });
}


void GraphWindow::onMoved(const Vector<int>& movement)
{
	GraphPosition = { GraphLayout.x + area().x, GraphLayout.y + area().y, GraphLayout.w, GraphLayout.h };
	TitleBarPosition = { TitleBarLayout.x + area().x, TitleBarLayout.y + area().y, TitleBarLayout.w, TitleBarLayout.h };
	CloseButtonPosition = { CloseButtonLayout.x + area().x, CloseButtonLayout.y + area().y, CloseButtonLayout.w, TitleBarLayout.h };

	for (auto& button : Buttons)
	{
		button.area = { ButtonLayout.at(button.id).x + area().x, ButtonLayout.at(button.id).y + area().y, 36, 36 };
		button.iconPosition = { button.area.x + 6, button.area.y + 6, 22, 22 };
	}
}


void GraphWindow::onPositionChanged(const Point<int>& position)
{
	GraphPosition = { GraphLayout.x + area().x, GraphLayout.y + area().y, GraphLayout.w, GraphLayout.h };
	TitleBarPosition = { TitleBarLayout.x + area().x, TitleBarLayout.y + area().y, TitleBarLayout.w, TitleBarLayout.h };
	CloseButtonPosition = { CloseButtonLayout.x + area().x, CloseButtonLayout.y + area().y, CloseButtonLayout.w, TitleBarLayout.h };

	for (auto& button : Buttons)
	{
		button.area = { ButtonLayout.at(button.id).x + area().x, ButtonLayout.at(button.id).y + area().y, 36, 36 };
		button.iconPosition = { button.area.x + 6, button.area.y + 6, 22, 22 };
	}
}


void GraphWindow::onMouseDown(const Point<int>& position)
{ 
	const SDL_Point& pt{ position.x, position.y };

	if (SDL_PointInRect(&pt, &CloseButtonPosition))
	{
		hide();
		return;
	}

	for (auto& button : Buttons)
	{
		if (SDL_PointInRect(&pt, &button.area))
		{
			button.toggled = !button.toggled;
			update();
			return; // assumption: mouse position can only ever be within one button
		}
	}
}


void GraphWindow::draw()
{
    const SDL_Rect rect{ area().x, area().y, area().width, area().height};
	SDL_RenderCopy(&mRenderer, mTexture.texture, &Bg, &rect);

	for (auto& button : Buttons)
	{
		const SDL_Rect& buttonTexture = button.toggled ? ButtonDown : ButtonUp;
		const int offset = button.toggled ? 1 : 0;
		const SDL_Rect iconPosition{ button.iconPosition.x + offset, button.iconPosition.y + offset, button.iconPosition.w, button.iconPosition.h };

		SDL_RenderCopy(&mRenderer, mTexture.texture, &buttonTexture, &button.area);
		SDL_RenderCopy(&mRenderer, mTexture.texture, &IconRects.at(button.id), &iconPosition);
		
		SDL_RenderCopy(&mRenderer, mGraphTexture.texture, &mGraphTexture.area, &GraphPosition);
	}
}


void GraphWindow::update()
{
	for (auto& [type, graph] : HistoryGraphTable)
	{
		fillGraphPoints(graph.points, graph.history);
	}

	SDL_SetRenderTarget(MainWindowRenderer, mGraphTexture.texture);
	
	turnOffBlending(*MainWindowRenderer, mGraphTexture);

	SDL_SetRenderDrawColor(MainWindowRenderer, 0, 0, 0, 0);
	SDL_RenderClear(MainWindowRenderer);

	for (auto& [type, graph] : HistoryGraphTable)
	{
		if (!ButtonToggled(type)) { continue; }
		SDL_SetRenderDrawColor(MainWindowRenderer, graph.color.r, graph.color.g, graph.color.b, 255);
		SDL_RenderDrawLines(MainWindowRenderer, graph.points.data(), static_cast<int>(graph.points.size()));
	}

	turnOnBlending(*MainWindowRenderer, mGraphTexture);
	SDL_SetRenderTarget(MainWindowRenderer, nullptr);
}


void GraphWindow::fillGraphPoints(Graph::PointsList& points, const GraphHistory& history)
{
	float sx = static_cast<float>(GraphLayout.w / 120.0f);
	float sy = static_cast<float>(GraphLayout.h / 256.0f);

	for (int i = 0; i < HistoryLength; ++i)
	{
		const int x = static_cast<int>(i * sx);
		const int y = GraphLayout.h - static_cast<int>(history[i] * sy);

		points[i] = { x, y };
	}
}
