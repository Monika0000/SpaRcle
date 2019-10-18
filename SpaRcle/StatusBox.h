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
#include "Debug.h"
#include "Helper.h"

namespace SpaRcle {
	class StatusBox {
	private:
		//общие данные
		sf::RenderWindow *window;
		sf::Vector2f Position;
		sf::Font font;

		sf::Color unknow = sf::Color(0, 121, 171); // blue
		sf::Color green = sf::Color(52, 255, 24);
		sf::Color yellow = sf::Color(255, 240, 24);
		sf::Color red = sf::Color(255, 33, 24);

		std::string text = "";
		double textPos;

		double Size;
	public:
		int status = 0;

		StatusBox(sf::Vector2f pos, double Size, sf::RenderWindow *wind, int defaultStatus = 0) {
			this->Position = pos;
			this->Size = Size;
			this->window = wind;
			this->status = defaultStatus;
		}

		void SetText(sf::Font font, std::string text, double textPos) {
			this->font = font;
			this->text = text;
			this->textPos = textPos;
		}

		void draw() {
			sf::CircleShape border(5 * Size);
			border.setPosition(Position);
			border.setFillColor(sf::Color(60, 60, 60));
			(*window).draw(border);

			sf::Vector2f pos = sf::Vector2f(Position.x + Size, Position.y + Size);

			///////////////////////////////////////////////////////
			sf::CircleShape ButtonActive(4 * Size);
			ButtonActive.setPosition(pos);
			switch (status)
			{
			case 0: ButtonActive.setFillColor(unknow); break;
			case 1: ButtonActive.setFillColor(green); break;
			case 2: ButtonActive.setFillColor(yellow); break;
			case 3: ButtonActive.setFillColor(red); break;
			default:
				//std::string s = Helper::format() << status;
				std::string s = std::to_string(status);
				Debug::Log("StatusBox.draw : Error! Unknown status : " + s);
				Sleep(500);
				break;
			}

			if (text != "") {
				sf::Text t(text, font, Size * 8);
				t.setFillColor(sf::Color(10, 10, 10));
				t.setPosition(Position.x + textPos, Position.y - 2.5f);
				(*window).draw(t);
			}
			///////////////////////////////////////////////////////
			
			(*window).draw(ButtonActive);
		}
	};
}