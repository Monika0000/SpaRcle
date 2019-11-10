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
	class Window;

	struct Monitor {
	public:
		Monitor(Window & win);

		~Monitor();
		short memory_size;
		sf::Vector2f pos;
		short size;

		void Resize();

		std::vector<std::vector<short>> poses;
		std::vector<sf::VertexArray> lines; 
		std::vector<sf::Color> colors;
	private:
		Window& win;
	};
	struct Button{
		Button(sf::Vector2f pos, sf::Vector2f size, std::string text, float scale, short max_delay = 20);
		Button();
		~Button();
		bool IsActive;
		sf::RectangleShape* active,* notActive, *click;
		sf::RectangleShape* border;
		sf::Text* text;
		float scale;
		short delay;
		short max_delay;
		int L_clicked;
	};
	struct CheckBox : public Button {
		CheckBox(sf::Vector2f pos, sf::Vector2f size, float scale, short max_delay = 20);
		bool IsChecked;
		sf::RectangleShape* _checked;
		sf::RectangleShape* _default;
	};
	struct InfoPanel {
		InfoPanel(sf::Vector2f pos, sf::Vector2f size, float scale, std::vector<sf::Text*> texts, std::vector < std::function<void(Window& win, sf::Text& text)>> funcs);
		sf::RectangleShape* border, * panel;
		std::vector<sf::Text*> texts;
		std::vector < std::function<void(Window& win, sf::Text& text)>> funcs;
		float scale;
	};

	

	typedef std::map<std::string, sf::Sprite*> GSprites;
	typedef std::map<std::string, sf::Text*> GTexts;
	typedef std::map<std::string, sf::CircleShape*> GCircles;
	typedef std::map<std::string, sf::RectangleShape*> GRects;
	typedef std::function<void(Window&win, Button*button)> GAction;
	typedef std::function<void(Window&win, sf::RectangleShape& pos, std::string keyAction)> GMouse;

	typedef std::map<std::string, GMouse> MouseActions;

	class Window
	{
	public:
		static sf::Texture defaultTex;
		static sf::Vector2u DisplaySize;
		static sf::Vector2u CurrentSize;
		static sf::Font defaultFont;

		static sf::Vector2f NormalizePos(sf::Vector2f pos, float modifer = 1);
		static sf::Vector2f NormalizeScale(sf::Vector2f scale, float modifer = 1);
		static sf::Sprite NormalizeSprite(sf::Sprite sp);
		static sf::RectangleShape NormalizeRect(sf::RectangleShape sp, float modifer = 1);
		static sf::Text NormalizeText(sf::Text sp);

		static sf::Text* SetText(sf::Text* text, sf::Font& font, unsigned int size, sf::Color color);
		static sf::Text* SetText(sf::Text* text, std::string str, sf::Font& font, unsigned int size, sf::Color color);

		sf::Vector2i MousePos;
		CentralCore* core;
		bool IsActive;
		bool LMouse;

		void AddAllElements();
		void AddButton(std::string name, sf::Vector2f pos, sf::Vector2f size, std::string context = "", float scale = 1, GAction act = [=](Window&win, Button*button) {}, short max_delay= 20);
		void AddCheckBox(std::string name, sf::Vector2f pos, sf::Vector2f size, std::string context = "", float scale = 1, GAction act = [=](Window&win, Button* button) {}, short max_delay= 20);
		void AddMouseEvent(std::string key);
		void AddEvent(std::string key, GAction act);
		void AddRect(std::string key, sf::Vector2f pos, sf::Vector2f size, sf::Color color = sf::Color(100, 100, 100));
		void AddTextEntry(std::string key, sf::Vector2f pos, sf::Vector2f size, float scale = 1);
		void AddLClickEvent(std::string key, GAction event);
		void AddInfoPanel(std::string key, InfoPanel* panel);
		

		Window();
		~Window();
		static Window* Get();
		void SetCore(CentralCore* core);

		Monitor* monitor;
		GTexts Texts;
		GCircles Circles;
		GRects Rectangles;
		GSprites Sprites;
		std::map<std::string, Button*> Buttons;
		std::map<std::string, InfoPanel*> InfoPanels;
		std::map<std::string, GAction> Actions;
		MouseActions Mouses;
		GRects MouseRects;
	private:
		static std::thread G_proc;
		static std::thread L_proc;
	};
}
//•