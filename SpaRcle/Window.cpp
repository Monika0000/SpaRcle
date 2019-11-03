#include "pch.h"
#include "Window.h"
#include "Settings.h"
#include <Windows.h>

namespace SpaRcle {
	sf::Vector2u Window::DisplaySize;
	sf::Vector2u Window::CurrentSize;
	std::thread Window::G_proc;
	std::thread Window::L_proc;
	sf::Texture Window::defaultTex;
	sf::Font Window::defaultFont;

	sf::Vector2f Window::NormalizePos(sf::Vector2f pos, float modifer) {
		return sf::Vector2f(
			(std::sqrt(CurrentSize.x / 2) + pos.x - 40) * (float(CurrentSize.x) / float(Window::DisplaySize.x)),
			(std::sqrt(CurrentSize.y / 2) + pos.y - 40) * (float(CurrentSize.y) / float(Window::DisplaySize.y)));
	}
	sf::Vector2f Window::NormalizeScale(sf::Vector2f scale, float modifer) {
		return sf::Vector2f(
			 modifer* (float(CurrentSize.x) / float(Window::DisplaySize.x)),
			 modifer* (float(CurrentSize.y) / float(Window::DisplaySize.y)));
	}
	sf::Sprite Window::NormalizeSprite(sf::Sprite sp)
	{
		sp.setPosition(NormalizePos(sp.getPosition()));
		//sp.setScale(NormalizeScale(sp.getScale()));
		sp.scale((float(CurrentSize.x) / float(Window::DisplaySize.x)), (float(CurrentSize.y) / float(Window::DisplaySize.y)));
		return sp;
	}
	sf::Text Window::NormalizeText(sf::Text sp)
	{
		sp.setPosition(NormalizePos(sp.getPosition()));
		sp.setScale(NormalizeScale(sp.getScale()));
		return sp;
	}
	sf::RectangleShape Window::NormalizeRect(sf::RectangleShape sp, float modifer)
	{
		sp.setPosition(NormalizePos(sp.getPosition(), modifer));
		sp.setScale(NormalizeScale(sp.getScale(), modifer));
		return sp; }

	void Window::AddAllElements()
	{
		AddButton("Hello", sf::Vector2f(140, 90), sf::Vector2f(60, 20), "Test button", 3);
		AddButton("Hello1", sf::Vector2f(140, 190), sf::Vector2f(60, 20), "Test button", 3);
	}

	void Window::AddButton(std::string name, sf::Vector2f pos, sf::Vector2f size, std::string context, float scale)
	{
		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(size));

		(*rect).setPosition(pos);
		MouseRects.insert(std::pair<std::string, sf::RectangleShape*>(name, rect));
		
		sf::Text* text = new sf::Text();
		(*text).setString(context);

		Buttons.insert(std::pair<std::string, Button*>(name, new Button(pos, size, context, scale)));

		AddMouseEvent(name);
		AddEvent(name);
	}


	void Window::AddMouseEvent(std::string key)
	{
		Mouses.insert(std::pair<std::string, GMouse>(key, [=](Window& win, sf::RectangleShape& pos, std::string keyAction) {

			if (win.MousePos.x <= (int)win.CurrentSize.x && win.MousePos.y <= (int)win.CurrentSize.y && win.MousePos.x >= (int)pos.getPosition().x
				&& win.MousePos.x <= (int)pos.getPosition().x + pos.getSize().x &&
				win.MousePos.y >= (int)pos.getPosition().y && win.MousePos.y <= pos.getPosition().y + pos.getSize().y) 
				{
				win.Actions[keyAction](&win);
				win.Buttons[keyAction]->IsActive = true;

				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
					win.Buttons[keyAction]->L_clicked = 1;
			}
			else
				win.Buttons[keyAction]->IsActive = false;

			}));
	}

	void Window::AddEvent(std::string key)
	{
		Actions.insert(std::pair<std::string, GAction>(key, [=](Window*win) {
			Debug::Log("IsClicked!");
			}));
	}

	void Window::AddTextEntry(std::string key, sf::Vector2f pos, sf::Vector2f size, float scale)
	{
	}

	void Window::AddLClickEvent(std::string key, std::function<void(Window& win)> event)
	{
	}

	void Window::AddTable(std::string key, sf::Vector2f pos, sf::Vector2f size, float scale)
	{
	}

	Window::Window()
	{
		Debug::Log("Creating window...", DType::Info);

		defaultTex.loadFromFile("D:\\SpaRcle\\SpaRcle\\Resources\\default.jpg");
		defaultFont.loadFromFile(Settings::Resources + "\\arial.ttf"); //calibri.ttf a_AlternaNr.ttf

		IsActive = true;

		int iWidth = GetSystemMetrics(SM_CXSCREEN);  // разрешение экрана по горизонтали
		int iHeight = GetSystemMetrics(SM_CYSCREEN); // разрешение экрана по вертикали
		DisplaySize = sf::Vector2u(iWidth, iHeight); //  - 23
		CurrentSize = sf::Vector2u(854, 480);

		G_proc = std::thread([]() {
			auto& wn = *Window::Get();

			sf::RenderWindow window(sf::VideoMode(800, 600), "SpaRcle : Display");
			window.setVerticalSyncEnabled(true); // V-Syns

			sf::Vector2u a = window.getSize();
			typename GTexts::iterator it_texts;
			typename GRects::iterator it_rects;
			typename GSprites::iterator it_sprites;
			typename std::map<std::string, Button*>::iterator it_buttons;
			sf::RectangleShape rect;
			sf::Sprite sprite;

			while (window.isOpen()) {
				Sleep(100);

				wn.MousePos = sf::Mouse::getPosition(window);

				if (wn.CurrentSize.x != a.x || wn.CurrentSize.y != a.y) {
					a = wn.CurrentSize;
					window.setSize(a);
				}

				sf::Event event;
				while (window.pollEvent(event)) {
					switch (event.type)
					{
					case sf::Event::Closed: // пользователь попытался закрыть окно: мы закрываем окно
						window.close();
					case sf::Event::Resized:
					{
						double w = static_cast<double>(event.size.width);
						double h = static_cast<double>(event.size.height);
						wn.CurrentSize = sf::Vector2u(w, h);
						window.setView(sf::View(
							sf::Vector2f(w / 2.0, h / 2.0),
							sf::Vector2f(w, h)));
						break;
					}
					default:
						break;
					}
				}

				window.clear();


				it_texts = (wn.Texts.begin());
				for (size_t t = 0; t < wn.Texts.size(); t++) {
					window.draw(NormalizeText(*it_texts->second));
					it_texts++;
				}


				it_rects = (wn.Rectangles.begin());
				for (size_t t = 0; t < wn.Rectangles.size(); t++) {
					window.draw(NormalizeRect(*it_rects->second));
					it_rects++;
				}


				it_buttons = (wn.Buttons.begin());
				for (size_t t = 0; t < wn.Buttons.size(); t++) {
					if (it_buttons->second->L_clicked == 0) {
						window.draw(NormalizeRect(*it_buttons->second->border, it_buttons->second->scale));
						if (it_buttons->second->IsActive)
							window.draw(NormalizeRect(*it_buttons->second->active, it_buttons->second->scale));
						else
							window.draw(NormalizeRect(*it_buttons->second->notActive, it_buttons->second->scale));
					}
					else
						window.draw(NormalizeRect(*it_buttons->second->click, it_buttons->second->scale));

					window.draw(NormalizeText(*it_buttons->second->text));
					it_buttons++;
				}

				it_sprites = (wn.Sprites.begin());
				for (size_t t = 0; t < wn.Sprites.size(); t++) {
					sprite = *it_sprites->second;

					sprite.setPosition(NormalizePos(sprite.getPosition()));

					sprite.scale((float(wn.CurrentSize.x) / float(Window::DisplaySize.x)), (float(wn.CurrentSize.y) / float(Window::DisplaySize.y)));
					window.draw(sprite);
					it_sprites++;
				}


				window.display();
			}
			wn.IsActive = false;
		});
		L_proc = std::thread([]() {
			auto& wn = *Window::Get();

			typename MouseActions::iterator it_mouse;
			sf::RectangleShape rect;

			while (wn.IsActive) {
				Sleep(10);

				it_mouse = (wn.Mouses.begin());
				for (size_t t = 0; t < wn.Mouses.size(); t++) {
					rect = NormalizeRect(*wn.MouseRects[it_mouse->first], (*wn.Buttons[it_mouse->first]).scale);
					if ((*wn.Buttons[it_mouse->first]).L_clicked > 0) {
						(*wn.Buttons[it_mouse->first]).L_clicked--;
					}
					rect.setSize(sf::Vector2f(rect.getSize().x* rect.getScale().x, rect.getSize().y* rect.getScale().y));

					it_mouse->second(wn, rect, it_mouse->first);

					it_mouse++;
				}
			}
		});
	}

	Window* Window::Get()
	{
		static Window* wind = new Window();
		return wind;
	}
	Button::Button(sf::RectangleShape* act, sf::RectangleShape* nAct, sf::RectangleShape* bord)
	{
		this->active = act;
		this->border = bord;
		this->notActive = nAct;
		this->IsActive = false;
	}
	Button::Button(sf::Vector2f pos, sf::Vector2f size, std::string text, float scale)
	{
		this->L_clicked = 0;
		this->scale = scale;
		this->active = new sf::RectangleShape();
		this->notActive = new sf::RectangleShape();
		this->border = new sf::RectangleShape();
		this->click = new sf::RectangleShape();

		this->text = new sf::Text();
		this->text->setString(text);
		this->text->setPosition(sf::Vector2f(pos.x + 10, pos.y + pos.x / 8));
		this->text->setFillColor(sf::Color(255, 255, 255));
		this->text->setFont(Window::defaultFont);

		this->click->setFillColor(sf::Color(0, 255, 0));
		this->click->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));
		this->click->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));

		this->active->setFillColor(sf::Color(200, 200, 200));
		this->active->setTexture(&Window::defaultTex);
		this->active->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->active->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->notActive->setFillColor(sf::Color(100, 100, 100));
		this->notActive->setTexture(&Window::defaultTex);
		this->notActive->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->notActive->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->border->setFillColor(sf::Color(50, 50, 50));
		this->border->setSize(size);
		this->border->setPosition(pos.x, pos.y);
	}
}