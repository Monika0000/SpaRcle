#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "GL/glut.h"
#include "GetKeyCode.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

namespace SpaRcle {
	class Selector {
	private:
		//общие данные
		sf::Font font;
		sf::RenderWindow *window;
		sf::Vector2f Position;
		sf::Vector2f Size;
		std::vector<std::string> variants;
		bool active = false;
	public:
		int select = 0;
		Selector(sf::Vector2f pos, sf::RenderWindow *wind, sf::Font font, std::vector<std::string> vars) {
			this->Position = pos;
			this->Size = sf::Vector2f(75, 25);
			this->window = wind;
			this->font = font;
			this->variants = vars;
		}

		int draw() {
			sf::RectangleShape border(Size);
			border.setPosition(Position);
			border.setFillColor(sf::Color(100, 100, 100));
			(*window).draw(border);

			sf::Vector2f sizA = sf::Vector2f(Size.x - 4, Size.y - 4);
			sf::Vector2f pos = sf::Vector2f(Position.x + 2, Position.y + 2);

			///////////////////////////////////////////////////////
			sf::RectangleShape ButtonActive(sizA);
			ButtonActive.setPosition(pos);
			ButtonActive.setFillColor(sf::Color(155, 155, 155));


			sf::RectangleShape ButtonDeactive(sizA);
			ButtonDeactive.setPosition(pos);
			ButtonDeactive.setFillColor(sf::Color(75, 75, 75));
			///////////////////////////////////////////////////////

			#pragma region Finaly
			sf::Vector2i MouseCoords = sf::Mouse::getPosition(*window);
			sf::Vector2u WinSize = (*window).getSize();

			bool IsButtonFocused;

			if (MouseCoords.x <= WinSize.x && MouseCoords.y <= WinSize.y && MouseCoords.x >= Position.x && MouseCoords.x <= Position.x + Size.x && MouseCoords.y >= Position.y && MouseCoords.y <= Position.y + Size.y) IsButtonFocused = true; else IsButtonFocused = false;

			if(active)
				for (int i = 0; i < variants.size(); i++)
					if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
						if (MouseCoords.x <= WinSize.x && MouseCoords.y <= WinSize.y && MouseCoords.x >= Position.x && MouseCoords.x <= Position.x + Size.x && MouseCoords.y >= Position.y - 20 * (i + 1) && MouseCoords.y <= Position.y + Size.y - 20 * (i + 1))
							select = i;

			bool Press = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

			if (Press)
				if (IsButtonFocused)
					active = true;
				else
					active = false;

			if (active)
				(*window).draw(ButtonActive);
			else 
				(*window).draw(ButtonDeactive);

			sf::Text text(variants[select], font, 15);
			text.setFillColor(sf::Color(2, 2, 2));
			text.setPosition(Position.x + 5.0f, Position.y + 1.5f);
			(*window).draw(text);

			if (active) {
				double mult = 0;
				for (std::string s : variants) {
					sf::Text text(s, font, 15);
					text.setFillColor(sf::Color(2, 2, 2));
					text.setPosition(Position.x + 5.0f, Position.y + 1.5f - mult - 20);

					mult += 20;

					(*window).draw(text);
				}
			}

			return select;
			#pragma endregion
		}
	};
}