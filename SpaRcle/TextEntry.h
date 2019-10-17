#pragma once
#include <iostream>
#include <string>
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
	class TextEntry {
	private:
		//общие данные
		sf::Font font;
		sf::RenderWindow *window;
		sf::Vector2f Position;
		sf::Vector2f Size;
		bool active = false;
		int timer = 0;
	public:
		std::string context;
		TextEntry(sf::Vector2f pos, sf::Vector2f sizes, sf::RenderWindow *wind, sf::Font font) {
			this->Position = pos;
			this->Size = sizes;
			this->window = wind;
			this->font = font;
		}

		std::string draw(){
			sf::RectangleShape border(Size);
			border.setPosition(Position);
			border.setFillColor(sf::Color(100, 100, 100));
			(*window).draw(border);
			

			sf::Vector2f pos = sf::Vector2f(Position.x + 2, Position.y + 2);
			sf::Vector2f siz = sf::Vector2f(Size.x - 4, Size.y - 4);

			///////////////////////////////////////////////////////
			sf::RectangleShape ButtonActive(siz);
			ButtonActive.setPosition(pos);
			ButtonActive.setFillColor(sf::Color(155, 155, 155));

			sf::RectangleShape ButtonDeactive(siz);
			ButtonDeactive.setPosition(pos);
			ButtonDeactive.setFillColor(sf::Color(75, 75, 75));
			///////////////////////////////////////////////////////

			if (timer > 0)
				timer--;

			if(timer == 0)
				if (active) {
					std::string key = GetKeyCode::Down();
					if (key != "") {
						if (key == "BackSpace")
						{
							if (context.size() > 0)
								context.erase(context.begin() + context.size() - 1);
						}
						else if (key == "Enter")
						{
							auto s = context;
							this->context = "";
							return s;
						}
						else
							context += key;
						//Sleep(100);
					}
					timer = 4;
				}

			std::string s;

			if (context != "")
				s = context;
			else
				s = "|";

			sf::Text text(s, font, 15);
			text.setFillColor(sf::Color(2,2,2));
			text.setPosition(Position.x + 1.3f, Position.y + 1.5f);

			#pragma region Finaly
			sf::Vector2i MouseCoords = sf::Mouse::getPosition(*window);
			sf::Vector2u WinSize = (*window).getSize();

			bool IsButtonFocused;

			if (MouseCoords.x <= WinSize.x && MouseCoords.y <= WinSize.y && MouseCoords.x >= Position.x && MouseCoords.x <= Position.x + Size.x && MouseCoords.y >= Position.y && MouseCoords.y <= Position.y + Size.y) IsButtonFocused = true; else IsButtonFocused = false;

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

			(*window).draw(text);

			return "";
			#pragma endregion
		}
	};
}