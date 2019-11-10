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
	LogicalCore::LogicalCore(int cpuSpeed)
	{
		core = NULL;
		CoreLoad = 0;
		DelayCPU = cpuSpeed;

		Tasks.resize(0);

		Debug::Log("-> Creating the logical core are successful!");
	}
	LogicalCore::~LogicalCore() {
		if (Process.joinable())
			Process.detach();

		this->Causes.clear();

		Debug::Log("-> The logical core has completed it's work!", Info);
	}
	 
	void LogicalSolution(int* delay, LogicalCore* _core) {
		CentralCore& core = *(*_core).core;
		RealityCore& real = *core._reality;
		CausalityCore& causal = *core._causality;
		char timer = 0, load = 0;

		while (Settings::IsActive)
		{
			if (timer >= 100) {
				timer = 0; _core->CoreLoad = load; load = 0;
			}
			else timer++;

			try {
				if (causal.CheckedEvents.size() <= Settings::Size_SCP * 2 + 1 && causal.UncheckedEvents.size() == 0 && core.Events.size() == 0) {
					if ((*_core).Causes.size() > 0) {
						load++;
						LogicalCore::CauseReputation((*_core).Causes[0]); // Get first element  //ERROR
						(*_core).Causes.erase((*_core).Causes.begin()); // ”дал€ем элемент в самом начале
					}
				}
				else Sleep(*delay);
			}
			catch (...) {
				Debug::Log("LogicalSolution : An exception has ocurred!", Error);
				break;
			}
			if (Settings::CoreDebug)
				Debug::Log("Processing logical... ");
		}
	}
	void LogicalCore::Start()
	{
		Process = std::thread(LogicalSolution, &DelayCPU, this);
		//union d { int f = 5; };
		Debug::Log("-> The logical core is started!");
	}

	void LogicalCore::DoIt(Task task)
	{
		bool has = false;
		if(Tasks.size() != 0)
			for (Task t : Tasks)
				if (t.result.name == task.result.name)
					has = true;

		if (!has)
		{
			if (Tasks.size() == maxTasks) {
				Tasks.erase(Tasks.begin()); // first element
				Tasks.push_back(task); // add element to end
				Debug::Log("____" + Tasks[0].result.name + Tasks[1].result.name + Tasks[2].result.name + Tasks[3].result.name + 
					Tasks[4].result.name + Tasks[5].result.name + Tasks[6].result.name + Tasks[7].result.name);
			}
			else
				Tasks.push_back(task); // add element to end

			Debug::Log("LogicalCore.DoIt = New task : \"" + task.result.name + "\"", Info);

			//core->_reality->DoAction(Action(Sound("I'am can speek!"))); // [TEST CODE]
		}
	}
	void LogicalCore::EditCauses(std::vector<std::string>& Causes, std::vector<int>& Meets, std::vector<std::string> Sensivitys, Consequence& conq)
	{
		std::string name;
		name += ToString(conq.action.type)[0];
		name += "/"; 
		name += conq.name;
		this->Causes.push_back(boost::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, std::string>
			(Causes, Meets, Sensivitys, conq.GetSummHP(), name)); // ƒобавл€ем элемент в конец
	}
	std::vector<std::string> LogicalCore::DecomposeConsequence(Consequence& conseq)
	{
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
			if (name == conseq.Causes[t].get<0>()) {
				continue;
			}

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
			Debug::Log("LogicalCore::CauseReputation : Size causes equal zero...", Info);
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

				/*
				if (Cause.get<0>()[last_index][0] == 'S') { // Error
					if (!loaded.Load(Cause.get<0>()[last_index].substr(2), AType::Speech, true, Diagnostic))
						if (Diagnostic) return false;
						else continue; }
				else {
					Debug::Log("LogicalCore::CauseReputation : Unknown type! \"" + Cause.get<0>()[last_index] + "\"", Warning);
					continue; }
				*/

				// TODO :
				//	Ќаписать логику того, как насто€щее событие будет вли€ть на наше отношение к прошлому - причинам насто€щего.
				//	“ак же следует добавить учет количества встреч причины в этом следствии.
				//
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
					/*
					int index = Helper::IndexOfSynapse(loaded.PerhapsWill, Cause.get<4>());
					if (index == -1) {
						std::string& _1 = Cause.get<4>();
						double& _3 = Cause.get<3>();
						std::string& _2 = Cause.get<2>()[i];
						//int& _4 = Cause.get<3>();
						//_2.substr(2);// не то 
						//std::cout << _1 << std::endl;
						//std::cout << _1 << std::endl;
						//std::cout << _3 << std::endl;
						//std::cout << "sensiv : " << _2 << std::endl;
						loaded.PerhapsWill.push_back(boost::tuple<std::string, std::string, double, int>(_1, _2, _3, 1)); // INFO : передаетс€ одна встреча
						// TODO: „астые ошибки
					}
					else {
						//std::cout << "added sensiv" << std::endl;
						//loaded.PerhapsWill[index].get<1>() = Cause.get<2>()[i] + "_ADDED"; // TODO: Ќужно прописать сложение чувствительности!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						//loaded.PerhapsWill[index].get<1>() = Helper::SummSensivity(Cause.get<2>()[i], loaded.PerhapsWill[index].get<1>()); // TODO: Ќужно прописать сложение чувствительности!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						//Helper::SummSensivity(loaded.PerhapsWill[index].get<1>(),Cause.get<2>()[i]);
						Helper::SummSensivity(loaded, index, Cause.get<2>()[i], ECom::PerhWill);
						loaded.PerhapsWill[index].get<2>() = (loaded.PerhapsWill[index].get<2>() + Cause.get<3>()) / 2;
						loaded.PerhapsWill[index].get<3>()++; // Increment
					}
					*/
					//boost::tuple<std::string, std::string, double> tp(, Cause.get<2>()[i], );
					Synapse::FindAndSummSensiv(loaded, Cause.get<4>(), Cause.get<2>()[i], Cause.get<3>());

					loaded.meetings++;

					if (!Diagnostic) // Saving
						loaded.Save();
					else // Debug-Save
						Consequence::Save(&loaded, true);
				}
				catch (...) {
					Debug::Log("LogicalCore::CauseReputation : An exception has occured! [Third Block]", Error); }
			}
			else break;
		}
		return true;
	}
	bool LogicalCore::GetOpposite(Consequence & opposite, Consequence & event, bool Diagnostic)
	{
		// ј вот и самый сложный участок кода. «десь мы должны найти противоположность следствию. ”дачи...

		Helper::SelectionSort(event.Causes, false);
		bool find = false;

		for (size_t t = 0; t < event.Causes.size(); t++) {
			std::string cause = event.Causes[t].get<0>();
			if (cause.substr(2) == event.name)
				continue;
			//std::cout << cause << std::endl;
			int m_cs = event.Causes[t].get<1>(); // “о, сколько раз встретилось это событие перед текущим следствием.

			// Ќаверное нужно загружать самую чстую причину в виде следстви€,
			// после сравнивать количество встреч, провер€€ соответствие двух следствий.

			//Consequence PO; // Possible opposite
			//std::cout << Diagnostic << std::endl;
			//opposite.Load(Diagnostic, false);

			//continue;
			if (opposite.Load(cause.substr(2), Helper::GetConseqType(cause), true, Diagnostic, "Oppos")) {
				//std::cout << event.name << std::endl;
				std::string name;
				name += ToString(event.action.type)[0];
				name += "/" + event.name;

				int index = Helper::IndexOfPerhapsWill(opposite.PerhapsWill, name);
				
				if (index != -1) {

					int m_po = opposite.PerhapsWill[index].get<3>();

#if 0
					std::cout << m_po << " " << m_cs << std::endl;
#endif // 0

					if (double(m_cs) * 0.75f <= double(m_po)) {
						///if ()
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