#include "GraphWindow.h"

#include <array>
#include <map>

namespace
{
	const SDL_Rect ButtonUp{ 285, 0, 36, 36 };
	const SDL_Rect ButtonDown{ 326, 0, 36, 36 };
	const SDL_Rect Bg{ 0, 0, 264, 287 };

	enum class ButtonId
	{
		Residential,
		Commercial,
		Industrial,
		Pollution,
		Crime,
		Money
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
		ButtonMeta{ ButtonId::Residential, ButtonLayout.at(ButtonId::Residential), { 0 }, { false } },
		ButtonMeta{ ButtonId::Commercial, ButtonLayout.at(ButtonId::Commercial), { 0 }, { false } },
		ButtonMeta{ ButtonId::Industrial, ButtonLayout.at(ButtonId::Industrial), { 0 }, { false } },
		ButtonMeta{ ButtonId::Pollution, ButtonLayout.at(ButtonId::Pollution), { 0 }, { false } },
		ButtonMeta{ ButtonId::Crime, ButtonLayout.at(ButtonId::Crime), { 0 }, { false } },
		ButtonMeta{ ButtonId::Money, ButtonLayout.at(ButtonId::Money), { 0 }, { false } }
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
};


GraphWindow::GraphWindow(SDL_Renderer* renderer) :
	mRenderer(*renderer),
	mTexture(loadTexture(renderer, "images/graph.png"))
{}


void GraphWindow::position(const SDL_Point& position)
{
	mArea = { mArea.x + position.x, mArea.y + position.y, mArea.w, mArea.h };

	for (auto& button : Buttons)
	{
		button.area = { ButtonLayout.at(button.id).x + mArea.x, ButtonLayout.at(button.id).y + mArea.y, 36, 36 };
		button.iconPosition = { button.area.x + 6, button.area.y + 6, 22, 22 };
	}
}


void GraphWindow::injectMouseDown(const SDL_Point& position)
{
	for (auto& button : Buttons)
	{
		if (SDL_PointInRect(&position, &button.area))
		{
			button.toggled = !button.toggled;
		}
	}
}


void GraphWindow::draw()
{
	SDL_RenderCopy(&mRenderer, mTexture.texture, &Bg, &mArea);

	for (auto& button : Buttons)
	{
		const SDL_Rect& buttonTexture = button.toggled ? ButtonDown : ButtonUp;
		const int offset = button.toggled ? 1 : 0;
		const SDL_Rect iconPosition{ button.iconPosition.x + offset, button.iconPosition.y + offset, button.iconPosition.w, button.iconPosition.h };

		SDL_RenderCopy(&mRenderer, mTexture.texture, &buttonTexture, &button.area);
		SDL_RenderCopy(&mRenderer, mTexture.texture, &IconRects.at(button.id), &iconPosition);
	}
}
