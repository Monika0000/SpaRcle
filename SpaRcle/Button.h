#pragma once
#include "pch.h"
#include <iostream>
#include "GL/glut.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

namespace SpaRcle {
	class Button {
	private:
		size_t time = 0;
		size_t range = 50;
		//Text
		sf::Font font;
		std::string button_Context;
		int textScale;
		sf::Color TextColor;
		//общие данные
		sf::RenderWindow *window;
		sf::Vector2f Position;
		sf::Vector2f normal;
		sf::Vector2f Size;
		sf::Mouse::Button MouseClickButton;
		//кнопка
		sf::Color ButtonColor;
		sf::Color BorderColor;
		int BorderScale;
		sf::Image ButtonImage;
		sf::Vector2f ButtonTextureStartPoint;
		//спрайт кнопки при наведении курсора
		sf::Color ButtonActiveColor;
		sf::Image ButtonActiveImage;
		sf::Vector2f ButtonActiveTextureStartPoint;
		//переключатели состояния отрисовки
		bool ButtonActive = true;
		bool ButtonHaveImage = false;
		bool ButtonHaveActiveImage = false;
		bool ButtonHaveColor = false;
		bool ButtonActiveHaveColor = false;
		bool BorderHave = false;
	public:
		Button(sf::Vector2f pos, sf::Vector2f sizes, sf::RenderWindow *wind, std::string context, sf::Font font, size_t range, int textScale = 10, sf::Color textColor = sf::Color::White) {
			this->Position = pos;
			this->Size = sizes;
			this->window = wind;
			this->button_Context = context;
			this->font = font;
			this->range = range;
			this->textScale = textScale;
			this->TextColor = textColor;
			MouseClickButton = sf::Mouse::Button::Left;
		}

		void setButtonImage(std::string name, sf::Vector2f position) {
			ButtonTextureStartPoint = position;
			ButtonImage.loadFromFile(name);
			ButtonHaveImage = true;
		}

		void setButtonColor(sf::Color cl) {
			ButtonColor = cl;
			ButtonHaveColor = true;
		}

		void setBorder(sf::Color col, int scale) {
			BorderColor = col;
			BorderHave = true;
			BorderScale = scale;
		}

		void setButtonActiveImage(std::string name, sf::Vector2f position) {
			ButtonActiveTextureStartPoint = position;
			ButtonActiveImage.loadFromFile(name);
			ButtonHaveActiveImage = true;
		}

		void setButtonActiveColor(sf::Color cl) {
			ButtonActiveColor = cl;
			ButtonActiveHaveColor = true;
		}

		void setMouseClickButton(sf::Mouse::Button but) {
			MouseClickButton = but;
		}

		bool draw() {
			#pragma region Render
			if (BorderHave) {
				sf::RectangleShape border(Size);
				border.setPosition(Position);
				border.setFillColor(BorderColor);

				(*window).draw(border);
			}

			sf::Vector2f pos = sf::Vector2f(Position.x + BorderScale, Position.y + BorderScale);

			sf::Texture ButtonPressT;
			sf::Sprite ButtonPressS;
			ButtonPressS.setTexture(ButtonPressT);
			ButtonPressS.setPosition(pos);
			ButtonPressS.setTextureRect(sf::IntRect(ButtonTextureStartPoint.x, ButtonTextureStartPoint.y, Size.x - BorderScale * 2, Size.y - BorderScale * 2));
			ButtonPressS.setColor(sf::Color(84.f, 125.f, 0.f));


			sf::Texture ButtonTexture;
			if (ButtonHaveImage)
				ButtonTexture.loadFromImage(ButtonImage);
			sf::Sprite ButtonSprite;
			ButtonSprite.setTexture(ButtonTexture);
			ButtonSprite.setPosition(pos);
			ButtonSprite.setTextureRect(sf::IntRect(ButtonTextureStartPoint.x, ButtonTextureStartPoint.y, Size.x - BorderScale * 2, Size.y - BorderScale * 2));
			if (ButtonHaveColor) ButtonSprite.setColor(ButtonColor);


			sf::Texture ButtonActiveTexture;
			if (ButtonHaveActiveImage)
				ButtonActiveTexture.loadFromImage(ButtonActiveImage);
			sf::Sprite ButtonActiveSprite;
			ButtonActiveSprite.setTexture(ButtonActiveTexture);
			ButtonActiveSprite.setPosition(pos);
			ButtonActiveSprite.setTextureRect(sf::IntRect(ButtonActiveTextureStartPoint.x, ButtonActiveTextureStartPoint.y, Size.x - BorderScale * 2, Size.y - BorderScale * 2));
			if (ButtonActiveHaveColor) ButtonActiveSprite.setColor(ButtonActiveColor);

			sf::Text text(button_Context, font, textScale);
			text.setFillColor(TextColor);
			text.setPosition(Position.x + 3.3f, Position.y + 5.5f);
			#pragma endregion

			#pragma region Finaly
			sf::Vector2i MouseCoords = sf::Mouse::getPosition(*window);
			sf::Vector2u WinSize = (*window).getSize();

			bool IsButtonFocused;

			if (MouseCoords.x <= (int)WinSize.x && MouseCoords.y <= (int)WinSize.y && MouseCoords.x >= (int)Position.x && MouseCoords.x <= (int)Position.x + Size.x && MouseCoords.y >= (int)Position.y && MouseCoords.y <= Position.y + Size.y) IsButtonFocused = true; else IsButtonFocused = false;

			bool Press = sf::Mouse::isButtonPressed(MouseClickButton);

			if (Press & IsButtonFocused)
				(*window).draw(ButtonPressS);
			else
				if (IsButtonFocused) (*window).draw(ButtonActiveSprite); else  (*window).draw(ButtonSprite);

			(*window).draw(text);

			if (IsButtonFocused && Press && time <= 0) {
				time = range;
				return TRUE;
			}
			else {
				if (time > 0)
					time--;
				return FALSE;
			}
			#pragma endregion
		}
	};
}