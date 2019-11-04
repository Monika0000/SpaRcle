// SpaRcle.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Visual Studio\\SFML-2.5.1\\lib\\sfml-graphics.lib")
#include "pch.h"
#include <iostream>
#include "Display.h"
#include "CentralCore.h"
#include "Window.h"
#include "CausalityCore.h"
#include "LogicalCore.h"
#include "RealityCore.h"
#include "EmotionCore.h"
#include "Debug.h"
#include "Helper.h"
#include "Settings.h"
#include <string>
#include <thread>
#include <stdexcept>
using namespace SpaRcle;

#pragma region Propetries;
CentralCore* _core;
CausalityCore* _causality;
LogicalCore* _logic;
RealityCore* _reality;
EmotionCore* _emotion;
void Destruct() {
	delete _emotion;
	delete _reality;
	delete _logic;
	delete _causality;
	delete _core;
}
#pragma endregion
 
int main(int argc, char** argv)
{
	system("color 70");
	setlocale(LC_ALL, "rus");
	ShowWindow(GetConsoleWindow(), SW_NORMAL);

	#pragma region [======== Pre-Init ========]
	Settings::IsActive = true;
	Debug::StartDebugger();
	Sleep(100);
	Debug::Log("Pre-initializing", DType::System);

	Settings::CoreDebug = false;
	TCHAR cwd[100];
	GetCurrentDirectory(100, cwd);
	std::wstring ws(&cwd[0]);
	Settings::SetPaths(std::string(ws.begin(), ws.end()),
		"D:\\SpaRcleData\\System",
		"D:\\SpaRcle\\SpaRcle\\Resources",
		"D:\\SpaRcleData\\Memory",
		"D:\\SpaRcleData\\Logic");
	Debug::Log("~ DIR \"" + Settings::PathPj + "\"");

	if (!Settings::Diagnostic())
		Debug::Log("Diagnostic... System is unstable!", true, DType::System, SpaRcle::Red);
	else
	{
		boost::tuple<int> r;
		Debug::Log("Diagnostic... System is stable!", DType::System, SpaRcle::Green);

		//std::cout << Helper::SimilarityPercentage("12", "123") << std::endl;
		//Sleep(225552);

		//////////////////////////////
		_core = new CentralCore(5);
		_causality = new CausalityCore(5);
		_logic = new LogicalCore(5);
		_reality = new RealityCore(100);
		_emotion = new EmotionCore(1500);
#pragma endregion

	#pragma region [======== Init ========]
		Debug::Log("Initializing", DType::System);
		//Display::StartDisplay(_core); // Graph-mode
		Window::Get()->SetCore(_core); Window::Get()->AddAllElements();

		_core->ConnectLogic(_logic);
		_core->ConnectCausality(_causality);
		_core->ConnectReality(_reality);
		_core->ConnectEmotion(_emotion);

		Sleep(100);
		_reality->Start();

		Sleep(100);
		_logic->Start();

		Sleep(100);
		_causality->Start();

		Sleep(100);
		_emotion->Start();
		Sleep(100);

		_core->Start();
		Sleep(100);
#pragma endregion

		while (true)
		{
			Sleep(100);
			Settings::Layout = Helper::GetLayout();
			if (!Settings::IsActive)
				break;

			if (Display::Status == 2)
				break;
			else if (Display::Status == 0)
			{
				//std::cout << "Для продолжения нажмите Enter\n";
				Debug::Log("Starting are successful. For exit press the Enter...", DType::System);
				std::cin.get();
				Settings::IsActive = false;
				break;
			}
		}

		std::cout << "==================================[STOP]==================================" << std::endl;
		Sleep(2000);
		Destruct();
		delete Window::Get();
		Display::Deactive();
		Sleep(2000);

		Debug::StopDebugger();
		Sleep(2000);
		std::cout << std::endl;

		return 0;
	}

	Debug::StopDebugger();
	Sleep(1000);

	return -1;
}