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
		static std::string Actions;
		/* �����������-�� ������� ����������*/
		static bool PathsIsSet; 
		///////////////////////////////
		/* ������� ������ ���� */
		static bool CoreDebug; 
		static bool ActionDebug;
		static bool DEOSDebug;
		static bool DFSDebug;
		static bool ExternalInteraction;
		static KeyboardLayout Layout;
		static bool CentralCoreDebug;
		static bool CauseReputationDebug;
		/* ������� ��������� ����������� �������*/
		static bool EventsProcessigDebug; 
		static const int SaveNumbers;
		/* Synapse & Cause & Perhaps | Min = 5 | ��������� <=> ������������������ */
		static const size_t Size_SCP; 
		/* size monotone list in Logical core*/
		static const size_t Monotone_size;
		static float Monotone_MaxPercent;
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

		static bool dynamic_win_var_1;

		static void SetDefaultPaths(std::string project);
		static void SetPaths(std::string proj, std::string sys, std::string res, std::string mem, std::string logic, std::string actions);

		static bool Diagnostic();
	private:

	protected:

	};

	const static double Div = 1.8f;

	const static size_t size_t_max = 4294967295;

	const double PeriodicSix = (1.0 / 3.0) * 200.0;

	const static size_t count_word_in_sensiv = 2;

	static const size_t max_size_SEWMA = 25; // Max size sensivity events with my actions.
}

