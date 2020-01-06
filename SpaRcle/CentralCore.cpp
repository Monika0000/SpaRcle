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
		//%Выполняем %полезное %действие...
		std::string syn = event.GetSN_Name(_index);
	Deep:
		if (syn[0] == 'S' || syn[0] == 'M') {
			Consequence con;
			if (con.Load(syn.substr(2), ToAType(syn[0]), true, false, "DFS") == 1) {
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
								/// \see Нормализуем длину чувствительностей и сравниваем их между собой.
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
						short ind = 0; int meets = 0;
						for (short i = 0; i < (short)super_indx.size(); i++) {
							if (con.GetPW_Meet(super_indx[i]) > meets) {
								meets = con.GetPW_Meet(super_indx[i]);
								ind = i;
							}
						}
						Debug::Log("DFS finaly (super) : \n\tSens : " + core.SE_With_MyActions + "\n" + sens_log + "\tSuper result : " +
							con.GetPW_Name(ind) + "; Max = " + std::to_string(max), Module);
						///\TODO !!!!!!!
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
			con.~Consequence();
		}
		else
			Debug::Log("DFS::DoAction : Unknown type action! \n\tName : \"" + syn + "\" \n\tEvent : " + event.name + "\n\tIndex : " + std::to_string(_index), Error);
		return false;
	}
	bool DoEventOfSynapse(Consequence& _event, std::string& Situation, CentralCore& core) {
		RealityCore& real = (*core._reality);
		//std::vector<short> super_variants; 
		size_t mem_sup_index = 0;

		if (_event.Synapses.size() > 0) {
			Helper::SelectionSort(_event.Synapses);
			int index = _event.Synapses.size() - 1;
			bool find = false, super = false; 
			int meets = -1, deepModeIndex = -1;
			/// Добавить возможность крайностей - когда мы совершенно не знаем ответа, но нам нужно что-то сказать,
			/// и у нас есть варианты (хоть и не самые подходящие(но не плохие)) ты мы будем пытаться их выполнить
		Repeat:
			if (_event.GetSN_HP(index) > 0)	{ // Полезность синапса внутри _event

				/// ^see ^IIIIIIIIIIIIIIIII Ищем самый схожий синапс
				double percent = 0; // Итоговый процент схожести ситуации на одну из возможных вариаций
				size_t idx = 0;		// Индекс найденой вариации
				auto& s_au = _event.GetSN_Name(index); // Имя синапса под индексом перечисления
				for (size_t t = 0; t < _event.PerhapsWill.size(); t++) {
					if (_event.GetPW_Name(t) == s_au) {
						if (_event.GetPW_HP(t) < 0) { 
							///\!Debug::Log("DEOS : Bad synapse! \"" + s_au + "\" = " + event.GetPW_Sens(t), Module); 
							continue; 
						}
						/// \see Нормализуем длину чувствительностей и сравниваем их между собой.
						double per = Synapse::SimilarityPercentage(_event.GetPW_Sens(t), core.SE_With_MyActions, true, true);
						if (per > percent)  {
							percent = per; 
							idx = t; 
						}
					}
				}
				if (percent == 100) super = true;
				Debug::Log("DEOS : Similarity percentage situation = " + std::to_string(percent) + " \"" + s_au + "\"", Module);//(" + core.SE_With_MyActions + " " + + ")", Module);
				/// ^see ^IIIIIIIIIIIIIIIII Ищем самый схожий синапс

				if (percent != 0) {
					if (super) {
						if (percent == 100) { 
							// Если у нас однажды был найден супер вариант, то мы выполняем проверку,
							// на принадлежность последнего найденного, к супер вариации.
							// Если принадлежит - сравниваем с последним на число встреч
							// Если не принадлежит - прверяем есть ли еще варианты, 
							// Если есть - продолжаем искать супер варианты
							// Если нет - выполняем лучший найденный супер вариант

							Debug::Log("DEOS : Super variant = " + _event.GetPW_Name(idx) + "; Meets = " + std::to_string(_event.GetPW_Meet(idx)), Module);
							//super_variants.push_back(short(idx));
							find = true;
							if (_event.GetPW_Meet(idx) > meets) {
								meets = _event.GetPW_Meet(idx);
								deepModeIndex = index;
							}
						}

						if (index > 0) { index--; goto Repeat; } // Выполняем проверку
						else  DoFindSynapse(_event, deepModeIndex, real, core);
					}
					else { // Если не было найдено абсолютных супер вариантов
						if (percent >= PeriodicSix + 10) { ///\%isGood +10
							DoFindSynapse(_event, index, real, core);
							find = true;
							if (!find && index > 0) { index--; /*Debug::Log(index);*/ goto Repeat; } // Если не вышло
						}
						else if (index > 0) { index--; goto Repeat; }/// $Если $не $подходит
						else {
							/*
							if (super_variants.size() != 0) { // Финальное умо-заключение, при том, что не было найдено абсолютных вариаций
								if (super_variants.size() > 1)
									for (size_t s = 1; s < super_variants.size(); s++)
										if (std::get<3>(_event.PerhapsWill[super_variants[s]]) > std::get<3>(_event.PerhapsWill[super_variants[mem_sup_index]])) {
											mem_sup_index = super_variants[s];
										}

								std::string _name = std::get<0>(_event.PerhapsWill[mem_sup_index]);
								Debug::Log("DEOS : Finaly variant = " + _name, Module);
								Consequence* _final = new Consequence(_name.substr(2), ToAType(_name[0]));
								//core._reality->DoAction((*_final).action);
								_name.clear(); delete _final;
							}
							else {*/
							// "Последняя надежда", тупо для отладки
							std::string _name = std::get<0>(_event.PerhapsWill[idx]);
							Debug::Log("DEOS : last variant = " + _name, Module);
							Consequence* _final = new Consequence(_name.substr(2), ToAType(_name[0]));
							//core._reality->DoAction((*_final).action);
							_name.clear(); delete _final;
							//}
						}
					}
				}
				else { ///\to NOT FOUND SOLUTION
					Debug::Log("DEOS : Not found solution = \"" + _event.name + "\" at synapse \"" + s_au + "\" index=" + std::to_string(index), Module);
					if (index > 0) { index--; goto Repeat; }
				}
			}
			
			//super_variants.clear(); //last_variants.clear();
			return find;
		}
		else { /*super_variants.clear(); last_variants.clear();*/ return false; }
	}

	void SpaRcle::CentralCore::ProcessingEvent(Consequence& event, std::string& Situation, CentralCore& core) {
		// TODO : Требуется калибровка порогов чувствительности.
		if (event.Bad > event.Good && event.Bad > 3) { // Пытаемся выкрутиться из ситуации с наименьшим ущербом
			Debug::Log("CentralCore::PE : " + event.name + " is Bad! \n\tBad : " + std::to_string(event.Bad) + "\n\tGood : " + std::to_string(event.Good)); 
			bool find = DoEventOfSynapse(event, Situation, core); // Если имеется хоть какое-то решение

			if (!find) // Если все совсем печально...
			{
				Debug::Log("CentralCore::PE : Find opposite...", Mind);

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

				/*  Для начала ищем противоположности этого следствия.
					Если не выходит, идем ва-банк и пробуем повторить плохое действие, с некоторыми изменениями...
					Может помочь, если нас бьют и мы не понимаем за что (просто издеваются), и нам нужно как-то спастись.
					В какой-то мемент, мы понимаем, что можем и тоже бить, но это плохо, однако, мы в безысходном положении
					и у нас отчаяние, потому, нам ничего не остается, как бить в ответ, то-есть, повторить плохое действие.
				*/
			}
			else {
				Debug::Log("ProcessingEvent : solution is found.", Info);
				return; /* Ничего не делаем, тк в DEOS уже все выполнилось*/ }
		}
		// Если плохо 
		else if (event.Good > event.Bad && event.Good > 5) {
			Debug::Log("CentralCore::PE : " + event.name + " is Good"); /* Debuging */
			DoEventOfSynapse(event, Situation, core); // Ищем продолжение
		}
		// Если хорошо
		else
		{
			///\TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			Debug::Log("CentralCore::PE : TODO!", DType::Error);
			Debug::Log("CentralCore::PE : \"" + event.name + "\" is undefined. \n\t" + "Good : " + std::to_string(event.Good) + "\n\tBad : " + std::to_string(event.Bad));
			// Повторяем для того, чтобы понять : плохо это или хорошо.
			(*core._reality).DoAction(event.action, event.name); // Trying to repeat
		}
		// Если не поняли
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
				if (causal.size_unchk_ev == 0) {
					load++;
				//Ret: /// name == EmptyName |and| logical loop
					Consequence& conseq = core.Events_conq[deep];
					(*_core).AddSE(conseq.name, false); // !Петля

					if (conseq.name.empty()) {
						Debug::Log("CentralCore : [1] Unknown ERROR => conseq.name == \"\"!", Error); Sleep(1000);
						core.Events_sens.erase(core.Events_sens.begin() + deep);

						if (deep < core.Events_conq.size()) core.Events_conq.erase(core.Events_conq.begin() + deep);
						else {
							Debug::Log("CentralCore : [1] Fatal ERORR!\n\tdeep = " + std::to_string(deep) + "\n\tsize = " + std::to_string(core.Events_conq.size()), Error);
							core.Events_conq.clear();
							core.Events_sens.clear();
							Sleep(1000);
						}

						deep = 0;
						continue;
					}
					else if (conseq.name == Settings::EmptyName) {
						core.Events_conq.erase(core.Events_conq.begin() + deep);
						core.Events_sens.erase(core.Events_sens.begin() + deep);
						if(deep > 0) deep--;
						continue;
						/*
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
						*/
					}
					else {
						/* if (core.Events_sens.size() > deep + 1) {
							if (conseq.EventData.sec + 5 >= core.Events_conq[deep + 1].EventData.sec) {
								//Debug::Log(conseq.name);
								deep++;
								//goto Ret;
								continue;
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
						else */
						if (core.Events_sens.size() == 1) {
							load++; // resource monitor
							Consequence& conseq = core.Events_conq[0];
							if (conseq.name == Settings::EmptyName)
								(*_core).AddSE(conseq.name, false);
							else if (conseq.name.empty()) {
								Debug::Log("CentralCore : [2] Unknown ERROR => conseq.name == \"\"!", Error); Sleep(1000);
							}
							else {
								Debug::Log("CentralCore : only (" + core.Events_sens[0] + "): " + conseq.name);
								CentralCore::ProcessingEvent(conseq, core.Events_sens[deep], core);
							}
							core.Events_sens.erase(core.Events_sens.begin());
							core.Events_conq.erase(core.Events_conq.begin());

						}
						else {
							if (core.Events_sens.size() > deep + 1) {
								deep++;
								continue;
							}
							
							std::string& sit = core.Events_sens[deep];
							Debug::Log("CentralCore : multi (" + sit + "): " + conseq.name);

							CentralCore::ProcessingEvent(conseq, core.Events_sens[deep], core);
							//if (core.Events_sens.size() != 0) {
								//core.Events_sens.erase(core.Events_sens.end() - deep, core.Events_sens.end());
								//core.Events_conq.erase(core.Events_conq.end() - deep, core.Events_conq.end());

							deep++;
								core.Events_sens.erase(core.Events_sens.begin(), core.Events_sens.begin() + deep);
								core.Events_conq.erase(core.Events_conq.begin(), core.Events_conq.begin() + deep);

								//if (core.Events_conq.size() != 0) {
								//	core.Events_sens.erase(core.Events_sens.begin());
								//	core.Events_conq.erase(core.Events_conq.begin()); }
				
							//}
							deep = 0;
						}
					}
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
			/// \see %Определяем %принадлежность %этого %события %к %нам (!сделали !его !мы !или !нет)

			this->SE_With_MyActions += Synapse::GetSensivityOfName(event_name, IDoIt);
		}
		event_name.clear(); event_name.~basic_string();
	}
	void CentralCore::NewEvent(Consequence& event, std::string& Situation) {
		//if(event.name != Settings::EmptyName) Debug::Log("CentralCore : New event = "+ event.name, Info);
		Events_conq.push_back(event);
		Events_sens.push_back(Situation); }
	void CentralCore::Start() {
		if (!Settings::PathsIsSet) {
			Debug::Log("CentralCore::Start FATAL : Paths is not set!", Error);
			Settings::IsActive = false;
			//Debug::Log("CentralCore::Start = WARNING : Paths is not set! Setting to default...");
			//TCHAR cwd[100];
			//GetCurrentDirectory(100, cwd);
			//std::wstring ws(&cwd[0]); // WARNINGS!!!!!
			//Settings::SetDefaultPaths(std::string(ws.begin(), ws.end()));
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
	void CentralCore::ConnectTCP(TCP* tcp) { this->_tcp = tcp; }
	TCP* CentralCore::GetTCP() { 
		if (_tcp == NULL) { Debug::Log("CentralCore FATAL : TCP* is NULL!", Error); Settings::IsActive = false; Sleep(2000); return nullptr; }
		else return _tcp; }
}
