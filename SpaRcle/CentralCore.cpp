#include "pch.h"
#include "CentralCore.h"
#include "EmotionCore.h"
#include "windows.h"
#include "Debug.h"
#include "Helper.h"
#include <iostream>
#include "Settings.h"
#include "Memory.h"

#include <thread> 
#include <future>
#include <chrono>

namespace SpaRcle {
	CentralCore::CentralCore(int cpuSpeed) {
		this->CoreLoad = 0;
		_reality = NULL;
		_causality = NULL;
		_logic = NULL;

		Memory::GetMemory()->InitMemory(this);

		Tree = TaskTree();
		Tree.Branches.push_back(Branch());
		Tree.Branches[0].Tasks.push_back(Task());
		Tree.Branches[0].Tasks[0].result = Consequence("Life", Action(Sound()));

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
		Debug::Log("DFS : Find synapse = " + event.GetSN_Name(_index), Module);
		//%¬ыполн€ем %полезное %действие...
		std::string syn = event.GetSN_Name(_index);
	Deep:
		if (syn[0] == 'S') {
			Consequence con;
			if (con.Load(syn.substr(2), AType::Speech, false, false, "DFS") == 1) {
				real.DoAction(con.action, con.name);
				real.core->AddSE(con.name, true);

				if (con.Synapses.size() != 0) {
					std::string sens_log; size_t index = 0; double max = 0; 
					std::vector<short> super_indx; bool super = false; //float max = 0;

					for (size_t t = 0; t < con.Synapses.size(); t++) {
						size_t idx_2 = 0; //double perc = 0; 
						auto& s_au_2 = con.GetSN_Name(t);
						for (size_t tt = 0; tt < con.PerhapsWill.size(); tt++)
							if (con.GetPW_Name(tt) == s_au_2) {
								/// \see Ќормализуем длину чувствительностей и сравниваем их между собой.
								double per = Synapse::SimilarityPercentage(con.GetPW_Sens(tt), core.SE_With_MyActions, true, true);
								if (per > max) { 
									if (super) { super_indx.clear(); super = false; }
									max = per; idx_2 = tt;
									index = idx_2;
									Debug::Log("DFS : Synapse ["+con.GetSN_Name(t)+"] = "+std::to_string(per) + 
										" {"+ con.GetPW_Sens(tt) +"}", Module);
								}
								else if(per == max) {
									Debug::Log("DFS : super variant = "+ con.GetPW_Name(tt) + " ["+ std::to_string(per) +"] {"+ con.GetPW_Sens(tt) +"}", Module);
									super = true;
								}
							}
					}

					if (super) {
						short ind = 0; float meets = 0;
						for (short i = 0; i < (short)super_indx.size(); i++) {
							if (con.GetPW_Meet(super_indx[i]) > meets) {
								meets = con.GetPW_Meet(super_indx[i]);
								ind = i;
							}
						}
						Debug::Log("DFS finaly : \n\tSens : " + core.SE_With_MyActions + "\n" + sens_log + "\tSuper result : " +
							con.GetPW_Name(ind) + "; Max = " + std::to_string(max), Module);
						//TODODODODODODODODODODODODODOODODOODODODODOODODOODOODOOD
						if (max > Settings::MinSimilarityPerc) { // 76 // 58
							syn = con.GetPW_Name(ind);
							dp++;
							if (dp > 20) Debug::Log("DFS : Logical loop! See to logs...", Warning);
							else goto Deep;
						}
					}
					else {
						Debug::Log("DFS finaly : \n\tSens : " + core.SE_With_MyActions + "\n" + sens_log + "\tResult : " +
							//con.Synapses[index].get<0>() + "; Max = " + std::to_string(max), Module);
							con.GetPW_Name(index) + "; Max = " + std::to_string(max), Module);

						if (max > Settings::MinSimilarityPerc) { // 76 // 58
							syn = con.GetPW_Name(index);
							dp++;
							if (dp > 20) Debug::Log("DFS : Logical loop! See to logs...", Warning);
							else goto Deep;
						}
					}
				}
				else
					Debug::Log("DFS : event \"" + con.name + "\" are logic end.", Module);
			}
			else
				Debug::Log("DSF::DoAction : Failed! \n\t  Name : \"" + con.name + "\"\n\t  Type : "
					+ ToString(con.action.type) + "\n\t  Synapse : " + syn, Warning);
		}
		else
			Debug::Log("DFS::DoAction : Unknown type action! \n\tName : \"" + syn + "\" \n\tEvent : " + event.name + "\n\tIndex : " + std::to_string(_index), Error);
		return false;
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
			if (event.GetSN_HP(index) > 0)	{
				/// \see »щем самый схожий синапс
				size_t idx = 0; double percent = 0; auto& s_au = event.GetSN_Name(index);
				for (size_t t = 0; t < event.PerhapsWill.size(); t++) {
					if (event.GetPW_Name(t) == s_au) {
						if (event.GetPW_HP(t) < 0) { 
							///\!Debug::Log("DEOS : Bad synapse! \"" + s_au + "\" = " + event.GetPW_Sens(t), Module); 
							continue; 
						}
						/// \see Ќормализуем длину чувствительностей и сравниваем их между собой.
						double per = Synapse::SimilarityPercentage(event.GetPW_Sens(t), core.SE_With_MyActions, true, true);
						if (per > percent) { percent = per; idx = t; }
					}
				}

				if (percent != 0) {
					Debug::Log("DEOS : Similarity percentage situation = " + std::to_string(percent) + " \"" + s_au + "\"", Module);
					if (percent == 100) {
						Debug::Log("DEOS : Super variant = " + event.GetPW_Name(idx) + "; Meets = " + std::to_string(event.GetPW_Meet(idx)), Module);
						find = true;
						if (event.GetPW_Meet(idx) > meets) {
							meets = event.GetPW_Meet(idx);
							deepModeIndex = index;
						}
						if (index > 0) { index--; goto Repeat; }
						else DoFindSynapse(event, deepModeIndex, real, core);
					}
					else if (percent >= PeriodicSix + 10) { /// \bug +10
						DoFindSynapse(event, index, real, core);
						find = true;
						if (!find && index > 0) { index--; /*Debug::Log(index);*/ goto Repeat; } // ≈сли не вышло
					}
					else if (index > 0) { index--; goto Repeat; }/// $≈сли $не $подходит
				}
				else { ///\to NOT FOUND SOLUTION
					Debug::Log("DEOS : Not found solution = \"" + event.name + "\" at synapse \"" + s_au + "\" index=" + std::to_string(index), Module);
					if (index > 0) { index--; goto Repeat; }
				}
			}

			return find;
		}
		else return false;
	}

	void SpaRcle::CentralCore::ProcessingEvent(Consequence& event, std::string& Situation, CentralCore& core) {
		// TODO : “ребуетс€ калибровка порогов чувствительности.
		if (event.Bad > event.Good && event.Bad > 3) { // ѕытаемс€ выкрутитьс€ из ситуации с наименьшим ущербом
			Debug::Log("CentralCore : " + event.name + " is Bad! \n\tBad : " + std::to_string(event.Bad) + "\n\tGood : " + std::to_string(event.Good)); 
			bool find = DoEventOfSynapse(event, Situation, core); // ≈сли имеетс€ хоть какое-то решение

			if (!find) // ≈сли все совсем печально...
			{
				Debug::Log("CentralCore : Find opposite...", Mind);

				Consequence opposite; bool seccues = false;
				if (LogicalCore::GetOpposite(opposite, event)) {
					std::string ev_name = event.name;
					std::string op_name = opposite.name;

					Debug::Log("CentralCore::GetOpposite : " + ev_name + " => " + op_name);
					seccues = true;
				}

				if (!seccues) {
					Debug::Log("CentralCore : opposite is not found!", Mind);

				}
				else {
					Debug::Log("Opposite : " + opposite.name, Mind);
					//TODO

				}

				opposite.~Consequence();

				/*  ƒл€ начала ищем противоположности этого следстви€.
					≈сли не выходит, идем ва-банк и пробуем повторить плохое действие, с некоторыми изменени€ми...
					ћожет помочь, если нас бьют и мы не понимаем за что (просто издеваютс€), и нам нужно как-то спастись.
					¬ какой-то мемент, мы понимаем, что можем и тоже бить, но это плохо, однако, мы в безысходном положении
					и у нас отча€ние, потому, нам ничего не остаетс€, как бить в ответ, то-есть, повторить плохое действие.
				*/
			}
			else {
				Debug::Log("ProcessingEvent : solution is found.", Info);
				return; /* Ќичего не делаем, тк в DEOS уже все выполнилось*/ }
		}
		// ≈сли плохо 
		else if (event.Good > event.Bad && event.Good > 5) {
			Debug::Log("CentralCore : " + event.name + " is Good"); /* Debuging */
			DoEventOfSynapse(event, Situation, core); // »щем продолжение
		}
		// ≈сли хорошо
		else
		{
			Debug::Log("CentralCore : \"" + event.name + "\" is undefined. \n\t" + "Good : " + std::to_string(event.Good) + "\n\tBad : " + std::to_string(event.Bad));
			// ѕовтор€ем дл€ того, чтобы пон€ть : плохо это или хорошо.
			(*core._reality).DoAction(event.action, event.name); // Trying to repeat
		}
		// ≈сли не пон€ли
	}

	void CentralSolution(int* DelayCPU, CentralCore* _core) {
	Restarting:
		Sleep(100);
		CentralCore& core = *_core;
		RealityCore& real = *core._reality;
		CausalityCore& causal = *core._causality;
		size_t size_ev = 0, deep = 0;
		char timer = 0; short load = 0;

		Debug::Log("-> The central core is started!");

		while (Settings::IsActive) {
			if (timer >= 100) { timer = 0; _core->CoreLoad = load; load = 0; }
			else timer++;

			core.size_events = core.Events_conq.size();

			if (core.size_events == 0) Sleep(*DelayCPU);
			else try {
				//core._logic->DoIt(core.Tree.Branches[core.Tree.Branches.size() - 1].Tasks[
				//	core.Tree.Branches[core.Tree.Branches.size() - 1].Tasks.size() - 1]);

				if (causal.size_unchk_ev == 0) {
					//if (core.Events.size() > 1) {
						load++;
					Ret:
						//Consequence& conseq = core.Events[deep].get<0>();
						Consequence& conseq = core.Events_conq[deep];
						(*_core).AddSE(conseq.name, false); // ѕетл€

						if (conseq.name == Settings::EmptyName) {
							if (core.Events_sens.size() > deep + 1) {
								deep++; goto Ret;
							}
							else {
								core.Events_sens.erase(core.Events_sens.end() - deep, core.Events_sens.end());
								core.Events_conq.erase(core.Events_conq.end() - deep, core.Events_conq.end());
								core.size_events -= deep;

								if (core.size_events > 0 && core.Events_sens.size() > 0) {
									core.Events_sens.erase(core.Events_sens.begin());
									core.Events_conq.erase(core.Events_conq.begin()); }

								deep = 0;
							}
						}
						else if (conseq.name.empty()) {
							Debug::Log("CentralCore : [1] Unknown ERROR => conseq.name == \"\"!", Error); Sleep(1000);
							core.Events_sens.erase(core.Events_sens.begin() + deep);
							
							if(deep < core.Events_conq.size()) core.Events_conq.erase(core.Events_conq.begin() + deep);
							else Debug::Log("CentralCore : [1] Fatal ERORR!\n\tdeep = "+std::to_string(deep) + "\n\tsize = "+std::to_string(core.Events_conq.size()), Error); Sleep(1000);
							
							deep = 0;
						}
						else {
							//std::string sit = core.Events[deep].get<1>();
							std::string& sit = core.Events_sens[deep];

							if (core.Events_sens.size() > deep + 1) {
								//if (conseq.EventData.sec + 5 >= core.Events[deep + 1].get<0>().EventData.sec) {
								if (conseq.EventData.sec + 5 >= core.Events_conq[deep + 1].EventData.sec) {
									deep++; goto Ret;
								}
								else {
									Debug::Log("CentralCore : While - Logical loop. \n\tEvents : "
										//+ std::to_string(core.Events.size()) + "\n\tDeep : " + std::to_string(deep) + "\n\tClean events list...", Warning);
										+ std::to_string(core.Events_sens.size()) + "\n\tDeep : " + std::to_string(deep) + "\n\tClean events list...", Warning);

									if (core.Events_sens.size() != 0) {
										core.Events_sens.erase(core.Events_sens.end() - deep, core.Events_sens.end());
										core.Events_conq.erase(core.Events_conq.end() - deep, core.Events_conq.end());

										if (core.Events_conq.size() != 0) {
											core.Events_sens.erase(core.Events_sens.begin());
											core.Events_conq.erase(core.Events_conq.begin()); }
									}
									deep = 0;
								}
							}
							else {
								Debug::Log("CentralCore : multi (" + sit + "): " + conseq.name);
								//CentralCore::ProcessingEvent(conseq, core.Events[deep].get<1>(), core);
								CentralCore::ProcessingEvent(conseq, core.Events_sens[deep], core);
								if (core.Events_sens.size() != 0) {
									//for (size_t t = deep; t > 0; t--)
									//	core.Events.erase(core.Events.end() - t);
									core.Events_sens.erase(core.Events_sens.end() - deep, core.Events_sens.end());
									core.Events_conq.erase(core.Events_conq.end() - deep, core.Events_conq.end());

									if (core.Events_conq.size() != 0) {
										core.Events_sens.erase(core.Events_sens.begin());
										core.Events_conq.erase(core.Events_conq.begin()); }
								}
								deep = 0;
							}
						}
					}
				else if (core.Events_sens.size() == 1) {
					load++;
					Consequence& conseq = core.Events_conq[0];
					if (conseq.name == Settings::EmptyName)
						(*_core).AddSE(conseq.name, false);
					else if (conseq.name.empty()) {
						Debug::Log("CentralCore : [2] Unknown ERROR => conseq.name == \"\"!", Error); Sleep(1000); }
					else {
						Debug::Log("CentralCore : only (" + core.Events_sens[0]+ "): " + conseq.name);
						//CentralCore::ProcessingEvent(conseq, core.Events_sens[deep], core);
					}
					core.Events_sens.erase(core.Events_sens.begin());
					core.Events_conq.erase(core.Events_conq.begin());
					//}
				}
			}
			catch (...) {
				Debug::Log("CentralCore : An exception has occured! \n\tEvents : " + std::to_string(core.Events_sens.size()) +
					"\n\tDeep : " + std::to_string(deep), Error);
				Sleep(500);
				Debug::Log("-> The central core is stopped!");
				Sleep(1000);
				Debug::Log("CentralCore : Restarting...", System);
				goto Restarting;
			}
			if (Settings::CoreDebug) Debug::Log("Processing core...");
		}
	}

	void CentralCore::AddSE(std::string event_name, bool IDoIt) {
		if (Settings::CentralCoreDebug)
			if (IDoIt) Debug::Log("CentralCore::AddSE : " + event_name + " ~ " + this->SE_With_MyActions + " (SELF)");
			else Debug::Log("CentralCore::AddSE : " + event_name + " ~ " + this->SE_With_MyActions);
		
		if (event_name == Settings::EmptyName) {
			if (this->SE_With_MyActions.size() > 0)
				this->SE_With_MyActions.erase(this->SE_With_MyActions.begin(), this->SE_With_MyActions.begin() + count_word_in_sensiv);
		}
		else {
			if (this->SE_With_MyActions.size() >= SpaRcle::max_size_SEWMA * SpaRcle::count_word_in_sensiv)
				for (short t = 0; t < SpaRcle::count_word_in_sensiv; t++)
					this->SE_With_MyActions.erase(this->SE_With_MyActions.begin() + t);

			//if (IDoIt) for (short i = 0; i < event_name.size(); i++) event_name[i] = toupper(event_name[i]);
			/// \see %ќпредел€ем %принадлежность %этого %событи€ %к %нам (!сделали !его !мы !или !нет)

			this->SE_With_MyActions += Synapse::GetSensivityOfName(event_name, IDoIt);
		}
	}
	void CentralCore::NewEvent(Consequence& event, std::string& Situation)
	{
		//Events.push_back(boost::tuple<Consequence, std::string>(event, Situation));
		Events_conq.push_back(event);
		Events_sens.push_back(Situation);
		//event.~Consequence();
		//Situation.~basic_string();
	}
	void CentralCore::Start() {
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
	}

	void CentralCore::ConnectReality(RealityCore* core) { _reality = core; _reality->core = this; }
	void CentralCore::ConnectCausality(CausalityCore* core) { _causality = core; _causality->core = this; }
	void CentralCore::ConnectLogic(LogicalCore* core) { _logic = core; _logic->core = this; }
	void CentralCore::ConnectEmotion(EmotionCore* core) { _emotion = core; _emotion->core = this; }
}
