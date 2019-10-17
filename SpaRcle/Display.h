#pragma once
#include <iostream>
#include "GL/glut.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include "CentralCore.h"

namespace SpaRcle {
	class Display
	{
	private:
		static std::thread display;
	public:
		static void StartDisplay(CentralCore* core);
		static void Deactive();
		static int Status;
	};
}