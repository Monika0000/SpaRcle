#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <tuple>

namespace SpaRcle {
	enum DType { Info = 1, Log = 2, Warning = 3, Error = 4, System = 5, Mind = 6, Module = 7 };

	enum DClass { D_Central, D_Logic, D_Causality, D_Reality, D_Emotion };

	enum ConsoleColor {
		Black = 0,
		Blue = 1,
		Green = 2,
		Cyan = 3,
		Red = 4,
		Magenta = 5,
		Brown = 6,
		LightGray = 7,
		DarkGray = 8,
		LightBlue = 9,
		LightGreen = 10,
		LightCyan = 11,
		LightRed = 12,
		LightMagenta = 13,
		Yellow = 14,
		White = 15
	};

	class Debug
	{
	public:
		Debug();
		~Debug();
		static void Log(std::string mess, bool nline, SpaRcle::DType type = DType::Log, SpaRcle::ConsoleColor type_color = Black);
		static void Log(std::string mess, SpaRcle::DType type = DType::Log, SpaRcle::ConsoleColor type_color = Black);
		static void Log(int mess, bool nline = true, DType type = DType::Log, ConsoleColor type_color = Black);

		static void StartDebugger();
		static void StopDebugger();

		static unsigned int Errors;
		static unsigned int Warnings;
		static unsigned int Info;
		static unsigned int Logs;
		static unsigned int Mind;
		static unsigned int System;
		static unsigned int Module;
	private :
		static void DebuggerSolution();

		static std::vector<std::tuple<std::string, bool, SpaRcle::DType, SpaRcle::ConsoleColor>> Messages;

		static bool IsStart;

		static std::thread Process;
	};
}
