#pragma once
#include <iostream>
#include <string>
#include "GL/glut.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

namespace SpaRcle {
	class CheckBox {
	private:
		//общие данные
		sf::RenderWindow *window;
		//sf::Image ActiveImage;
		//sf::Texture ActiveTexture;
		sf::Vector2f Position;
		double Size;
		int timer = 0;
	public:
		bool active = false;

		CheckBox(sf::Vector2f pos, double Size, sf::RenderWindow *wind, bool defaultState = false) {
			this->Position = pos;
			this->Size = Size;
			this->window = wind;
			this->active = defaultState;
			//ActiveImage.loadFromFile("Resources/check2.png");
			//ActiveTexture.loadFromImage(ActiveImage);
		}

		bool draw() {
			sf::Vector2f v_size = sf::Vector2f(10 * Size, 10 * Size);

			sf::RectangleShape border(v_size);
			border.setPosition(Position);
			border.setFillColor(sf::Color(100, 100, 100));
			(*window).draw(border);


			sf::Vector2f pos = sf::Vector2f(Position.x + 2, Position.y + 2);
			sf::Vector2f siz = sf::Vector2f(v_size.x - 4, v_size.y - 4);

			///////////////////////////////////////////////////////
			
			//sf::Sprite ActiveSprite;
			//ActiveSprite.setTexture(ActiveTexture);
			//ActiveSprite.setPosition(pos);
			//ActiveSprite.setTextureRect(sf::IntRect(15,  15, 20 * 1.01, 20 * 1.01 ));
			//ButtonActiveSprite.setColor(ButtonActiveColor);


			sf::RectangleShape ButtonActive(siz);
			ButtonActive.setPosition(pos);
			ButtonActive.setFillColor(sf::Color(155, 155, 155));

			sf::RectangleShape ButtonDeactive(siz);
			ButtonDeactive.setPosition(pos);
			ButtonDeactive.setFillColor(sf::Color(75, 75, 75));
			///////////////////////////////////////////////////////

			#pragma region Finaly
			sf::Vector2i MouseCoords = sf::Mouse::getPosition(*window);
			sf::Vector2u WinSize = (*window).getSize();

			bool IsButtonFocused;

			if (MouseCoords.x <= WinSize.x && MouseCoords.y <= WinSize.y && MouseCoords.x >= Position.x && MouseCoords.x <= Position.x + v_size.x && MouseCoords.y >= Position.y && MouseCoords.y <= Position.y + v_size.y) IsButtonFocused = true; else IsButtonFocused = false;

			bool Press = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

			if (Press & IsButtonFocused)
			{
				if(timer == 0)
					active = !active;
				timer = 4;
			}

			if(timer > 0)
				timer--;

			if (active) {
				(*window).draw(ButtonActive);
				//(*window).draw(ActiveSprite);
			}
			else
				(*window).draw(ButtonDeactive);
			#pragma endregion

			return active;
		}
	};
}