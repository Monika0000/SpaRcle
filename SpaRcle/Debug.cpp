#include "pch.h"
#include "Debug.h"
#include "Helper.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <stdio.h>

namespace SpaRcle {
	unsigned int Debug::Logs = 0;
	unsigned int Debug::Errors = 0;
	unsigned int Debug::Warnings = 0;
	unsigned int Debug::Info = 0;
	unsigned int Debug::Mind = 0;
	unsigned int Debug::System = 0;
	unsigned int Debug::Module = 0;

	Debug::Debug() { }
	Debug::~Debug() { }
	void Debug::Log(std::string mess, bool nline, SpaRcle::DType type, SpaRcle::ConsoleColor type_color)
	{ Messages.push_back(std::tuple<std::string, bool, DType, ConsoleColor>(mess, nline, type, type_color)); }
	void Debug::Log(std::string mess, SpaRcle::DType type, SpaRcle::ConsoleColor text_color) { Debug::Log(mess, true, type, text_color); }
	void Debug::Log(int mess, bool nline, DType type, ConsoleColor text_color) {
		std::string s = std::to_string(mess);
		Debug::Log(std::basic_string(s), nline, type, text_color); }

	bool Debug::IsStart = false;
	std::vector<std::tuple<std::string, bool, DType, ConsoleColor>> Debug::Messages;
	std::thread Debug::Process;

	void Debug::DebuggerSolution() {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		while (true) {
			if (Messages.size() == 0)Sleep(20);
			if (!IsStart) { break; }
			try {
				if (!Settings::IsActive && Debug::Messages.size() == 0) { continue; }

				if (Messages.size() > 0) {
					ConsoleColor& color = std::get<3>(Messages[0]);

					std::string pref;
					switch (std::get<2>(Messages[0]))//type
					{
					case SpaRcle::Info:
						Debug::Info++;
						pref = "[Info] ";
						if (color == Black) color = ConsoleColor::Magenta;
						break;
					case SpaRcle::Log:
						pref = "[Log] ";
						Debug::Logs++;
						if (color == Black) color = ConsoleColor::Cyan;
						break;
					case SpaRcle::Warning:
						pref = "[Warning] ";
						Debug::Warnings++;
						if (color == Black) color = ConsoleColor::Yellow;
						break;
					case SpaRcle::Error:
						Debug::Errors++;
						pref = "[Error] ";
						if (color == Black) color = ConsoleColor::Red;
						break;
					case SpaRcle::System:
						Debug::System++;
						pref = "[System] ";
						if (color == Black) color = ConsoleColor::Blue;
						break;
					case SpaRcle::Mind:
						Debug::Mind++;
						pref = "[Mind] ";
						if (color == Black) color = ConsoleColor::Green;
						break;
					case SpaRcle::Module:
						Debug::Module++;
						pref = "[Module] ";
						if (color == Black) color = ConsoleColor::Brown;
						break;
					default:
						break;
					}

					SetConsoleTextAttribute(hConsole, (WORD)((LightGray << 4) | color));
					std::cout << pref;
					SetConsoleTextAttribute(hConsole, (WORD)((LightGray << 4) | Black));

					if (std::get<1>(Messages[0])) std::cout << std::get<0>(Messages[0]) << std::endl;
					else std::cout << std::get<0>(Messages[0]);
					
					std::get<0>(Messages[0]).~basic_string();
					Messages.erase(Messages.begin());
					pref.~basic_string();
				}
			}
			catch (...) { std::cout << "[Error] Debug::Log : An exception has occured!" << std::endl; }
		}
		std::cout << "==================================[EXIT]==================================" << std::endl;
		Debug::Messages.clear();
	}

	void Debug::StartDebugger() {
		if (!IsStart) {
			IsStart = true;
			Process = std::thread(DebuggerSolution); } }

	void Debug::StopDebugger() {
		Debug::Log("Stopping debugger...", System);
		Sleep(2000); IsStart = false;
		if (Debug::Process.joinable()) Debug::Process.detach(); }
}