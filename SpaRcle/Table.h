#pragma once
#include <iostream>
#include "GL/glut.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

namespace SpaRcle {
	struct Consequence;

	class Table {
	private:
		sf::RenderWindow *window;

		sf::Font font;
		sf::Vector2f pos;

		double dist = 4;

		double size_x, size_y;
	public:
		Table(sf::RenderWindow *wind, sf::Font font, double Pos_x, double Pos_y, double Size_x, double Size_y)
		{
			this->window = wind;
			this->font = font;

			this->size_x = Size_x;
			this->size_y = Size_y;

			pos = sf::Vector2f(Pos_x, Pos_y);
		}

		void draw(std::vector<std::string>& List)
		{
			sf::Vector2f size = sf::Vector2f(10 * size_x, 10 * size_y);

			sf::RectangleShape border(size);
			border.setPosition(pos);
			border.setFillColor(sf::Color(100, 100, 100));
			(*window).draw(border);

			for (size_t i = 0; i < List.size(); i++) {
				sf::RectangleShape box(size - sf::Vector2f(dist * 2, 9.7f * size_y));
				box.setPosition(pos + sf::Vector2f(dist, dist + i * 15));
				box.setFillColor(sf::Color(80, 80, 80));
				(*window).draw(box);

				sf::Text t(List[i], font, 15);
				t.setFillColor(sf::Color(10, 10, 10));
				t.setPosition(pos + sf::Vector2f(dist + 5, dist + i * 15));
				(*window).draw(t);
			}
		}

		void draw(std::vector<Consequence>& List) {
			sf::Vector2f size = sf::Vector2f(10 * size_x, 10 * size_y);

			sf::RectangleShape border(size);
			border.setPosition(pos);
			border.setFillColor(sf::Color(100, 100, 100));
			(*window).draw(border);

			for (size_t i = 0; i < List.size(); i++) {
				sf::RectangleShape box(size - sf::Vector2f(dist * 2, 9.7f * size_y));
				box.setPosition(pos + sf::Vector2f(dist, dist + i * 15));
				box.setFillColor(sf::Color(80, 80, 80));
				(*window).draw(box);

				sf::Text t(List[i].name, font, 15);
				t.setFillColor(sf::Color(10, 10, 10));
				t.setPosition(pos + sf::Vector2f(dist + 5, dist + i * 15));
				(*window).draw(t);
			}
		}
	};
}