#include "pch.h"
#include "windows.h"
#include "CentralCore.h"
#include "RealityCore.h"
#include "CausalityCore.h"
#include "LogicalCore.h"
#include <iostream>
#include <vector>
#include "Settings.h"
#include "Debug.h"
#include "Helper.h"

namespace SpaRcle {
	LogicalCore::LogicalCore(int cpuSpeed) {
		core = NULL;
		CoreLoad = 0;
		DelayCPU = cpuSpeed;

		Tasks = std::vector<Task>(); Tasks.resize(0);

		Debug::Log("-> Creating the logical core are successful!"); }
	LogicalCore::~LogicalCore() {
		if (Process.joinable()) Process.detach();
		this->Causes.clear();
		Debug::Log("-> The logical core has completed it's work!", Info); }
	 
	void LogicalSolution(int* delay, LogicalCore* _core) {
		CentralCore& core = *(*_core).core;
		RealityCore& real = *core._reality;
		CausalityCore& causal = *core._causality;
		char timer = 0, load = 0;

		Debug::Log("-> The logical core is started!");
		size_t& check_ev_size= causal.size_check_ev, uncheck_ev_size=causal.size_unchk_ev;

		while (Settings::IsActive) {
			if (timer >= 100) { timer = 0; _core->CoreLoad = load; load = 0; } else timer++;
			try {
				if (check_ev_size <= Settings::Size_SCP * 2 + 1 && uncheck_ev_size == 0 && core.Events_sens.size() == 0) {
					if ((*_core).Causes.size() > 0) {
						load++;
						LogicalCore::CauseReputation((*_core).Causes[0]); ///\see Get first element

						_core->Causes[0].get<0>().clear();
						_core->Causes[0].get<1>().clear();
						_core->Causes[0].get<2>().clear();
						(*_core).Causes.erase((*_core).Causes.begin());   /// Delete first element
					}
				}
				else Sleep(*delay);
			}
			catch (...) { Debug::Log("LogicalSolution : An exception has ocurred!", Error); break; }
			if (Settings::CoreDebug) Debug::Log("Processing logical... ");
		}
	}
	void LogicalCore::Start() { Process = std::thread(LogicalSolution, &DelayCPU, this); /*union d { int f = 5; };*/ }

	void LogicalCore::DoIt(Task task) {
		bool has = false;
		if(Tasks.size() != 0)
			for (Task t : Tasks)
				if (t.result.name == task.result.name)
					has = true;

		if (!has) {
			if (Tasks.size() == maxTasks) {
				Tasks.erase(Tasks.begin()); // first element
				Tasks.push_back(task); // add element to end
				Debug::Log("____" + Tasks[0].result.name + Tasks[1].result.name + Tasks[2].result.name + Tasks[3].result.name + 
					Tasks[4].result.name + Tasks[5].result.name + Tasks[6].result.name + Tasks[7].result.name);
			}
			else Tasks.push_back(task); // add element to end

			Debug::Log("LogicalCore.DoIt = New task : \"" + task.result.name + "\"", Info);
		}
	}
	void LogicalCore::EditCauses(std::vector<std::string>& Causes, std::vector<int>& Meets, std::vector<std::string> Sensivitys, Consequence& conq) {
		std::string name = std::string(1, ToString(conq.action.type)[0]) + "/" + conq.name;
		this->Causes.push_back(boost::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, std::string>
			(Causes, Meets, Sensivitys, conq.GetSummHP(), name)); // ƒобавл€ем элемент в конец
		name.~basic_string();
		Sensivitys.clear();
		Sensivitys.~vector();
	}
	std::vector<std::string> LogicalCore::DecomposeConsequence(Consequence& conseq) {
		/*
			–аскладываем следствие на состовл€ющие
			ѕример : четыре состоит из :
				два + два
				три + один
				четыре + ноль
		*/
		std::string name = Helper::GetNameWithType(conseq);

		std::vector<std::string> decomps;
		for (size_t t = 0; t < conseq.Causes.size(); t++) {
			if (name == conseq.Causes[t].get<0>()) { continue; }

			Consequence load(conseq.Causes[t].get<0>().substr(2), Helper::GetConseqType(conseq.Causes[t].get<0>()));
			for (size_t t2 = 0; t2 < load.PerhapsWill.size(); t2++) {

				if (conseq.Causes[t].get<0>() == load.PerhapsWill[t2].get<0>())
				{
					decomps.push_back(conseq.Causes[t].get<0>());
					break;
				}
			}
		}

		return decomps;
	}

	bool LogicalCore::CauseReputation(boost::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, 
		std::string>& Cause, const bool Diagnostic) /* CauseReputation */ {
		//// «десь все вроде бы готово. “естирование прошло более менее стабильно.
		if (Cause.get<0>().size() == 0 && Cause.get<1>().size() == 0 && Cause.get<2>().size() == 0) {
			if(Settings::CauseReputationDebug) Debug::Log("LogicalCore::CauseReputation : Size causes equal zero...", Info);
			return false; }

		if (!Helper::SelectionSort(Cause.get<1>(), Cause.get<0>(), Cause.get<2>())) { // Sorting...
			Settings::IsActive = false; }

		size_t size = Cause.get<0>().size();
		for (size_t i = 0; i < size; i++) {
			Consequence loaded;
			try {
				int last_index = size - 1 - i; // ѕеречисление с конца массива в начало (инверси€). »бо массив отсортирован от меньшего к большему
				AType t = ToAType(Cause.get<0>()[last_index][0]);
				if (t != Undefined) { // Error
					if (!loaded.Load(Cause.get<0>()[last_index].substr(2), t, true, Diagnostic, "Logic"))
						if (Diagnostic) return false;
						else continue; }
				else { Debug::Log("LogicalCore::CauseReputation : Unknown type! \"" + Cause.get<0>()[last_index] + "\"", Warning); continue; }

				///\TODO :
				///	Ќаписать логику того, как насто€щее событие будет вли€ть на наше отношение к прошлому - причинам насто€щего.
				///	“ак же следует добавить учет количества встреч причины в этом следствии.
			}
			catch (...) { Debug::Log("LogicalCore::CauseReputation : An exception has occured! [First Block]", Error); }

			size_t percent = (size_t)((double)size * 0.6f);
			if (size - i > percent) {
				try {
					if (Cause.get<3>() < 0) {
						loaded.Bad -= (Cause.get<3>() / 3) * (size - i);
						loaded.Good -= loaded.Bad * 0.25f;
						// „ем выше веро€тность того, что это следствие €вл€етс€ истинной прчиной, тем сильнее мен€етс€ его репутаци€ 
					}
					else if (Cause.get<3>() > 0) {
						loaded.Good += (Cause.get<3>() / 3) * (size - i);
						loaded.Bad -= loaded.Good * 0.25f; }
					else Debug::Log("LogicalCore::CauseReputation : Ќепредвиденна€ ситуаци€... \n\tHelpfulness = 0 \n\tName conseq : " + Cause.get<4>(), Warning); }
				catch (...) {
					Debug::Log("LogicalCore::CauseReputation : An exception has occured! [Second Block]", Error); }

				try {
					Synapse::FindAndSummSensiv(loaded, Cause.get<4>(), Cause.get<2>()[i], Cause.get<3>());
					loaded.meetings++;

					if (!Diagnostic) loaded.Save(); // Saving
					else Consequence::Save(&loaded, true); // Debug-Save
				}
				catch (...) { Debug::Log("LogicalCore::CauseReputation : An exception has occured! [Third Block]", Error); }
			}
			else break;
		}
		return true;
	}
	bool LogicalCore::GetOpposite(Consequence & opposite, Consequence & ev, bool Diagnostic)
	{
		Helper::SelectionSort(ev.Causes, false);
		bool find = false;

		for (size_t t = 0; t < ev.Causes.size(); t++) {
			std::string cause = ev.Causes[t].get<0>();
			if (cause.substr(2) == ev.name)
				continue;
			//std::cout << cause << std::endl;
			int m_cs = ev.Causes[t].get<1>(); // “о, сколько раз встретилось это событие перед текущим следствием.

			// Ќаверное нужно загружать самую чстую причину в виде следстви€,
			// после сравнивать количество встреч, провер€€ соответствие двух следствий.

			if (opposite.Load(cause.substr(2), Helper::GetConseqType(cause), true, Diagnostic, "Oppos")) {
				//std::cout << ev.name << std::endl;
				std::string name;
				name += ToString(ev.action.type)[0];
				name += "/" + ev.name;

				int index = Helper::IndexOfPerhapsWill(opposite.PerhapsWill, name);
				
				if (index != -1) {

					int m_po = opposite.PerhapsWill[index].get<3>();

					if (double(m_cs) * 0.75f <= double(m_po)) {
						find = true;
					}
				}
			}
			else {
				Debug::Log("GetOpposite : Loading failed! Name : " + cause, Error);
			}
		}

		if (!find) {
			opposite.name = "[MISSING]";
			opposite.action.type = Undefined; }

		return find;
	}
}