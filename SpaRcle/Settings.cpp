#include "pch.h"
#include "Settings.h"
#include "LogicalCore.h"
#include "CausalityCore.h"
#include "Helper.h"

namespace SpaRcle {
	std::string Settings::PathPj = "D:\\SpaRcle\\Release";
	std::string Settings::SysDir = "\\System";
	std::string Settings::Resources = "\\Resources";
	std::string Settings::Memory = "\\Memory";
	std::string Settings::Logic = "\\Logic";
	bool Settings::PathsIsSet = false;
	////////////////////////////
	bool Settings::CoreDebug = false;
	KeyboardLayout Settings::Layout = UNKNOWN;
	bool Settings::EventsProcessigDebug = true;
	const int Settings::SaveNumbers = 5;
	const size_t Settings::Size_SCP = 5;
	std::string Settings::version = "0.1_alpha_sparcle";
	const std::string Settings::EmptyName = "[NTHG]";
	int Settings::Status = 0;
	////////////////////////////
	std::string Settings::exp_conseq = ".cnq"; // .cnq
	std::string Settings::exp_action = ".act";

	void Settings::SetDefaultPaths(std::string project)
	{
		Layout = Helper::GetLayout();
		PathPj = project;
		SysDir = PathPj + "\\System";
		Resources = PathPj + "\\Resources";
		Memory = PathPj + "\\Memory";
		Logic = PathPj + "\\Logic";
		PathsIsSet = true;
	}

	void Settings::SetPaths(std::string proj, std::string sys, std::string res, std::string mem, std::string logic)
	{
		PathPj = proj;
		SysDir = sys;
		Resources = res;
		Memory = mem;
		Logic = logic;
		PathsIsSet = true;
	}



	bool Settings::Diagnostic()
	{
		std::string temp = "%temp%\\";

		boost::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, std::string> _Cause(
			{ "S/Бежал", "S/Упал", "S/Прыгал", "S/Плавал", "S/Ходил" },  // Причины
			{ 8, 22, 10, 2, 5 }, // Колличество встреч в данном следствии
			{ "QWERT" , "WERT", "ERT", "RT", "T" },
			-15.0f, "Больно"); // Результат следствия

		//if (!LogicalCore::CauseReputation(_Cause, true))
		//	return false;

		///////////////////////////////////////////////////////////////////////////////

		std::vector<std::string>Temp_Causes({  }); /// Затычка
		std::vector<int>Temp_Meets({ });		   /// Затычка

		std::vector<Consequence>CheckedEvents({
			Consequence("Пошел", Action(Sound("Пошел")), 12),
			Consequence("Пошел", Action(Sound("Пошел")), 12),
			Consequence("Пошел", Action(Sound("Пошел")), 12),
			Consequence("Пошел", Action(Sound("Пошел")), 12),
			Consequence("Пошел", Action(Sound("Пошел")), 12),
			Consequence("Пошел", Action(Sound("Пошел")), 12),
			Consequence("Побежал", Action(Sound("Побежал")), 12),
			Consequence("Побежал", Action(Sound("Побежал")), 12),
			Consequence("Побежал", Action(Sound("Побежал")), 12),
			Consequence("Прыгнул", Action(Sound("Прыгнул")), 12),
			Consequence("Упал", Action(Sound("Упал")), 12, 15, -5) });

		//if (!CausalityCore::CheckedEventsProcessing(CheckedEvents, Temp_Causes, Temp_Meets, true))
		//	return false;

		if (false) {
			Consequence event("Open");
			event.Causes.push_back(boost::tuple<std::string, int, double>("Close", 28, 0.f));
			//event.Causes.push_back(boost::tuple<std::string, int, double>("Break", 1, 0.f));
			//event.Causes.push_back(boost::tuple<std::string, int, double>("Week", 2, 0.f));
			event.Causes.push_back(boost::tuple<std::string, int, double>("Home", 8, 0.f));
			event.Causes.push_back(boost::tuple<std::string, int, double>("Door", 12, 0.f));

			Consequence opposite;
			if (LogicalCore::GetOpposite(opposite, event, true))
			{
				Debug::Log("Diagnostic::GetOpposite : " + event.name + " => " + opposite.name); // +
				//	"\n\tMeets : " + std::to_string(event.meetings) + " => " + std::to_string(opposite.meetings));
			}
			else return false;
		}

		return true;
	}

	// 0.1_alpha : 18 июля 2019 года ‏‎17:07:40 создан проект.


	//std::vector<boost::tuple<std::string, double>>({	 /* Syn */ }),
	//std::vector<boost::tuple<std::string, int, double>>({ /* PWill */ }),
	//std::vector<boost::tuple<std::string, int, double>>({ /* Cas */ }))}
}