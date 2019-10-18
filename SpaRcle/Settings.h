#pragma once
#include <iostream>
#include <string>

namespace SpaRcle {
	enum KeyboardLayout {
		EN, RU, UNKNOWN
	};
	class Settings
	{
	public:
		static std::string PathPj;
		// PathProject += ------------->
		static std::string SysDir;
		static std::string Resources;
		static std::string Memory;
		static std::string Logic;
		static bool PathsIsSet; // Установлена-ли рабочая директория
		///////////////////////////////
		static bool CoreDebug; // Отладка работы ядер
		static KeyboardLayout Layout;
		static bool EventsProcessigDebug; // Отладка обработки проверенных событий
		static const int SaveNumbers;
		static const size_t Size_SCP; /* Synapse & Cause & Perhaps | Min = 5 | Интеллект <=> Производительность */
		static std::string version;
		static const std::string EmptyName;
		static int Status;
		///////////////////////////////
		static std::string exp_conseq;
		static std::string exp_action;

		static void SetDefaultPaths(std::string project);
		static void SetPaths(std::string proj, std::string sys, std::string res, std::string mem, std::string logic);

		static bool Diagnostic();
	private:

	protected:

	};

	const static double Div = 1.5f;

	const double PeriodicSix = (1.f / 3.f) * 200.f;

	const static size_t count_word_in_sensiv = 1;

	static const size_t max_size_SEWMA = 25; // Max size sensivity events with my actions.
}

