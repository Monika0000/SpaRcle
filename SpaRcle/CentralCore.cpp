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



	bool DoEventOfSynapse(Consequence& event, std::string& Situation, CentralCore& core) {
		RealityCore& real = (*core._reality);
		if (event.Synapses.size() > 0) {
			Helper::SelectionSort(event.Synapses);
			int index = event.Synapses.size() - 1;
			//Debug::Log("DoEventOfSynapse : " + event.name + " variants = " +std::to_string(index + 1));
			bool find = false;
			size_t dp = 0;

		Repeat:
			//Debug::Log(index);
			//if (event.Synapses[index].get<2>() > 0)
			if (event.Synapses[index].get<1>() > 0)
			{
				size_t idx = 0; double percent = 0; auto& s_au = event.Synapses[index].get<0>();
				for (size_t t = 0; t < event.PerhapsWill.size(); t++) 
					if (event.PerhapsWill[t].get<0>() == s_au) {
						/// \see Нормализуем длину чувствительностей и сравниваем их между собой.
						double per = Helper::SimilarityPercentage(event.PerhapsWill[t].get<1>(), core.SE_With_MyActions, true, true);
						if (per > percent) { percent = per; idx = t; }
					}

				//Debug::Log("Result : " + event.Synapses[index].get < 0 >() + std::to_string(max));

				//double percent = Helper::SimilarityPercentage(Situation, event.Synapses[index].get<1>(), true);
				//Debug::Log(Situation + "   in_ev:" + event.Synapses[index].get<1>() + " | Name : " + event.Synapses[index].get<0>());

				//%TODO : Возможно лучше сортировать и искать самый схожий синапс

				Debug::Log("DoEventOfSynapse : Similarity percentage situation = " + std::to_string(percent));// + "\n\tFirst : " + Situation +
				if (percent >= PeriodicSix + 10) { /// \bug +10
					Debug::Log("DoEventOfSynapse : Find synapse = " + event.Synapses[index].get<0>());
					//%Выполняем %полезное %действие...
					std::string syn = event.Synapses[index].get<0>();
				Deep:
					if (syn[0] == 'S') {
						//syn = syn.substr(2);

						Consequence con;
						if (con.Load(syn.substr(2), AType::Speech, false, false)) {
							real.DoAction(con.action);
							real.core->AddSE(con.name, true);
							find = true;

							if (con.Synapses.size() != 0) {
								std::string sens_log; size_t index = 0; double max = 0;

								for (size_t t = 0; t < con.Synapses.size(); t++) {
									//auto& a = con.Synapses[t];
									//double perc = Helper::SimilarityPercentage(core.SE_With_MyActions, a.get<1>(), true);

									size_t idx_2 = 0; double perc = 0; auto& s_au_2 = con.Synapses[t].get<0>();
									for (size_t tt = 0; tt < con.PerhapsWill.size(); tt++)
										if (con.PerhapsWill[tt].get<0>() == s_au_2) {
											//double per = Helper::GetPercent(con.PerhapsWill[tt].get<1>(), core.SE_With_MyActions);
											/// \see Нормализуем длину чувствительностей и сравниваем их между собой.
											double per = Helper::SimilarityPercentage(con.PerhapsWill[tt].get<1>(), core.SE_With_MyActions, true, true);
											if (per > perc) { perc = per; idx_2 = tt; }
										}

									if (t == 0) max = perc;
									else if (perc >= max) { index = t; max = perc; }

									//sens_log += "\t" + con.Synapses[t].get<1>() + " : " + std::to_string(perc) + " = " + con.Synapses[t].get<0>() + "\n";
									//sens_log += "\t" + a.get<1>() + " : " + std::to_string(perc) + " = " + a.get<0>() + "\n";
								}
								Debug::Log("DoEventOfSynapse : " + core.SE_With_MyActions + "\n" + sens_log + "\tResult : " + con.Synapses[index].get<0>() + "; Max = " + std::to_string(max));
								if (max > 58) { // 76
									syn = con.Synapses[index].get<0>();
									//}
										//if (max > 6) {
									dp++;
									if (dp > 20)
										Debug::Log("CentralCore : Logical loop! See to logs...", Warning);
									else
										goto Deep;
								}
							}
						}
						else
							Debug::Log("DoEventOfSynapse::DoAction : Failed! \n\t  Name : \"" + con.name + "\"\n\t  Type : "
								+ ToString(con.action.type) + "\n\t  Synapse : " + syn, Warning);
					}
					else
						Debug::Log("DoEventOfSynapse::DoAction : Unknown type action! \n\tName : \"" + syn + "\" \n\tEvent : " + event.name + "\n\tIndex : " + std::to_string(index), Error);

					if (!find && index > 0) { index--; /*Debug::Log(index);*/ goto Repeat; } // Если не вышло
				}
				else if (index > 0) { /// $Если $не $подходит
					index--; goto Repeat;
				}
			}
			return find;
		}
		else
			return false;
	}

	void SpaRcle::CentralCore::ProcessingEvent(Consequence& event, std::string Situation, CentralCore& core)
	{
		// TODO : Требуется калибровка порогов чувствительности.

		if (event.Bad > event.Good && event.Bad > 3) {

			// Пытаемся выкрутиться из ситуации с наименьшим ущербом
			Debug::Log("CentralCore : " + event.name + " is Bad"); /* Debuging */

			// Если имеется хоть какое-то решение
			bool find = DoEventOfSynapse(event, Situation, core);

			if (!find) // Если все совсем печально...
			{
				Debug::Log("CentralCore : Я не знаю, что мне делать!", Mind);
				// А..Э.. А что мне делать?
				// TODO : current work!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// Пытаемся придумать решение...

				//Debug::Log(ToString(event.action.type));
				//Debug::Log(event.name);
				Consequence opposite; bool seccues = false;
				if (LogicalCore::GetOpposite(opposite, event))
				{
					std::string ev_name = event.name;
					std::string op_name = opposite.name;

					Debug::Log("CentralCore::GetOpposite : " + ev_name + " => " + op_name);
					seccues = true;
				}

				if (!seccues) {
					Debug::Log("CentralCore : Все совсем плохо!", Mind);
				}
				else {
					Debug::Log("Opposite : " + opposite.name, Mind);
				}

				/*  Для начала ищем противоположности этого следствия.
					Если не выходит, идем ва-банк и пробуем повторить плохое действие, с некоторыми изменениями...
					Может помочь, если нас бьют и мы не понимаем за что (просто издеваются), и нам нужно как-то спастись.
					В какой-то мемент, мы понимаем, что можем и тоже бить, но это плохо, однако, мы в безысходном положении
					и у нас отчаяние, потому, нам ничего не остается, как бить в ответ, то-есть, повторить плохое действие.
				*/
			}
		}
		// Если плохо 
		else if (event.Good > event.Bad && event.Good > 5) {
			Debug::Log("CentralCore : " + event.name + " is Good"); /* Debuging */
			// (OLD) О даааа с*ка как хорошо-то! (наслаждаемся и возможно еще раз повторяем)
			DoEventOfSynapse(event, Situation, core);
		}
		// Если хорошо
		else
		{
			Debug::Log("CentralCore : \"" + event.name + "\" is undefined. \n\t" + "Good : " + std::to_string(event.Good) + "\n\tBad : " + std::to_string(event.Bad));
			// Повторяем для того, чтобы понять : плохо это или хорошо.
			(*core._reality).DoAction(event.action); // Trying to repeat
		}
		// Если не поняли

		//core.Events.erase(core.Events.begin());
	}

	void CentralSolution(int* DelayCPU, CentralCore* _core)
	{
		Sleep(100);
		CentralCore& core = *_core;
		RealityCore& real = *core._reality;
		CausalityCore& causal = *core._causality;
		//Consequence pain = Consequence("pain", Action(Sound("PAIN")), 1, 40, 10); 
		//	pain.Synapses.push_back(boost::tuple<std::string, std::string, double>("S/nothing", "rj", 14));
		//	pain.Synapses.push_back(boost::tuple<std::string, std::string, double>("S/regen", "snrj", 100));
		//	pain.Synapses.push_back(boost::tuple<std::string, std::string, double>("S/sit", "nstj", 90));
		//core.Events.push_back(boost::tuple<Consequence, std::string>(pain, "nsrj"));
		size_t timer = 0, size_ev = 0, deep = 0;

		while (true)
		{
			//break;

			Sleep(*DelayCPU);
			//_asm { }
			try {
				core._logic->DoIt(core.Tree.Branches[core.Tree.Branches.size() - 1].Tasks[
					core.Tree.Branches[core.Tree.Branches.size() - 1].Tasks.size() - 1]);

				if (causal.UncheckedEvents.size() == 0) {
					//if (core.Events.size() > 0) {
					//	Debug::Log("multi (" + core.Events[0].get<1>() + "): " + core.Events[0].get<0>().name);
					//	core.Events.erase(core.Events.begin()); }

					if (core.Events.size() > 1) {
					Ret:
						Consequence& conseq = core.Events[deep].get<0>();
						(*_core).AddSE(conseq.name, false); // Петля

						if (conseq.name == Settings::EmptyName) {
							//Debug::Log("EPMTY : " + std::to_string(deep) + " " + std::to_string(core.Events.size()));
							//core.Events.erase(core.Events.begin() + deep);
							if (core.Events.size() > deep + 1)
							{
								deep++;
								goto Ret;
							}
							else
							{
								for (size_t t = deep; t > 0; t--)
									core.Events.erase(core.Events.end() - t);
								core.Events.erase(core.Events.begin());
								deep = 0;
							}
							//deep = 0;
						}
						else {
							std::string sit = core.Events[deep].get<1>();

							if (core.Events.size() > deep + 1) {
								if (conseq.EventData.sec + 5 >= core.Events[deep + 1].get<0>().EventData.sec) {
									deep++;
									goto Ret;
								}
								else {
									Debug::Log("CentralCore : While - Logical loop. \n\tEvents : "
										+ std::to_string(core.Events.size()) + "\n\tDeep : " + std::to_string(deep) + "\n\tClean events list...", Warning);

									if (core.Events.size() != 0) {
										for (size_t t = deep; t > 0; t--)
											core.Events.erase(core.Events.end() - t);
										core.Events.erase(core.Events.begin());
									}
									deep = 0;
								}
							}
							else {
								Debug::Log("CentralCore : multi (" + sit + "): " + conseq.name);
								//std::reverse(core.Events[deep].get<1>().begin(), core.Events[deep].get<1>().end());
								CentralCore::ProcessingEvent(conseq, core.Events[deep].get<1>(), core);

								//std::cout << core.Events.size() << std::endl;
								//std::cout << deep << std::endl;
								if (core.Events.size() != 0) {
									for (size_t t = deep; t > 0; t--)
										core.Events.erase(core.Events.end() - t);
									core.Events.erase(core.Events.begin());
								}
								deep = 0;
							}
						}
					}
					else if (core.Events.size() == 1) {
						Consequence& conseq = core.Events[0].get<0>();
						if (conseq.name == Settings::EmptyName) {
							(*_core).AddSE(conseq.name, false);
							//Debug::Log("dasdsad");
						}
						else {
							Debug::Log("CentralCore : only (" + core.Events[0].get<1>() + "): " + conseq.name);
							//std::reverse(core.Events[deep].get<1>().begin(), core.Events[deep].get<1>().end());
							CentralCore::ProcessingEvent(conseq, core.Events[deep].get<1>(), core);
						}
						core.Events.erase(core.Events.begin());
					}
				}

				/*
					if (core.Events.size() > 0)
						if (size_ev != core.Events.size()) {
							if(size_ev < core.Events.size())
								//if (core.Events[0].get<0>().PerhapsWill.size())
									timer = 50;
							////////////////////////////
							size_ev = core.Events.size();
							deep++;
						}
						else {
							if (timer == 0) {
								Debug::Log(core.Events[deep].get<0>().name);
								core.Events.erase(core.Events.begin());
								deep--;

							}
							else timer--;
						}
						*/
						//std::string s = Helper::format() << count;
						//core->_logic->DoIt(Task(Consequence("Life " + s, Action())));
						//count++;
						//core->DelayCPU = 1500;
			}
			catch (...) {
				Debug::Log("CentralCore : An exception has occured! \n\tEvents : " + std::to_string(core.Events.size()) +
					"\n\tDeep : " + std::to_string(deep), Error);
				//					"\n\tDeep : " + std::to_string(deep)
				break;
			}
			if (Settings::CoreDebug)
				Debug::Log("Processing core...");
		}
	}

	void CentralCore::AddSE(std::string event_name, bool IDoIt)
	{
		if (true)
			if (IDoIt)
				Debug::Log("CentralCore::AddSE : " + event_name + " ~ " + this->SE_With_MyActions + " (SELF)");
			else
				Debug::Log("CentralCore::AddSE : " + event_name + " ~ " + this->SE_With_MyActions);
		//Debug::Log(this->SE_With_MyActions.size());
		if (event_name == Settings::EmptyName) {
			if (this->SE_With_MyActions.size() > 0)
				this->SE_With_MyActions.erase(this->SE_With_MyActions.begin(), this->SE_With_MyActions.begin() + count_word_in_sensiv);
		}
		else
		{
			if (this->SE_With_MyActions.size() >= SpaRcle::max_size_SEWMA * SpaRcle::count_word_in_sensiv)
				for (size_t t = 0; t < SpaRcle::count_word_in_sensiv; t++)
					this->SE_With_MyActions.erase(this->SE_With_MyActions.begin() + t);

			if (IDoIt) for (size_t i = 0; i < event_name.size(); i++)	event_name[i] = toupper(event_name[i]);
			/// \see %Определяем %принадлежность %этого %события %к %нам (!сделали !его !мы !или !нет)

			this->SE_With_MyActions += Helper::GetSensivityOfName(event_name);
		}
	}

	/*
	void CentralCore::AddSE() {
		//Debug::Log(this->SE_With_MyActions.size());

	ret:
		//Debug::Log(this->SE_With_MyActions.size());
		if (this->SE_With_MyActions.size() >= 1)
			if (std::isupper(this->SE_With_MyActions[0])) {
				this->SE_With_MyActions.erase(this->SE_With_MyActions.begin());
				goto ret;
			}
			else
				this->SE_With_MyActions.erase(this->SE_With_MyActions.begin());
	}
	*/

	void CentralCore::NewEvent(Consequence event, std::string Situation)
	{
		Events.push_back(boost::tuple<Consequence, std::string>(event, Situation));
		//Debug::Log(event.name + " - 123");
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
