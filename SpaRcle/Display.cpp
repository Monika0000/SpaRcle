#include "pch.h"
#include <iostream>
#include "GL/glut.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include "Button.h"
#include "CheckBox.h"
#include "TextEntry.h"
#include "StatusBox.h"
#include "Display.h"
#include "Selector.h"
#include "Table.h"
#include "Debug.h"
#include "Consequence.h"
#include <vector>

namespace SpaRcle {
	int Display::Status = 0;
	std::thread Display::display;

	static const sf::Color BACKGROUND_COLOR = sf::Color(125, 125, 125);

	static void MainDialog(CentralCore* core) {
		sf::Font defaultFont;
		defaultFont.loadFromFile(Settings::Resources + "\\arial.ttf"); //calibri.ttf a_AlternaNr.ttf

		//sf::Window window(sf::VideoMode(800, 600), "SpaRcle : Display");
		sf::RenderWindow window(sf::VideoMode(800, 600), "SpaRcle : Display");
		window.setVerticalSyncEnabled(true); // V-Syns

		#pragma region [---Buttons---]
		Button *exit = new Button(sf::Vector2f(2.5f, 567.5f), sf::Vector2f(80.f, 30.f), &window, "  [One Time]", defaultFont, 10, 12, sf::Color::Blue);
			(*exit).setBorder(sf::Color(50, 50, 50), 2);
			(*exit).setButtonColor(sf::Color(125, 155, 125));
			(*exit).setButtonActiveColor(sf::Color(100, 100, 100));

		Button *exit2 = new Button(sf::Vector2f(90.5f, 567.5f), sf::Vector2f(112.f, 30.f), &window, "      [More Time]", defaultFont, 0, 12, sf::Color::Blue);
			(*exit2).setBorder(sf::Color(50, 50, 50), 2);
			(*exit2).setButtonColor(sf::Color(125, 155, 125));
			(*exit2).setButtonActiveColor(sf::Color(100, 100, 100));

		TextEntry *entry = new TextEntry(sf::Vector2f(2.5f, 540.5f), sf::Vector2f(200.f, 23.f), &window, defaultFont);
		CheckBox *checkbox = new CheckBox(sf::Vector2f(2.5f, 510.5f), 2, &window);

		Selector* select = new Selector(sf::Vector2f(205.0f, 572.5f), &window, defaultFont, 
			{ "Monotone", "Agressive", "Positive" });

		Button* hello = new Button(sf::Vector2f(2.5f, 470.5f), sf::Vector2f(100.f, 25.f), &window, "  [Hello Monika]", defaultFont, 10, 12, sf::Color::Blue);
			(*hello).setBorder(sf::Color(50, 50, 50), 2);
			(*hello).setButtonColor(sf::Color(125, 155, 125));
			(*hello).setButtonActiveColor(sf::Color(100, 100, 100));

		Button* how_are_you = new Button(sf::Vector2f(2.5f, 442.5f), sf::Vector2f(100.f, 25.f), &window, "  [How are you]", defaultFont, 10, 12, sf::Color::Blue);
			(*how_are_you).setBorder(sf::Color(50, 50, 50), 2);
			(*how_are_you).setButtonColor(sf::Color(125, 155, 125));
			(*how_are_you).setButtonActiveColor(sf::Color(100, 100, 100));

		Button* you_baka = new Button(sf::Vector2f(2.5f, 412.5f), sf::Vector2f(100.f, 25.f), &window, "     [You baka]", defaultFont, 10, 12, sf::Color::Blue);
			(*you_baka).setBorder(sf::Color(50, 50, 50), 2);
			(*you_baka).setButtonColor(sf::Color(125, 155, 125));
			(*you_baka).setButtonActiveColor(sf::Color(255, 100, 100));
		#pragma endregion

		#pragma region Indicators
		int dist = 32;

		Table* EventsList = new Table(&window, defaultFont, 680, 0, 12, 60);
		Table* SensivityList = new Table(&window, defaultFont, 550, 0, 12.5f, 60);

		StatusBox *CentralCoreStatus = new StatusBox(sf::Vector2f(2.5f, 5.f), 3, &window, 3);
		(*CentralCoreStatus).SetText(defaultFont, "[CentralCore]", dist);

		StatusBox *RealityCoreStatus = new StatusBox(sf::Vector2f(2.5f, 5.f + dist), 3, &window, 0);
		(*RealityCoreStatus).SetText(defaultFont, "[RealityCore]", dist);

		StatusBox *LogicalCoreStatus = new StatusBox(sf::Vector2f(2.5f, 5.f + dist * 2), 3, &window, 1);
		(*LogicalCoreStatus).SetText(defaultFont, "[LogicalCore]", dist);
		#pragma endregion

		while (window.isOpen()) // программа буд ет запущена до тех пор, пока окно открыто
		{
			Sleep(10);

			// проверка всех событий окна, произошедших с последней итерации цикла
			sf::Event event;
			while (window.pollEvent(event)) /* Events */
			{
				switch (event.type)
				{
				case sf::Event::Closed: // пользователь попытался закрыть окно: мы закрываем окно
					window.close();
				case sf::Event::Resized:
				{
					double w = static_cast<double>(event.size.width);
					double h = static_cast<double>(event.size.height);
					window.setView(sf::View(
						sf::Vector2f(w / 2.0, h / 2.0),
						sf::Vector2f(w, h)));
					break;
				}
				default:
					break;
				}
			} /* Events */

			window.clear(BACKGROUND_COLOR);

			if ((*exit).draw()) (*core)._causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
			if ((*exit2).draw()) (*core)._causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
			if ((*hello).draw()) {
				(*core)._causality->UncheckedEvents.push_back(Consequence(Sound("hello", 10, 15)));
				(*core)._causality->UncheckedEvents.push_back(Consequence(Sound("monika", 10,15)));
			}
			if ((*how_are_you).draw()) {
				(*core)._causality->UncheckedEvents.push_back(Consequence(Sound("how", 10, 15)));
				(*core)._causality->UncheckedEvents.push_back(Consequence(Sound("are", 10, 15)));
				(*core)._causality->UncheckedEvents.push_back(Consequence(Sound("you", 10, 15)));
			}
			if ((*you_baka).draw()) {
				(*core)._causality->UncheckedEvents.push_back(Consequence(Sound("you", 6, 35)));
				(*core)._causality->UncheckedEvents.push_back(Consequence(Sound("baka", 6, 35)));
			}

			checkbox->draw();

			CentralCoreStatus->draw();
			RealityCoreStatus->draw();
			LogicalCoreStatus->draw();

			int selectResult = select->draw();

			std::string entrytext = entry->draw();
			if (entrytext != "")
			{
				std::vector<std::string> arr = Helper::Split(entrytext, " ");

				for (std::string var : arr) {
					if (var == " " || var == "")
						continue;

					if (var == ".")
						var = Settings::EmptyName;
					else
						for (int i = 0; i < var.size(); i++)
							var[i] = tolower(var[i]);

					//std::cout << 423432 << std::endl;
					switch (selectResult)
					{
					case 0:
						core->_causality->NewEvent(Consequence(var, Action(Sound(var, 0, 15)))); /* monotone*/ break;
					case 1:
						core->_causality->NewEvent(Consequence(var, Action(Sound(var, -20, 35)))); /*agressive*/ break;
					case 2:
						core->_causality->NewEvent(Consequence(var, Action(Sound(var, 10, 15)))); /* positive*/ break;
					default:
						//std::string token = Helper::format() << selectResult;
						std::string token = std::to_string(selectResult);
						Debug::Log("Display::MainDialog = Error : unknown token \"" + token + "\"", Error);
						break;
					}
				}
				//core->_reality->DoAction(Action(Sound("Display : "+entrytext)));
				//core->_causality->NewEvent(Consequence(entrytext, Action(Sound(entrytext))));
			}

			EventsList->draw((*core)._causality->CheckedEvents);
			SensivityList->draw((*core)._causality->Sensivity_List);

			window.display();
		}

		Display::Status = 2;
		return;
	}

	void Display::StartDisplay(CentralCore* core) {
		if (Status == 0) {
			display = std::thread(MainDialog, core);
			Status = 1; }
		else SpaRcle::Debug::Log("Display already is started!"); }

	void Display::Deactive() {
		if (display.joinable())
			display.detach(); }
}