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
	class GetKeyCode {
	public:
		static std::string Down() {
			bool shift = false;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				shift = true;

			#pragma region 0_Line
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				return " ";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				return "Enter";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
				return "BackSpace";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
				return "0";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
				if (shift) return "!"; else return "1";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
				return "2";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
				return "3";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
				return "4";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
				return "5";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
				return "6";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7))
				if (shift) return "?"; else return "7";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8))
				return "8";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
				return "9";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen))
				return "-";
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
				if(shift) return "+"; else return "=";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Slash))
				if (shift) return "?"; else return ".";
			#pragma endregion

			#pragma region 1_Line
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				if (shift) return "Q"; else return "q";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				if (shift) return "W"; else return "w";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				if (shift) return "E"; else return "e";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				if (shift) return "R"; else return "r";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
				if (shift) return "T"; else return "t";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
				if (shift) return "Y"; else return "y";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
				if (shift) return "U"; else return "u";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
				if (shift) return "I"; else return "i";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
				if (shift) return "O"; else return "o";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
				if (shift) return "P"; else return "p";
			#pragma endregion

			#pragma region 2_Line
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				if (shift) return "A"; else return "a";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				if (shift) return "S"; else return "s";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				if (shift) return "D"; else return "d";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
				if (shift) return "F"; else return "f";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
				if (shift) return "G"; else return "g";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
				if (shift) return "H"; else return "h";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
				if (shift) return "J"; else return "j";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
				if (shift) return "K"; else return "k";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
				if (shift) return "L"; else return "l";
			#pragma endregion

			#pragma region 3_Line
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
				if (shift) return "Z"; else return "z";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
				if (shift) return "X"; else return "x";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
				if (shift) return "C"; else return "c";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
				if (shift) return "V"; else return "v";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
				if (shift) return "B"; else return "b";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
				if (shift) return "N"; else return "n";

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
				if (shift) return "M"; else return "m";
			#pragma endregion

			return "";
		}
	};
}