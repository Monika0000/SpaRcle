#include "pch.h"
#include "CentralCore.h"
#include "EmotionCore.h"
#include "windows.h"
#include "Debug.h"
#include "Helper.h"
#include <iostream>
#include "Settings.h"

#include <thread> 
#include <future>
#include <chrono>

namespace SpaRcle {
	CentralCore::CentralCore(int cpuSpeed)
	{
		_reality = NULL;
		_causality = NULL;
		_logic = NULL;

		Tree = TaskTree();
		Tree.Branches.push_back(Branch());
		Tree.Branches[0].Tasks.push_back(Task());
		Tree.Branches[0].Tasks[0].result = Consequence("Life", Action());

		DelayCPU = cpuSpeed;
		Debug::Log("-> Creating the central core are successful!");
	}
	CentralCore::~CentralCore()
	{
		if (Process.joinable())
			Process.detach();
		Debug::Log("-> The central core has completed it's work!", Info);
	}

	bool DoFindSynapse(Consequence& event, int _index, RealityCore& real, CentralCore& core) {
		size_t dp = 0;
		Debug::Log("DFS : Find synapse = " + event.Synapses[_index].get<0>(), Module);
		//%¬ыполн€ем %полезное %действие...
		std::string syn = event.Synapses[_index].get<0>();
	Deep:
		if (syn[0] == 'S') {
			Consequence con;
			if (con.Load(syn.substr(2), AType::Speech, false, false)) {
				real.DoAction(con.action);
				real.core->AddSE(con.name, true);

				if (con.Synapses.size() != 0) {
					std::string sens_log; size_t index = 0; double max = 0;

					for (size_t t = 0; t < con.Synapses.size(); t++) {
						size_t idx_2 = 0; double perc = 0; auto& s_au_2 = con.Synapses[t].get<0>();
						for (size_t tt = 0; tt < con.PerhapsWill.size(); tt++)
							if (con.PerhapsWill[tt].get<0>() == s_au_2) {
								/// \see Ќормализуем длину чувствительностей и сравниваем их между собой.
								double per = Synapse::SimilarityPercentage(con.PerhapsWill[tt].get<1>(), core.SE_With_MyActions, true, true);
								if (per > perc) { perc = per; idx_2 = tt; }
							}

						if (t == 0) max = perc;
						else if (perc >= max) { index = t; max = perc; }
					}
					Debug::Log("DFS : " + core.SE_With_MyActions + "\n" + sens_log + "\tResult : " +
						con.Synapses[index].get<0>() + "; Max = " + std::to_string(max), Module);
					if (max > 58) { // 76
						syn = con.Synapses[index].get<0>();
						dp++;
						if (dp > 20) Debug::Log("DFS : Logical loop! See to logs...", Warning);
						else goto Deep;
					}
				}
			}
			else
				Debug::Log("DSF::DoAction : Failed! \n\t  Name : \"" + con.name + "\"\n\t  Type : "
					+ ToString(con.action.type) + "\n\t  Synapse : " + syn, Warning);
		}
		else
			Debug::Log("DFS::DoAction : Unknown type action! \n\tName : \"" + syn + "\" \n\tEvent : " + event.name + "\n\tIndex : " + std::to_string(_index), Error);
	}
	bool DoEventOfSynapse(Consequence& event, std::string& Situation, CentralCore& core) {
		RealityCore& real = (*core._reality);
		if (event.Synapses.size() > 0) {
			Helper::SelectionSort(event.Synapses);
			int index = event.Synapses.size() - 1;
			bool find = false;
			int meets = -1, deepModeIndex = -1;
			/// ƒобавить возможность крайностей - когда мы совершенно не знаем ответа, но нам нужно что-то сказать,
			/// и у нас есть варианты (хоть и не самые подход€щие(но не плохие)) ты мы будем пытатьс€ их выполнить

		Repeat:
			if (event.Synapses[index].get<1>() > 0)
			{
				/// \see »щем самый схожий синапс
				size_t idx = 0; double percent = 0; auto& s_au = event.Synapses[index].get<0>();
				for (size_t t = 0; t < event.PerhapsWill.size(); t++) {
					if (event.PerhapsWill[t].get<0>() == s_au) {
						if (event.PerhapsWill[t].get<2>() < 0) { Debug::Log("DEOS : Bad syanpse! \"" + s_au + "\" = " + event.PerhapsWill[t].get<1>(), Module); continue; }
						/// \see Ќормализуем длину чувствительностей и сравниваем их между собой.
						double per = Synapse::SimilarityPercentage(event.PerhapsWill[t].get<1>(), core.SE_With_MyActions, true, true);
						if (per > percent) { percent = per; idx = t; }
					}
				}

				if (percent != 0) {
					Debug::Log("DEOS : Similarity percentage situation = " + std::to_string(percent) + " \"" + s_au + "\"", Module);
					if(percent == 100){
						Debug::Log("DEOS : Super variant = " + event.PerhapsWill[idx].get<0>() + "; Meets = " + std::to_string(event.PerhapsWill[idx].get<3>()), Module);
						find = true; 
						if (event.PerhapsWill[idx].get<3>() > meets) {
							meets = event.PerhapsWill[idx].get<3>();
							deepModeIndex = index; }
						if (index > 0) { index--; goto Repeat; }
						else 
							DoFindSynapse(event, deepModeIndex, real, core);
					}
					else if (percent >= PeriodicSix + 10) { /// \bug +10
						DoFindSynapse(event, index, real, core);
						find = true;
						if (!find && index > 0) { index--; /*Debug::Log(index);*/ goto Repeat; } // ≈сли не вышло
					}
					else if (index > 0) { index--; goto Repeat; }/// $≈сли $не $подходит
				}
				else { ///\to NOT FOUND SOLUTION
					Debug::Log("DEOS : Not found solution = \"" + event.name + "\" at synapse \"" + s_au + "\" index="+std::to_string(index), Module);
					if (index > 0) { index--; goto Repeat; }
				}
			}
			return find;
		}
		else
			return false;
	}

	void SpaRcle::CentralCore::ProcessingEvent(Consequence& event, std::string Situation, CentralCore& core) {
		// TODO : “ребуетс€ калибровка порогов чувствительности.
		if (event.Bad > event.Good && event.Bad > 3) { // ѕытаемс€ выкрутитьс€ из ситуации с наименьшим ущербом
			Debug::Log("CentralCore : " + event.name + " is Bad"); /* Debuging */
			bool find = DoEventOfSynapse(event, Situation, core); // ≈сли имеетс€ хоть какое-то решение

			if (!find) // ≈сли все совсем печально...
			{
				Debug::Log("CentralCore : я не знаю, что мне делать!", Mind);
				// ј..Ё.. ј что мне делать?
				// TODO : current work!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// ѕытаемс€ придумать решение...

				Consequence opposite; bool seccues = false;
				if (LogicalCore::GetOpposite(opposite, event))
				{
					std::string ev_name = event.name;
					std::string op_name = opposite.name;

					Debug::Log("CentralCore::GetOpposite : " + ev_name + " => " + op_name);
					seccues = true;
				}

				if (!seccues) {
					Debug::Log("CentralCore : ¬се совсем плохо!", Mind);
				}
				else {
					Debug::Log("Opposite : " + opposite.name, Mind);
				}

				/*  ƒл€ начала ищем противоположности этого следстви€.
					≈сли не выходит, идем ва-банк и пробуем повторить плохое действие, с некоторыми изменени€ми...
					ћожет помочь, если нас бьют и мы не понимаем за что (просто издеваютс€), и нам нужно как-то спастись.
					¬ какой-то мемент, мы понимаем, что можем и тоже бить, но это плохо, однако, мы в безысходном положении
					и у нас отча€ние, потому, нам ничего не остаетс€, как бить в ответ, то-есть, повторить плохое действие.
				*/
			}
		}
		// ≈сли плохо 
		else if (event.Good > event.Bad && event.Good > 5) {
			Debug::Log("CentralCore : " + event.name + " is Good"); /* Debuging */
			// (OLD) ќ даааа с*ка как хорошо-то! (наслаждаемс€ и возможно еще раз повтор€ем)
			DoEventOfSynapse(event, Situation, core);
		}
		// ≈сли хорошо
		else
		{
			Debug::Log("CentralCore : \"" + event.name + "\" is undefined. \n\t" + "Good : " + std::to_string(event.Good) + "\n\tBad : " + std::to_string(event.Bad));
			// ѕовтор€ем дл€ того, чтобы пон€ть : плохо это или хорошо.
			(*core._reality).DoAction(event.action); // Trying to repeat
		}
		// ≈сли не пон€ли

		//core.Events.erase(core.Events.begin());
	}

	void CentralSolution(int* DelayCPU, CentralCore* _core)
	{
		Sleep(100);
		CentralCore& core = *_core;
		RealityCore& real = *core._reality;
		CausalityCore& causal = *core._causality;
		size_t size_ev = 0, deep = 0;

		while (Settings::IsActive) {
			if(core.Events.size() == 0) Sleep(*DelayCPU);

			try {
				core._logic->DoIt(core.Tree.Branches[core.Tree.Branches.size() - 1].Tasks[
					core.Tree.Branches[core.Tree.Branches.size() - 1].Tasks.size() - 1]);

				if (causal.UncheckedEvents.size() == 0) {
					if (core.Events.size() > 1) {
					Ret:
						Consequence& conseq = core.Events[deep].get<0>();
						(*_core).AddSE(conseq.name, false); // ѕетл€

						if (conseq.name == Settings::EmptyName) {
							if (core.Events.size() > deep + 1) {
								deep++; goto Ret; }
							else {
								for (size_t t = deep; t > 0; t--)
									core.Events.erase(core.Events.end() - t);
								core.Events.erase(core.Events.begin());
								deep = 0; }
						}
						else {
							std::string sit = core.Events[deep].get<1>();

							if (core.Events.size() > deep + 1) {
								if (conseq.EventData.sec + 5 >= core.Events[deep + 1].get<0>().EventData.sec) {
									deep++; goto Ret; }
								else {
									Debug::Log("CentralCore : While - Logical loop. \n\tEvents : "
										+ std::to_string(core.Events.size()) + "\n\tDeep : " + std::to_string(deep) + "\n\tClean events list...", Warning);

									if (core.Events.size() != 0) {
										for (size_t t = deep; t > 0; t--)
											core.Events.erase(core.Events.end() - t);
										core.Events.erase(core.Events.begin()); }
									deep = 0;
								}
							}
							else {
								Debug::Log("CentralCore : multi (" + sit + "): " + conseq.name);
								CentralCore::ProcessingEvent(conseq, core.Events[deep].get<1>(), core);
								if (core.Events.size() != 0) {
									for (size_t t = deep; t > 0; t--)
										core.Events.erase(core.Events.end() - t);
									core.Events.erase(core.Events.begin()); }
								deep = 0;
							}
						}
					}
					else if (core.Events.size() == 1) {
						Consequence& conseq = core.Events[0].get<0>();
						if (conseq.name == Settings::EmptyName) {
							(*_core).AddSE(conseq.name, false);
						}
						else {
							Debug::Log("CentralCore : only (" + core.Events[0].get<1>() + "): " + conseq.name);
							CentralCore::ProcessingEvent(conseq, core.Events[deep].get<1>(), core); }
						core.Events.erase(core.Events.begin());
					}
				}
			}
			catch (...) {
				Debug::Log("CentralCore : An exception has occured! \n\tEvents : " + std::to_string(core.Events.size()) +
					"\n\tDeep : " + std::to_string(deep), Error);
				break;
			}
			if (Settings::CoreDebug)
				Debug::Log("Processing core...");
		}
	}

	void CentralCore::AddSE(std::string event_name, bool IDoIt)
	{
		if (Settings::CentralCoreDebug)
			if (IDoIt)
				Debug::Log("CentralCore::AddSE : " + event_name + " ~ " + this->SE_With_MyActions + " (SELF)");
			else
				Debug::Log("CentralCore::AddSE : " + event_name + " ~ " + this->SE_With_MyActions);
		//Debug::Log(this->SE_With_MyActions.size());
		if (event_name == Settings::EmptyName) {
			if (this->SE_With_MyActions.size() > 0)
				this->SE_With_MyActions.erase(this->SE_With_MyActions.begin(), this->SE_With_MyActions.begin() + count_word_in_sensiv);
		}
		else {
			if (this->SE_With_MyActions.size() >= SpaRcle::max_size_SEWMA * SpaRcle::count_word_in_sensiv)
				for (short t = 0; t < SpaRcle::count_word_in_sensiv; t++)
					this->SE_With_MyActions.erase(this->SE_With_MyActions.begin() + t);

			if (IDoIt) for (short i = 0; i < event_name.size(); i++) event_name[i] = toupper(event_name[i]);
			/// \see %ќпредел€ем %принадлежность %этого %событи€ %к %нам (!сделали !его !мы !или !нет)

			this->SE_With_MyActions += Synapse::GetSensivityOfName(event_name);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			//this->_causality->Current_sensivity.erase(this->_causality->Current_sensivity.begin(), this->_causality->Current_sensivity.begin() + count_word_in_sensiv);
			//this->_causality->Current_sensivity += Synapse::GetSensivityOfName(event_name);

			//this->_causality->Sensivity_List.erase(this->_causality->Sensivity_List.begin());
			//this->_causality->Sensivity_List.push_back(this->_causality->Current_sensivity);
		}
	}
	void CentralCore::NewEvent(Consequence event, std::string Situation)
	{
		Events.push_back(boost::tuple<Consequence, std::string>(event, Situation));
		event.~Consequence();
		Situation.~basic_string();
	}
	void CentralCore::Start()
	{
		if (!Settings::PathsIsSet) {
			Debug::Log("CentralCore::Start = WARNING : Paths is not set! Setting to default...");
			TCHAR cwd[100];
			GetCurrentDirectory(100, cwd);
			std::wstring ws(&cwd[0]); // WARNINGS!!!!!
			Settings::SetDefaultPaths(std::string(ws.begin(), ws.end()));
		}
		//Process = std::thread(std::move(task));
		Process = std::thread(CentralSolution, &DelayCPU, this);
		//std::packaged_task<void(int &)> task{ CentralSolution };
		//Process = std::thread { std::move(CentralSolution), std::ref(DelayCPU) };
		Debug::Log("-> The central core is started!");
	}

	void CentralCore::ConnectReality(RealityCore* core) { _reality = core; _reality->core = this; }
	void CentralCore::ConnectCausality(CausalityCore* core) { _causality = core; _causality->core = this; }
	void CentralCore::ConnectLogic(LogicalCore* core) { _logic = core; _logic->core = this; }
	void CentralCore::ConnectEmotion(EmotionCore* core) { _emotion = core; _emotion->core = this; }
}
