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
#include "Memory.h"
#include <stdexcept>
#include "TCP.h"

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

	char buf[255]; _getcwd(buf, 255); Settings::Drive = "D";// std::string(1, buf[0]);

	Settings::SetPaths(std::string(ws.begin(), ws.end()),
		std::string(1, buf[0]) + ":\\SpaRcle\\System",
		std::string(1, buf[0]) + ":\\SpaRcle\\SpaRcle\\Resources",
		Settings::Drive + ":\\SpaRcleData\\Memory",
		Settings::Drive + ":\\SpaRcleData\\Logic");
	Debug::Log("~ DIR \"" + Settings::PathPj + "\"");

	if (!Settings::Diagnostic())
		Debug::Log("Diagnostic... System is unstable!", true, DType::System, SpaRcle::Red);
	else
	{
		//boost::tuple<int> r;
		Debug::Log("Diagnostic... System is stable!", DType::System, SpaRcle::Green);

		//std::cout << Helper::SimilarityPercentage("12", "123") << std::endl;
		//Sleep(225552);

		//////////////////////////////
		_core = new CentralCore(50);
		_causality = new CausalityCore(50);
		_logic = new LogicalCore(50);
		_reality = new RealityCore(50);
		_emotion = new EmotionCore(50);

		TCP tcp = TCP(_core);
#pragma endregion

	#pragma region [======== Init ========]
		Debug::Log("Initializing", DType::System);
		//Display::StartDisplay(_core); // Graph-mode
		Window::Get()->SetCore(_core); Window::Get()->AddAllElements();

		_core->ConnectLogic(_logic);
		_core->ConnectCausality(_causality);
		_core->ConnectReality(_reality);
		_core->ConnectEmotion(_emotion);

		tcp.Start();

		
		//size_t index = 0;
		//std::string line = "event S monika~event S how~event S are~event S you";
	//ret:
		//Sleep(100);
		/*
		for (std::string s : Helper::Split(line, " ", "~", index))
			Debug::Log(s + " ("+std::to_string(index)+")");
		Debug::Log("==============");
		if (index != ::size_t_max)
		{
			line = line.substr(index);
			index = 0;
			goto ret;
		}
		*/
		//while (true) { }

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

		Memory::GetMemory()->CopyFile("memory_mapped_file.hpp", "copy.tmp", true);
#pragma endregion

		while (true) {
			Sleep(100); Settings::Layout = Helper::GetLayout();
			if (!Settings::IsActive) break;

			if (Display::Status == 2) break;
			else if (Display::Status == 0) {
				//std::cout << "Для продолжения нажмите Enter\n";
				Debug::Log("Starting are successful. For exit press the Enter...", DType::System);
				std::cin.get();
				Settings::IsActive = false;
				break;
			}
		}

		std::cout << "==================================[STOP]==================================" << std::endl;
		Sleep(2000);
		tcp.Close();
		Destruct();
		delete Window::Get();
		Display::Deactive();
		delete Memory::GetMemory();
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