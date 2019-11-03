#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Debug.h"
#include "Helper.h"
#include "CentralCore.h"
#include <vector>
#include <map>
#include <thread>
#include <functional>

namespace SpaRcle {
	struct Button{
		Button(sf::RectangleShape* act, sf::RectangleShape* nAct, sf::RectangleShape* bord);
		Button(sf::Vector2f pos, sf::Vector2f size, std::string text, float scale);
		bool IsActive;
		sf::RectangleShape* active,* notActive, *click;
		sf::RectangleShape* border;
		sf::Text* text;
		float scale;
		int L_clicked;
	};
	class Window;
	

	typedef std::map<std::string, sf::Sprite*> GSprites;
	typedef std::map<std::string, sf::Text*> GTexts;
	typedef std::map<std::string, sf::CircleShape*> GCircles;
	typedef std::map<std::string, sf::RectangleShape*> GRects;
	typedef std::function<void(Window*win)> GAction;
	typedef std::function<void(Window&win, sf::RectangleShape& pos, std::string keyAction)> GMouse;

	typedef std::map<std::string, GMouse> MouseActions;

	class Window
	{
	public:

		static sf::Texture defaultTex;

		static sf::Vector2u DisplaySize;
		static sf::Vector2u CurrentSize;
		
		static sf::Vector2f NormalizePos(sf::Vector2f pos, float modifer = 0);
		static sf::Vector2f NormalizeScale(sf::Vector2f scale, float modifer = 1);
		static sf::Sprite NormalizeSprite(sf::Sprite sp);
		static sf::RectangleShape NormalizeRect(sf::RectangleShape sp, float modifer = 0);
		static sf::Text NormalizeText(sf::Text sp);

		sf::Vector2i MousePos;

		CentralCore* core;
		static sf::Font defaultFont;
		
		void AddAllElements();
		void AddButton(std::string name, sf::Vector2f pos, sf::Vector2f size, std::string context = "", float scale = 1);
		void AddMouseEvent(std::string key);
		void AddEvent(std::string key);
		void AddTextEntry(std::string key, sf::Vector2f pos, sf::Vector2f size, float scale = 1);
		void AddLClickEvent(std::string key, std::function<void(Window&win)> event);
		void AddTable(std::string key, sf::Vector2f pos, sf::Vector2f size, float scale = 1);

		Window();
		static Window* Get();

		bool IsActive;

		GTexts Texts;
		GCircles Circles;
		GRects Rectangles;
		GSprites Sprites;

		std::map<std::string, Button*> Buttons;

		std::map<std::string, GAction> Actions;
		MouseActions Mouses;
		GRects MouseRects;
	private:
		static std::thread G_proc;
		static std::thread L_proc;
	};
}
//•