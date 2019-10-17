#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>

namespace SpaRcle {
	enum DType {
		Info = 1, Log = 2, Warning = 3, Error = 4, System = 5, Mind = 6
	};

	enum DClass
	{
		Central, Logic, Causality, Reality, Emotion
	};

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
		static void Log(std::string mess, bool nline, SpaRcle::DType type = DType::Log, SpaRcle::ConsoleColor type_color = Black);

		static void Log(std::string mess, SpaRcle::DType type = DType::Log, SpaRcle::ConsoleColor type_color = Black);

		static void Log(int mess, bool nline = true, DType type = DType::Log, ConsoleColor type_color = Black);

		static void StartDebugger();

		static void StopDebugger();

	private :
		static void DebuggerSolution();

		static std::vector<boost::tuple<std::string, bool, SpaRcle::DType, SpaRcle::ConsoleColor>> Messages;

		static bool IsStart;

		static std::thread Process;
	};
}