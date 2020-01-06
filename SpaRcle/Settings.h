#pragma once
#include <iostream>
#include <string>

namespace SpaRcle {
	enum KeyboardLayout { EN, RU, UNKNOWN };
	class Settings {
	public:
		static std::string PathPj;
		static std::string Drive;
		// PathProject += ------------->
		static std::string SysDir;
		static std::string Resources;
		static std::string Memory;
		static std::string Logic;
		static bool PathsIsSet; // Установлена-ли рабочая директория
		///////////////////////////////
		static bool CoreDebug; // Отладка работы ядер
		static bool ExternalInteraction;
		static KeyboardLayout Layout;
		static bool CentralCoreDebug;
		static bool CauseReputationDebug;
		static bool EventsProcessigDebug; // Отладка обработки проверенных событий
		static const int SaveNumbers;
		static const size_t Size_SCP; /* Synapse & Cause & Perhaps | Min = 5 | Интеллект <=> Производительность */
		static double MinSimilarityPerc;
		static std::string version;
		static const std::string EmptyName;
		static const std::string ClearEvent;
		static const char TrueSymbol;
		static bool IsActive;
		static bool isMinding;
		static bool isUseMemory;
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

	const static size_t size_t_max = 4294967295;

	const double PeriodicSix = (1.0 / 3.0) * 200.0;

	const static size_t count_word_in_sensiv = 2;

	static const size_t max_size_SEWMA = 25; // Max size sensivity events with my actions.
}

