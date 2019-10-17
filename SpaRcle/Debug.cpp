#include "pch.h"
#include "Debug.h"
#include "Helper.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <stdio.h>

namespace SpaRcle {
	void Debug::Log(std::string mess, bool nline, SpaRcle::DType type, SpaRcle::ConsoleColor type_color)
	{
		//std::cout << "asd" << std::endl;
		//try {
		//	if (Settings::Status >= 0)
				Messages.push_back(boost::tuple<std::string, bool, DType, ConsoleColor>(mess, nline, type, type_color));
		//}
		//catch (...) {

		//}
	}

	void Debug::Log(std::string mess, SpaRcle::DType type, SpaRcle::ConsoleColor text_color) {
		Debug::Log(mess, true, type, text_color);
	}

	void Debug::Log(int mess, bool nline, DType type, ConsoleColor text_color)
	{
		std::string s = Helper::format() << mess;
		Debug::Log(std::basic_string(s), nline, type, text_color);
	}

	bool Debug::IsStart = false;
	std::vector<boost::tuple<std::string, bool, DType, ConsoleColor>> Debug::Messages;
	std::thread Debug::Process;

	void Debug::DebuggerSolution() {
		//std::string p = Settings::SysDir + "\\Log.txt"; // path to system file
		//std::ofstream fout; fout.open(p);
		//fout << "[=================== START ===================]" << std::endl;
		while (true) {
			try {
				if (Settings::Status < 0 && Debug::Messages.size() == 0) {
					//std::cout << Settings::Status << std::endl;
					continue;
				}

				if (Messages.size() > 0) {
					HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

					std::string pref;
					switch (Messages[0].get<2>())//type
					{
					case SpaRcle::Info:
						pref = "[Info] ";
						if (Messages[0].get<3>() == Black)
							Messages[0].get<3>() = ConsoleColor::Magenta;
						break;
					case SpaRcle::Log:
						pref = "[Log] ";
						if (Messages[0].get<3>() == Black)
							Messages[0].get<3>() = ConsoleColor::Cyan;
						break;
					case SpaRcle::Warning:
						pref = "[Warning] ";
						if (Messages[0].get<3>() == Black)
							Messages[0].get<3>() = ConsoleColor::Yellow;
						break;
					case SpaRcle::Error:
						pref = "[Error] ";
						if (Messages[0].get<3>() == Black)
							Messages[0].get<3>() = ConsoleColor::Red;
						break;
					case SpaRcle::System:
						pref = "[System] ";
						if (Messages[0].get<3>() == Black)
							Messages[0].get<3>() = ConsoleColor::Blue;
						break;
					case SpaRcle::Mind:
						pref = "[Mind] ";
						if (Messages[0].get<3>() == Black)
							Messages[0].get<3>() = ConsoleColor::Green;
						break;
					default:
						break;
					}

					SetConsoleTextAttribute(hConsole, (WORD)((LightGray << 4) | Messages[0].get<3>()));

					std::cout << pref;

					SetConsoleTextAttribute(hConsole, (WORD)((LightGray << 4) | Black));

					if (Messages[0].get<1>())
						std::cout << Messages[0].get<0>() << std::endl;
					else
						std::cout << Messages[0].get<0>();

					//fout << (pref + Messages[0].get<0>()) << std::endl;

					Messages.erase(Messages.begin());
				}
			}
			catch (...) {

			}
		}
	}

	void Debug::StartDebugger()
	{
		if (!IsStart) {
			Process = std::thread(DebuggerSolution);
		}
	}

	void Debug::StopDebugger()
	{
		if (Debug::Process.joinable())
			Debug::Process.detach();
	}
}