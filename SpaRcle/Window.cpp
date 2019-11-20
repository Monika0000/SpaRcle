#include "pch.h"
#include "Window.h"
#include "Settings.h"
#include <Windows.h>
#include <random>

namespace SpaRcle {
	sf::Vector2u Window::DisplaySize;
	sf::Vector2u Window::CurrentSize;
	std::thread Window::G_proc;
	std::thread Window::L_proc;
	sf::Texture Window::defaultTex;
	sf::Font Window::defaultFont;

	sf::Vector2f Window::NormalizePos(sf::Vector2f pos, float modifer) {
		return sf::Vector2f(
			(std::sqrt(CurrentSize.x / 2) + pos.x - 40.f) * 
				(double (CurrentSize.x) / double(Window::DisplaySize.x)),
			(std::sqrt(CurrentSize.y / 2) + pos.y - 40.f) * 
				(double(CurrentSize.y) / double(Window::DisplaySize.y))); }
	sf::Vector2f Window::NormalizeScale(sf::Vector2f scale, float modifer) {
		return sf::Vector2f(
			 modifer * (float(CurrentSize.x) / float(Window::DisplaySize.x)),
			 modifer * (float(CurrentSize.y) / float(Window::DisplaySize.y))); }
	sf::Sprite Window::NormalizeSprite(sf::Sprite sp) {
		sp.setPosition(NormalizePos(sp.getPosition()));
		//sp.setScale(NormalizeScale(sp.getScale()));
		sp.scale((float(CurrentSize.x) / float(Window::DisplaySize.x)), (float(CurrentSize.y) / float(Window::DisplaySize.y)));
		return sp; }
	sf::Text Window::NormalizeText(sf::Text sp) {
		sp.setPosition(NormalizePos(sp.getPosition()));
		sp.setScale(NormalizeScale(sp.getScale()));
		return sp; }
	sf::Text* Window::SetText(sf::Text* text, sf::Font& font, unsigned int size, sf::Color color) {
		(*text).setFillColor(color); (*text).setFont(font); (*text).setCharacterSize(size);
		return text; }
	sf::Text* Window::SetText(sf::Text* text, std::string str, sf::Font& font, unsigned int size, sf::Color color) {
		(*text).setFillColor(color); (*text).setFont(font); (*text).setCharacterSize(size); (*text).setString(str);
		return text; }
	sf::RectangleShape Window::NormalizeRect(sf::RectangleShape sp, float modifer) {
		sp.setPosition(NormalizePos(sp.getPosition()));
		sp.setScale(NormalizeScale(sp.getScale(), modifer));
		return sp; }

	void Window::AddAllElements() {
		monitor = new Monitor(*this);

		AddRect("BG", sf::Vector2f(0, 0), sf::Vector2f(10000, 10000), sf::Color(155, 155, 155));

		AddTextEntry("TextEntry", sf::Vector2f(500, 500), sf::Vector2f(150, 20),"enter text...", 3, [=](Window& win, Button* button) {
			TextEntry* box = static_cast<TextEntry*>(&(*button));
			if (box->no_clicked > 0)
			{
				if(box->no_clicked == 1)
					box->IsChecked = false;
				else
					box->no_clicked--;
			}
			else {
				(*box).IsChecked = !(*box).IsChecked;
				if ((*box).IsChecked)
					(*box).notActive = (*box)._checked;
				else
					(*box).notActive = (*box)._default;
				//Debug::Log(static_cast<CheckBox*>(&(*button))->data););
			}
			});

		AddButton("badEvent", sf::Vector2f(25, 610), sf::Vector2f(80, 20), "	[Bad Event]", 3, [=](Window& win, Button* button) {
			win.core->_causality->NewEvent(Consequence(Sound("door", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("is", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("close", 10, 15)));

			win.core->_causality->NewEvent(Consequence(Sound("open", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("door", 10, 15)));

			win.core->_causality->NewEvent(Consequence(Sound("door", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("is", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("open", 10, 15)));

			win.core->_causality->NewEvent(Consequence(Sound("hit", 2, 50)));
			});
		AddButton("plus2", sf::Vector2f(25, 680), sf::Vector2f(80, 20), "		[3 + 4]", 3, [=](Window& win, Button* button) {
			win.core->_causality->NewEvent(Consequence(Sound("bhree", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("plus", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("four", 10, 15)));
			});
		AddButton("plus", sf::Vector2f(25, 750), sf::Vector2f(80, 20), "		[2 + 2]", 3, [=](Window& win, Button* button) {
			win.core->_causality->NewEvent(Consequence(Sound("two", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("plus", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("two", 10, 15)));
			});
		AddButton("Spam", sf::Vector2f(25, 820), sf::Vector2f(80, 20), "    	[Spam]", 3, [=](Window& win, Button* button) {
			win.core->_causality->NewEvent(Consequence(Sound("hello", 10, 15)),false);
			win.core->_causality->NewEvent(Consequence(Sound("monika", 10, 15)), false);
			win.core->_causality->NewEvent(Consequence(Sound("kak", 10, 15)), false);
			win.core->_causality->NewEvent(Consequence(Sound("dele", 10, 15)), false);
			win.core->_causality->NewEvent(Consequence(Sound("baka", 10, 15)), false);
			win.core->_causality->NewEvent(Consequence(Sound("desu", 10, 15)), false);
			}, 0);
		AddButton("Hello", sf::Vector2f(25, 890), sf::Vector2f(80, 20), "  [Hello Monika]", 3, [=](Window& win, Button*button) {
				win.core->_causality->NewEvent(Consequence(Sound("hello", 10, 15)));
				win.core->_causality->NewEvent(Consequence(Sound("monika", 10, 15)));
			});
		AddButton("Hello1", sf::Vector2f(25, 960), sf::Vector2f(80, 20), "  [How are you]", 3, [=](Window& win, Button* button) {
			win.core->_causality->NewEvent(Consequence(Sound("how", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("are", 10, 15)));
			win.core->_causality->NewEvent(Consequence(Sound("you", 10, 15)));
			});
		AddButton("Skip", sf::Vector2f(25, 1030), sf::Vector2f(80, 20), "    	[Skip]", 3, [=](Window& win, Button* button) {
				win.core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
			}, 1);

		AddCheckBox("test", sf::Vector2f(555, 830), sf::Vector2f(20, 20), "2134254354", 3, [=](Window& win, Button* button) {
				CheckBox* box = static_cast<CheckBox*>(&(*button));
				(*box).IsChecked = !(*box).IsChecked;
				if ((*box).IsChecked)
					(*box).notActive = (*box)._checked;
				else
					(*box).notActive = (*box)._default;
				//Debug::Log(static_cast<CheckBox*>(&(*button))->data);
			});
		
		{AddInfoPanel("panel", new InfoPanel(sf::Vector2f(25, 30), sf::Vector2f(300, 500), 3, std::vector<sf::Text*> {
			SetText(new sf::Text(), "		[DEBUG]", defaultFont, 30, sf::Color(255, 155, 255)), // LOGO
				SetText(new sf::Text(), defaultFont, 30, sf::Color(0, 255, 255)), // logs
				SetText(new sf::Text(), defaultFont, 30, sf::Color(255, 255, 0)), // warns
				SetText(new sf::Text(), defaultFont, 30, sf::Color(255, 0, 0)), // errs
				SetText(new sf::Text(), defaultFont, 30, sf::Color(255, 0, 255)), //info 
				SetText(new sf::Text(), defaultFont, 30, sf::Color(0, 255, 0)), // mind
				SetText(new sf::Text(), defaultFont, 30, sf::Color(255, 255, 125)), // module
				SetText(new sf::Text(), defaultFont, 30, sf::Color(0, 0, 255)), // sys
		},
			std::vector<std::function<void(Window & win, sf::Text & text)>> {
				[=](Window& win, sf::Text& text) {},
					[=](Window& win, sf::Text& text) { text.setString("[Logs] : " + std::to_string(Debug::Logs)); },
					[=](Window& win, sf::Text& text) { text.setString("[Warnings] : " + std::to_string(Debug::Warnings)); },
					[=](Window& win, sf::Text& text) { text.setString("[Errors] : " + std::to_string(Debug::Errors)); },
					[=](Window& win, sf::Text& text) { text.setString("[Info] : " + std::to_string(Debug::Info)); },
					[=](Window& win, sf::Text& text) { text.setString("[Mind] : " + std::to_string(Debug::Mind)); },
					[=](Window& win, sf::Text& text) { text.setString("[Module] : " + std::to_string(Debug::Module)); },
					[=](Window& win, sf::Text& text) { text.setString("[System] : " + std::to_string(Debug::System)); },
			})); }
	}

	void Window::AddButton(std::string name, sf::Vector2f pos, sf::Vector2f size, std::string context, float scale, GAction act, short max_delay) {
		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(size));

		(*rect).setPosition(pos);
		MouseRects.insert(std::pair<std::string, sf::RectangleShape*>(name, rect));
		
		Buttons.insert(std::pair<std::string, Button*>(name, new Button(pos, size, context, scale, max_delay)));

		AddMouseEvent(name);
		AddEvent(name, act); }
	void Window::AddTextEntry(std::string name, sf::Vector2f pos, sf::Vector2f size, std::string context, float scale, GAction act, short max_delay) {
		sf::RectangleShape* rect = new sf::RectangleShape(size);

		(*rect).setPosition(pos);
		MouseRects.insert(std::pair<std::string, sf::RectangleShape*>(name, rect));

		Buttons.insert(std::pair<std::string, TextEntry*>(name, new TextEntry(pos, size, scale, context)));

		AddMouseEvent(name);
		AddEvent(name, act); }
	void Window::AddCheckBox(std::string name, sf::Vector2f pos, sf::Vector2f size, std::string context, float scale, GAction act, short max_delay) {
		sf::RectangleShape* rect = new sf::RectangleShape(size);

		(*rect).setPosition(pos);
		MouseRects.insert(std::pair<std::string, sf::RectangleShape*>(name, rect));

		Buttons.insert(std::pair<std::string, CheckBox*>(name, new CheckBox(pos, size, scale, max_delay)));

		AddMouseEvent(name);
		AddEvent(name, act); }
	void Window::AddMouseEvent(std::string key) {
		Mouses.insert(std::pair<std::string, GMouse>(key, [=](Window& win, sf::RectangleShape& pos, std::string keyAction) {
			Button& button = *win.Buttons[keyAction];
			if (win.MousePos.x <= (int)win.CurrentSize.x && win.MousePos.y <= (int)win.CurrentSize.y && win.MousePos.x >= (int)pos.getPosition().x
				&& win.MousePos.x <= (int)pos.getPosition().x + pos.getSize().x &&
				win.MousePos.y >= (int)pos.getPosition().y && win.MousePos.y <= pos.getPosition().y + pos.getSize().y)
			{
				button.IsActive = true;

				if (button.delay > 0)
					button.delay--;
				else if(button.delay <= 0)
					if (win.LMouse)
					{
						button.L_clicked = 8;
						button.delay = button.max_delay;
						win.Actions[keyAction](win, &button);
					}
			}
			else {
				win.Buttons[keyAction]->IsActive = false;
				if (win.LMouse)
					win.Buttons[keyAction]->no_clicked = 5;
			}

			}));
	}
	void Window::AddEvent(std::string key, GAction act) { Actions.insert(std::pair<std::string, GAction>(key, act)); }
	void Window::AddRect(std::string key, sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
		sf::RectangleShape* rect = new sf::RectangleShape();
		(*rect).setSize(size); (*rect).setPosition(pos); (*rect).setFillColor(color);
		Rectangles.insert(std::pair<std::string, sf::RectangleShape*>(key, rect)); }
	void Window::AddTextEntry(std::string key, sf::Vector2f pos, sf::Vector2f size, float scale) { }
	void Window::AddLClickEvent(std::string key, GAction event) { }
	void Window::AddInfoPanel(std::string key, InfoPanel* panel) { InfoPanels.insert(std::pair<std::string, InfoPanel*>(key, panel)); }
	
	Window::Window() {
		Debug::Log("Creating window...", DType::Info);

		monitor = NULL;
		LMouse = false;
		core = NULL;
		defaultTex.loadFromFile("D:\\SpaRcle\\SpaRcle\\Resources\\default.jpg");
		defaultFont.loadFromFile(Settings::Resources + "\\arial.ttf"); //calibri.ttf a_AlternaNr.ttf

		IsActive = true;

		int iWidth = GetSystemMetrics(SM_CXSCREEN);  // разрешение экрана по горизонтали
		int iHeight = GetSystemMetrics(SM_CYSCREEN); // разрешение экрана по вертикали
		DisplaySize = sf::Vector2u(iWidth, iHeight); //  - 23
		CurrentSize = sf::Vector2u(854, 480);

		G_proc = std::thread([]() {
			auto& wn = *Window::Get();
		Ret:
			if (wn.core == NULL)
				goto Ret;

			sf::RenderWindow window(sf::VideoMode(800, 600), "SpaRcle : Display");
			window.setVerticalSyncEnabled(true); // V-Syns

			sf::Vector2u a = window.getSize();
			typename GTexts::iterator it_texts;
			typename GRects::iterator it_rects;
			typename GSprites::iterator it_sprites;
			typename std::map<std::string, Button*>::iterator it_buttons;
			typename std::map<std::string, InfoPanel*>::iterator it_panels;
			sf::RectangleShape rect;
			InfoPanel* panel = NULL;
			sf::Sprite sprite;

			while (window.isOpen()) {
				if (!Settings::IsActive) { break; }
				Sleep(30);

				wn.MousePos = sf::Mouse::getPosition(window);

				if (wn.CurrentSize.x != a.x || wn.CurrentSize.y != a.y) {
					a = wn.CurrentSize;
					window.setSize(a); }

				sf::Event event;
				while (window.pollEvent(event)) {
					switch (event.type) {
					case sf::Event::Closed: // пользователь попытался закрыть окно: мы закрываем окно
						window.close();
					case sf::Event::Resized:
					{
						float w = static_cast<float>(event.size.width);
						float h = static_cast<float>(event.size.height);
						wn.CurrentSize = sf::Vector2u(size_t(w), size_t(h));
						window.setView(sf::View(
							sf::Vector2f(w / 2.0f, h / 2.0f),
							sf::Vector2f(w, h)));
						break; }
					default: break; }
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

				it_panels = (wn.InfoPanels.begin());
				for (size_t t = 0; t < wn.InfoPanels.size(); t++) {

					window.draw(NormalizeRect(*it_panels->second->border));
					window.draw(NormalizeRect(*it_panels->second->panel));
					for(auto & a: it_panels->second->texts)
						window.draw(NormalizeText(*a));
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

				if (wn.monitor != NULL) { for(auto&a : wn.monitor->lines) window.draw(a); }

				window.display();
			}
			Debug::Log("Stopping window graphics...", Info);
			wn.IsActive = false;
			Settings::IsActive = false;
		});
		L_proc = std::thread([]() {
			auto& wn = *Window::Get();

			typename MouseActions::iterator it_mouse;
			typename std::map<std::string, InfoPanel*>::iterator it_panels;
			sf::RectangleShape rect; InfoPanel* panel = NULL;
			Monitor& m = *wn.monitor;

			while (wn.IsActive) {
				Sleep(10);

				wn.LMouse = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

				it_mouse = (wn.Mouses.begin());
				for (size_t t = 0; t < wn.Mouses.size(); t++) {
					rect = NormalizeRect(*wn.MouseRects[it_mouse->first], (*wn.Buttons[it_mouse->first]).scale);
					if ((*wn.Buttons[it_mouse->first]).L_clicked > 0) { (*wn.Buttons[it_mouse->first]).L_clicked--; }
					rect.setSize(sf::Vector2f(rect.getSize().x * rect.getScale().x, rect.getSize().y * rect.getScale().y));

					it_mouse->second(wn, rect, it_mouse->first); it_mouse++;
				}

				it_panels = (wn.InfoPanels.begin());
				for (size_t t = 0; t < wn.InfoPanels.size(); t++) {
					panel = it_panels->second;
					for (size_t tt = 0; tt < (*panel).texts.size(); tt++)
						(*panel).funcs[tt](wn, *(*panel).texts[tt]);
				}

				for (char c = 0; c < m.poses.size(); c++) {
					m.poses[c].erase(m.poses[c].begin());

					m.lines[c][0].position = Window::NormalizePos(m.pos);
					for (short i = 1; i < m.memory_size; i += 2) {
						m.lines[c][i].position = Window::NormalizePos(m.pos + sf::Vector2f(float(i * m.size), float(-m.poses[c][i])));
						//if(i == 1) m.lines[c][i].position += sf::Vector2f(0, )
						m.lines[c][i + 1].position = m.lines[c][i].position;
					}
				}

				m.poses[0].push_back((*wn.core->_logic).CoreLoad);
				m.poses[1].push_back((*wn.core->_causality).CoreLoad);
				m.poses[2].push_back((*wn.core).CoreLoad);
				m.poses[3].push_back((*wn.core->_causality).CoreLoad);
			}
			Debug::Log("Stopping window logic...", Info);
		});
		Debug::Log("Window is created!", DType::Info);
	}
	Window::~Window() {
		if (L_proc.joinable()) L_proc.detach();
		if (G_proc.joinable()) G_proc.detach(); }

	Window* Window::Get() { static Window* wind = new Window(); return wind; }
	void Window::SetCore(CentralCore* core) { this->core = core; }

	Button::Button(sf::Vector2f pos, sf::Vector2f size, std::string text, float scale, short max_delay) {
		this->max_delay = max_delay;
		this->IsActive = false;
		this->delay = 0; // Important!!!!
		this->L_clicked = 0;
		this->scale = scale;
		this->active = new sf::RectangleShape();
		this->notActive = new sf::RectangleShape();
		this->border = new sf::RectangleShape();
		this->click = new sf::RectangleShape();

		this->text = new sf::Text();
		if (text != "") {
			this->text->setString(text);
			this->text->setPosition(sf::Vector2f(pos.x + 10, pos.y + 8));
			this->text->setFillColor(sf::Color(255, 255, 255));
			this->text->setFont(Window::defaultFont); }

		this->click->setFillColor(sf::Color(0, 150, 0));
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
	Button::Button() {
		this->max_delay = 0;
		this->IsActive = false;
		this->delay = 0; // Important!!!!
		this->L_clicked = 0;
		this->scale = 0;
		this->active = new sf::RectangleShape();
		this->notActive = new sf::RectangleShape();
		this->border = new sf::RectangleShape();
		this->click = new sf::RectangleShape();
		this->text = new sf::Text();;
	}
	Button::~Button() {
		delete this->active;
		delete this->border;
		delete this->click;
		delete this->notActive;
		delete this->text; }
	CheckBox::CheckBox(sf::Vector2f pos, sf::Vector2f size, float scale, short max_delay)// : Button(pos, sf::Vector2f(2 * scale, 2 * scale) , "", scale, max_delay)
	{
		this->max_delay = max_delay;
		this->IsActive = false;
		this->delay = 0; // Important!!!!
		this->L_clicked = 0;
		this->scale = scale;
		this->active = new sf::RectangleShape();
		this->notActive = new sf::RectangleShape();
		this->border = new sf::RectangleShape();
		this->click = new sf::RectangleShape();
		this->text = new sf::Text();
		this->_default = new sf::RectangleShape();
		this->_checked = new sf::RectangleShape();

		this->_checked->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->_checked->setFillColor(sf::Color(0, 100, 0));
		this->_checked->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->_default->setFillColor(sf::Color(100, 100, 100));
		this->_default->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->_default->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));
		this->notActive = this->_default;

		this->active->setFillColor(sf::Color(200, 200, 200));
		this->active->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->active->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->click->setFillColor(sf::Color(20, 20, 20));
		this->click->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->click->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->border->setFillColor(sf::Color(50, 50, 50));
		this->border->setSize(size);
		this->border->setPosition(pos.x, pos.y);

		IsChecked = false;
	}
	CheckBox::CheckBox(){ }
	InfoPanel::InfoPanel(sf::Vector2f pos, sf::Vector2f size, float scale, std::vector<sf::Text*> texts, std::vector < std::function<void(Window& win, sf::Text& text)>> funcs) {
		this->border = new sf::RectangleShape();
		this->panel = new sf::RectangleShape();
		this->scale = scale;

		this->panel->setFillColor(sf::Color(100, 100, 100));
		this->panel->setSize(sf::Vector2f(size.x - 8.f, size.y - 8.f));
		this->panel->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		for (short t = 0; t < (short)texts.size(); t++) {
			texts[t]->setPosition(texts[t]->getPosition() + sf::Vector2f(10, 35 * float(t)) + pos);
		}
		this->texts = texts;
		this->funcs = funcs;

		this->border->setFillColor(sf::Color(50, 50, 50));
		this->border->setSize(size);
		this->border->setPosition(pos.x, pos.y);
	}
	Monitor::Monitor(Window& win) : win{win} {
		pos = sf::Vector2f(400, 550);
		memory_size = 30;
		lines = std::vector<sf::VertexArray>{ 
			sf::VertexArray(sf::Lines, memory_size + 1),
			sf::VertexArray(sf::Lines, memory_size + 1),
			sf::VertexArray(sf::Lines, memory_size + 1),
			sf::VertexArray(sf::Lines, memory_size + 1),
		};
		colors = std::vector<sf::Color>{
			sf::Color(255, 0,0),
			sf::Color(0, 255,0),
			sf::Color(0, 255,255),
			sf::Color(0, 0,255),
		};
		size = 40;

		poses.resize(lines.size());

		for (short i = 0; i < (short)lines.size(); i++)
		{
			poses[i].resize(memory_size);

			for (short s = 0; s < memory_size; s++)
				lines[i][s].color = colors[i];
		}
		Debug::Log("Window::Monitor : creating is sucuessful!", System);
	}
	Monitor::~Monitor() { }
	void Monitor::Resize() {
		for (short t = 3; t< (short)lines.size();t++) {
				lines[t][0].position = Window::NormalizePos(pos);
				for (short i = 1; i < memory_size; i += 2) {
					lines[t][i].position = Window::NormalizePos(pos + sf::Vector2f(i * size, float(rand() % 120 + 1)));
					lines[t][i + 1].position = lines[t][i].position; }
		}
	}
	TextEntry::TextEntry(sf::Vector2f pos, sf::Vector2f size, float scale, std::string defText)  {
		this->max_delay = 20;
		this->IsActive = false;
		this->delay = 0; // Important!!!!
		this->L_clicked = 0;
		this->scale = scale;

		this->font = Window::defaultFont;
		this->active = new sf::RectangleShape();
		this->notActive = new sf::RectangleShape();
		this->border = new sf::RectangleShape();
		this->_checked = new sf::RectangleShape();
		this->_default = new sf::RectangleShape();
		this->click = new sf::RectangleShape();
		this->text = new sf::Text();

		this->text->setFillColor(sf::Color(50, 50, 50));
		this->text->setFont(this->font);
		this->text->setString(defText);
		this->text->setPosition(sf::Vector2f(pos.x + 15, pos.y +10));
		this->text->setScale(sf::Vector2f(size.x / 2, size.y / 2));

		this->_checked->setFillColor(sf::Color(100, 100, 150));
		this->_checked->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->_checked->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->_default->setFillColor(sf::Color(100, 100, 100));
		this->_default->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->_default->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));
		this->notActive = this->_default;

		this->active->setFillColor(sf::Color(200, 200, 200));
		this->active->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->active->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->click->setFillColor(sf::Color(20, 20, 20));
		this->click->setSize(sf::Vector2f(size.x - 2.5f, size.y - 2.5f));
		this->click->setPosition(sf::Vector2f(pos.x + 4.f, pos.y + 4.f));

		this->border->setFillColor(sf::Color(50, 50, 50));
		this->border->setSize(size);
		this->border->setPosition(pos.x, pos.y);
	}
}
// refactoring