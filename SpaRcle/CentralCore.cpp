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
	CentralCore::~CentralCore() {
		if (Process.joinable()) Process.detach();
		Debug::Log("-> The central core has completed it's work!", Info);
	}

	bool CentralCore::DoFindSynapse(Consequence& _event, int _index, std::string& Situation, std::string PWSit) {
		size_t dp = 0;
		//std::string caus_name = _event.name;
		//AType		caus_type = _event.action.type;
		//std::string syn = _event.GetSN_Name(_index);
		std::string syn = _event.GetPW_Name(_index);
		Consequence copy = _event;

		Debug::Log("DFS : Find synapse = " + syn, Module); 
		//%Выполняем %полезное %действие...
		//std::string syn = event.GetPW_Name(_index); //WTF????????????
	Deep:
		if (syn[0] == 'S' || syn[0] == 'M') {
			Consequence con;
			if (con.Load(syn.substr(2), ToAType(syn[0]), true, false, "DFS") == 1) {
				Neuron* neuron = Action::LoadNeuron(con, PWSit);

				if(neuron == nullptr){
					// create action with random values
					//Debug::Log("CentralCore::DoFindSynapse : Random values");
					con.action.SetData(new Neuron(Helper::RandomFloat(), Helper::RandomFloat()), Situation, copy);
				}
				else {
					if(!con.action.SetData(neuron, Situation, copy))
						con.action.SetData(new Neuron(Helper::RandomFloat(), Helper::RandomFloat()), Situation, copy);
				}

				_reality->DoAction(con.action,Situation, con.name);

				if(dp == 0) Action::SaveNeuron(syn, _event, PWSit, Situation);  //\^SAVE ^NEURON
				else		Action::SaveNeuron(syn, con   , PWSit, Situation);	//\^SAVE ^NEURON

				AddSE(con, true);

				if (con.Synapses.size() != 0) {
					std::string sens_log; size_t index = 0; double max = 0; 
					std::vector<short> super_indx; bool super = false; //float max = 0;

					for (size_t t = 0; t < con.Synapses.size(); t++) {
						size_t idx_2 = 0; //double perc = 0; 
						auto& s_au_2 = con.GetSN_Name(t);
						if (s_au_2[0] == 'V') { ///\^TODO 
							con.Synapses.erase(con.Synapses.begin() + t);
							continue;
						}
						for (size_t tt = 0; tt < con.PerhapsWill.size(); tt++)
							if (con.GetPW_Name(tt) == s_au_2) {
								/// \see Нормализуем длину чувствительностей и сравниваем их между собой.
								double per = Synapse::SimilarityPercentage(con.GetPW_Sens(tt), SE_With_MyActions, true, true);
								if (per > max) { 
									if (super) { super_indx.clear(); super = false; }
									max = per; idx_2 = tt;
									index = idx_2;
									if (Settings::DFSDebug) Debug::Log("DFS : Synapse ["+con.GetSN_Name(t)+"] = "+std::to_string(per) + 
										" {"+ con.GetPW_Sens(tt) +"}", Module);
								}
								else if(per == max) {
									if (Settings::DFSDebug) Debug::Log("DFS : super variant = "+ con.GetPW_Name(tt) + " ["+ std::to_string(per) +"] {"+ con.GetPW_Sens(tt) +"}", Module);
									super = true;
								}
							}
					}

					if (super) {
						short ind = 0; int meets = 0;
						for (short i = 0; i < (short)super_indx.size(); i++) {
							if (con.GetPW_Meet(super_indx[i]) > meets && con.GetPW_Name(i)[0] != 'V') {
								meets = con.GetPW_Meet(super_indx[i]);
								ind = i;
							}
						}
						Debug::Log("DFS finaly (super) : \n\tSens : " + SE_With_MyActions + "\n" + sens_log + "\tSuper result : " +
							con.GetPW_Name(ind) + "; Max = " + std::to_string(max), Module);
						///\TODO !!!!!!!
						if (max > Settings::MinSimilarityPerc) { // 76 // 58
							syn = con.GetPW_Name(ind);
							PWSit = con.GetPW_Sens(ind);
							//caus_name = con.name; caus_type = con.action.type;
							copy.~Consequence(); copy = con;
							dp++;
							if (dp > 20) Debug::Log("DFS : Logical loop! See to logs...", Warning);
							else goto Deep;
						}
					}
					else {
						Debug::Log("DFS finaly : \n\tSens : " + SE_With_MyActions + "\n" + sens_log + "\tResult : " +
							con.GetPW_Name(index) + "; Max = " + std::to_string(max), Module);

						if (max > Settings::MinSimilarityPerc) { // 76 // 58
							syn = con.GetPW_Name(index);
							PWSit = con.GetPW_Sens(index);
							copy.~Consequence(); copy = con;
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
			Debug::Log("DFS::DoAction : Unknown type action! \n\tName : \"" + syn + "\" \n\tEvent : " + _event.name + "\n\tIndex : " + std::to_string(_index), Error);

		PWSit.clear(); PWSit.~basic_string(); copy.~Consequence();
		return false;
	}
	bool CentralCore::DoEventOfSynapse(Consequence& _event, std::string& Situation) {
		this->temp_sim_sup_per = 0;
		normal_variants.clear(); super_variants.clear(); sens_variants.clear();

		static std::string s_au;
		int last_index = -1; std::string last_sens = "";
		RealityCore& real = (*_reality);

		size_t mem_sup_index = 0;

		Debug::Log("DEOS : event has " + std::to_string(_event.Synapses.size()) + " synapses.", DType::Module);

		if (_event.Synapses.size() > 0) {
			Helper::SelectionSort(_event.Synapses);
			int index = _event.Synapses.size() - 1;
			bool find = false, super = false;
			int meets = -1, deepModeIndex = -1;
			/// Добавить возможность крайностей - когда мы совершенно не знаем ответа, но нам нужно что-то сказать,
			/// и у нас есть варианты (хоть и не самые подходящие(но не плохие)) ты мы будем пытаться их выполнить
		Repeat:
			if (_event.GetSN_HP(index) > 0) { // Полезность синапса внутри _event

				/// ^see ^IIIIIIIIIIIIIIIII Ищем самый схожий синапс
				double percent = 0; // Итоговый процент схожести ситуации на одну из возможных вариаций
				size_t idx = 0;		// Индекс найденой вариации
				s_au = _event.GetSN_Name(index); // Имя синапса под индексом перечисления
				if (s_au[0] != 'V') {
					for (size_t t = 0; t < _event.PerhapsWill.size(); t++) {
						if (_event.GetPW_Name(t) == s_au) {
							if (_event.GetPW_HP(t) < 0) {
								Debug::Log("DEOS : Bad synapse! \"" + s_au + "\" = " + _event.GetPW_Sens(t), Module); 
								continue; }
							/// \see Нормализуем длину чувствительностей и сравниваем их между собой.
							double per = Synapse::SimilarityPercentage(_event.GetPW_Sens(t), SE_With_MyActions, true, true);
							if (per > percent) {
								percent = per;
								idx = t;
							}
						}
					}
				} else percent = 0;

				if (percent != 0) {
					// Обработка всех возможных и исключительных ситуаций
					if (percent > 99.f) { // Абсолютный синапс
						if (!super) {
							normal_variants.clear(); super_variants.clear(); sens_variants.clear(); super = true; meets = 0; 
							Debug::Log("DEOS : (1) Clear...", Module);
						}

						if (temp_sim_sup_per < percent) {
							temp_sim_sup_per = percent;
							meets = 0; }

						Debug::Log("DEOS : Super variant (1) = " + _event.GetPW_Name(idx) + "; Meets = " + std::to_string(_event.GetPW_Meet(idx)), Module);
						//if (_event.GetPW_Meet(idx) > meets) {
							meets = _event.GetPW_Meet(idx);
							//super_variants.push_back(short(index));
							super_variants.push_back(short(idx));
							sens_variants.push_back(_event.GetPW_Sens(idx));
						//}
					}
					else if (percent == temp_sim_sup_per) {
						if (!super) {
							normal_variants.clear(); super_variants.clear(); sens_variants.clear(); super = true; meets = 0;  
							Debug::Log("DEOS : (2) Clear...", Module);
						}

						//if(Settings::DEOSDebug)
						Debug::Log("DEOS : Super variant (2) = " + _event.GetPW_Name(idx) + "; Meets = " + std::to_string(_event.GetPW_Meet(idx)), Module);

						if (_event.GetPW_Meet(idx) > meets) {
							meets = _event.GetPW_Meet(idx);
							//super_variants.push_back(short(index));
							super_variants.push_back(short(idx));
							sens_variants.push_back(_event.GetPW_Sens(idx));
						}
					}
					else if (percent > temp_sim_sup_per) {
						if (super) {
							super = false; meets = 0;
							super_variants.clear(); sens_variants.clear();
							Debug::Log("DEOS : (3) Clear...", Module);
						}

						temp_sim_sup_per = percent;

						if (percent >= PeriodicSix + 10) {
							find = true;
							//normal_variants.push_back(index);
							normal_variants.push_back(idx
							);
							sens_variants.push_back(_event.GetPW_Sens(idx));
						}
					}

					if (Settings::DEOSDebug)
						Debug::Log("DEOS : Similarity percentage situation = " + std::to_string(percent) + " \"" + s_au + "\" [" + std::to_string(_event.GetPW_Meet(idx)) + "] " + std::to_string(super), Module);//(" + SE_With_MyActions + " " + + ")", Module);
					/// ^see ^IIIIIIIIIIIIIIIII Ищем самый схожий синапс

					// Определяем, искать дальше, или выполнить найденное, с учетом супер вариации
					if (super) {
						// Если у нас однажды был найден супер вариант, то мы выполняем проверку,
						// на принадлежность последнего найденного, к супер вариации.
						// Если принадлежит - сравниваем с последним на число встреч
						// Если не принадлежит - прверяем есть ли еще варианты, 
						// Если есть - продолжаем искать супер варианты
						// Если нет - выполняем лучший найденный супер вариант

						find = true;

						if (index > 0) { index--; goto Repeat; } // Выполняем проверку
						else {
							//if (super_variants.size() != 0)
							//	DoFindSynapse(_event, super_variants[super_variants.size() - 1]);
							//if(super_variants.size() == 0)
							Debug::Log("DEOS : Finaly super mode. Var = " + std::to_string(super_variants.size()), Module);
							CheckMonotoneAndDo(_event, super_variants, Situation, sens_variants);
						}
					}
					else { // Если не было найдено абсолютных супер вариантов
						if (index > 0) { 
							if (percent >= 58) {
								last_index = index;
								last_sens = _event.GetPW_Sens(idx);
							}
							else if (this->mono_per_temp > 50) {
								last_index = index;
								last_sens = _event.GetPW_Sens(idx);
							}

							index--; goto Repeat; 
						}
						else {
							if (normal_variants.size() != 0) {
								//DoFindSynapse(_event, variants[variants.size() - 1]);
								Debug::Log("DEOS : Finaly normal mode. Var = " + std::to_string(normal_variants.size()), Module);
								CheckMonotoneAndDo(_event, normal_variants, Situation, sens_variants);
								//END.
							}
							else {// Финальное умо-заключение, при том, что не было найдено абсолютных вариаций
								// "Последняя надежда", тупо для отладки
								std::string _name = std::get<0>(_event.PerhapsWill[idx]);
								Debug::Log("DEOS : last variant = " + _name, Module);
								//Consequence* _final = new Consequence(_name.substr(2), ToAType(_name[0]), "Central");
								//last.~Consequence(); last = Consequence(_name.substr(2), ToAType(_name[0]), "Central");
								//core._reality->DoAction((*_final).action);
								_name.clear(); //delete _final;
							}
						}
					}
				}
				else { ///\to NOT FOUND SOLUTION
					Debug::Log("DEOS : Not found solution = \"" + _event.name + "\" at synapse \"" + s_au + "\" index=" + std::to_string(index), Module);
					if (index > 0) { index--; goto Repeat; }
					else if (super_variants.size() > 0) {
						CheckMonotoneAndDo(_event, super_variants, Situation, sens_variants);
					}
					else if (normal_variants.size() > 0) {
						CheckMonotoneAndDo(_event, normal_variants, Situation, sens_variants);
					}
					else {
						if (last_index >= 0) { 
							Debug::Log("DEOS : logical break point!\n\t\tLast : " + _event.GetSN_Name(last_index), DType::System); 
							normal_variants.push_back(last_index);
							sens_variants.push_back(last_sens);
							CheckMonotoneAndDo(_event, normal_variants, Situation, sens_variants);
						}
						else Debug::Log("DEOS : logical break point! (Need Fix)", DType::System);
					}

					///\!TODO Если попадется последнее не решаемое, а остальные будут решаемые, не выполнится ни одно! (is fix)
				}
			}
			else {
				Debug::Log("DEOS : logical paradox!", DType::Warning);
				if (super)
					CheckMonotoneAndDo(_event, super_variants, Situation, sens_variants);
				else
					CheckMonotoneAndDo(_event, normal_variants, Situation, sens_variants);
			}

			return find;
		}
		else return false;
	}

	void CentralCore::CheckMonotoneAndDo(Consequence& _event, std::vector<short>& _variants, std::string& Situation, std::vector<std::string>& variants_sens) {
		static std::string debug = "", temp_syn; debug.clear();
		static size_t stat_var_size = 0, result;
		stat_var_size = _variants.size();

		Debug::Log("CheckMonotoneAndDo : " + _event.name + "("+std::to_string(stat_var_size)+")", Info);

		if (stat_var_size != 0) {
			if (mono_deep != 0) {
				if (mono_deep >= stat_var_size) mono_deep = stat_var_size - 1;

				for (size_t t = stat_var_size - 1; t > stat_var_size - mono_deep - 1; t--) {
					this->_logic->LoverPrioritySynapse( // Понижаем приоритет бесполезных синапсов
						ToATypeChar(_event.action.type) + "/" + _event.name, 
						(size_t)_variants[t], 
						(size_t)_variants[stat_var_size - mono_deep - 1]);

					//debug += " " + _event.GetSN_Name(_variants[t]);
					debug += " " + _event.GetPW_Name(_variants[t]);
				}


				if(stat_var_size > 1) Debug::Log("CMAD : useless actions :" + debug + "\n\tAlternative : "+ 
					//_event.GetSN_Name(_variants[stat_var_size - mono_deep - 1]), Module);
					_event.GetPW_Name(_variants[stat_var_size - mono_deep - 1]), DType::System);
			}

			result = stat_var_size - mono_deep - 1;
			//temp_syn = _event.GetSN_Name(result);
			temp_syn = _event.GetPW_Name(result);

			///////////////////////////////[CHECK ACTION SIM]///////////////////////////////
			if (false) {
				//Neuron* neuron = Action::LoadNeuron(temp_syn.substr(2), ToAType(temp_syn[0]), variants_sens[result]);
				Neuron* neuron = Action::LoadNeuron(_event.name, _event.action.type, variants_sens[result]);

				if (neuron != nullptr) {
					std::string data = _event.name;// = neuron->name;
					for (size_t t = 0; t < neuron->size; t++) {
						data += "\n\t" + neuron->sensitivities[t] + " " + std::to_string(neuron->value_1[t]);
					}
					Debug::Log("CMAD : " + data, DType::Info);

					delete neuron;
				}
			}
			///////////////////////////////[CHECK ACTION SIM]///////////////////////////////

			DoFindSynapse(_event, _variants[result], Situation, variants_sens[result]);
		}
	}

	void SpaRcle::CentralCore::ProcessingEvent(Consequence& _event, std::string& Situation) {
		this->mono_per_temp = _logic->IsMonotone(_event.name, Situation);
		Debug::Log("CentralCore PE : is monotone = " + std::to_string(mono_per_temp));

		if (!mono_wait) { // Проверка монотонности ситуации.
			if (mono_per_temp > Settings::Monotone_MaxPercent) 
				mono_deep++; // Если монотонна, ищем другое решение, отличное от предыдущего.
			else 
				mono_deep = 0;
			mono_wait = true;
		} else mono_wait = false;

		///\^TODO : Требуется калибровка порогов чувствительности.
		if (_event.GetSummHP() < 0) { //(event.Bad > event.Good && event.Bad > 3) { // Пытаемся выкрутиться из ситуации с наименьшим ущербом
			Debug::Log("CentralCore::PE : " + _event.name + " is Bad! \n\tBad : " + std::to_string(_event.Bad) + "\n\tGood : " + std::to_string(_event.Good)); 
			bool find = DoEventOfSynapse(_event, Situation); // Если имеется хоть какое-то решение

			if (!find) // Если все совсем печально...
			{
				Debug::Log("CentralCore::PE : Find opposite...", Mind);

				Consequence opposite; bool seccues = false;
				if (LogicalCore::GetOpposite(opposite, _event)) {
					std::string ev_name = _event.name;
					std::string op_name = opposite.name;

					Debug::Log("CentralCore::GetOpposite : " + ev_name + " => " + op_name);
					seccues = true;
				}

				if (!seccues) {
					Debug::Log("CentralCore : opposite is not found!", Mind);

				}
				else {
					Debug::Log("Opposite : " + opposite.name, Mind);
					///\TODO

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
		else if(_event.GetSummHP() > 0) { //if (event.Good > event.Bad && event.Good > 5) {
			Debug::Log("CentralCore::PE : " + _event.name + " is Good"); /* Debuging */

			DoEventOfSynapse(_event, Situation); // Ищем продолжение
		}
		// Если хорошо
		else {
			///\TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//Debug::Log("CentralCore::PE : TODO!", DType::Error);
			Debug::Log("CentralCore::PE : \"" + _event.name + "\" is undefined. \n\t" + "Good : " + std::to_string(_event.Good) + "\n\tBad : " + std::to_string(_event.Bad));
			// Повторяем для того, чтобы понять : плохо это или хорошо.
			//_reality->DoAction(_event.action, Situation, _event.name); // Trying to repeat
		}
		// Если не поняли
	}

	void CentralCore::CentralSolution() {
	Restarting:
		Sleep(100);
		//CentralCore& core = &this;
		RealityCore& real = *this->_reality;
		CausalityCore& causal = *this->_causality;
		size_t size_ev = 0, deep = 0;
		char timer = 0; short load = 0;
		//std::string temp_synapse;

		Debug::Log("-> The central core is started!");

		while (Settings::IsActive) {
			if (timer >= 100) { timer = 0; CoreLoad = load; load = 0; }
			else timer++;

			size_events = Events_conq.size();

			if (size_events == 0) Sleep(DelayCPU);
			else try {
				if (causal.size_unchk_ev == 0) {
					load++;
				//Ret: /// name == EmptyName |and| logical loop
					Consequence& conseq = Events_conq[deep];

					AddSE(conseq, false); // !Петля

					if (conseq.name.empty()) {
						Debug::Log("CentralCore : [1] Unknown ERROR => conseq.name == \"\"!", Error); Sleep(1000);
						Events_sens.erase(Events_sens.begin() + deep);

						if (deep < Events_conq.size()) Events_conq.erase(Events_conq.begin() + deep);
						else {
							Debug::Log("CentralCore : [1] Fatal ERORR!\n\tdeep = " + std::to_string(deep) + "\n\tsize = " + std::to_string(Events_conq.size()), Error);
							Events_conq.clear();
							Events_sens.clear();
							Sleep(1000);
						}

						deep = 0;
						continue;
					}
					else if (conseq.name == Settings::EmptyName) {
						Events_conq.erase(Events_conq.begin() + deep);
						if(Events_sens.size() > 0)
							Events_sens.erase(Events_sens.begin() + deep);
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
						//temp_synapse = ToATypeChar(conseq.action.type) + "/" + conseq.name;
						//Debug::Log("Action : " + conseq.name + "; Sens : " + Events_sens[deep], System);
						//Action::SaveNeuron(temp_synapse, conseq, Events_sens[deep], Events_sens[deep]);
						
						//Action::SaveNeuron(conseq, Events_sens[deep]);

						if (Events_sens.size() == 1) {
							load++; // resource monitor
							//Consequence& conseq = Events_conq[0];
							if (conseq.name == Settings::EmptyName)
								AddSE(conseq, false);
							else if (conseq.name.empty()) {
								Debug::Log("CentralCore : [2] Unknown ERROR => conseq.name == \"\"!", Error); Sleep(1000);
							}
							else {
								Debug::Log("CentralCore : only (" + Events_sens[0] + "): " + conseq.name);
								CentralCore::ProcessingEvent(conseq, Events_sens[deep]);
							}
							Events_sens.erase(Events_sens.begin());
							Events_conq.erase(Events_conq.begin());

						}
						else {
							if (Events_sens.size() > deep + 1) {
								deep++;
								continue;
							}
							
							std::string& sit = Events_sens[deep];
							Debug::Log("CentralCore : multi (" + sit + "): " + conseq.name);

							CentralCore::ProcessingEvent(conseq, Events_sens[deep]);
							//if (core.Events_sens.size() != 0) {
								//core.Events_sens.erase(core.Events_sens.end() - deep, core.Events_sens.end());
								//core.Events_conq.erase(core.Events_conq.end() - deep, core.Events_conq.end());

							deep++;
								Events_sens.erase(Events_sens.begin(), Events_sens.begin() + deep);
								Events_conq.erase(Events_conq.begin(), Events_conq.begin() + deep);

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
				Debug::Log("CentralCore : An exception has occured! \n\tEvents : " + std::to_string(Events_sens.size()) +
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

	void CentralCore::AddSE(Consequence& _event, bool IDoIt) {
		if (Settings::CentralCoreDebug)
			if (IDoIt) Debug::Log("CentralCore::AddSE : " + _event.name + " ~ " + this->SE_With_MyActions + " (SELF)");
			else Debug::Log("CentralCore::AddSE : " + _event.name + " ~ " + this->SE_With_MyActions);
		
		if (_event.name == Settings::EmptyName) {
			if (this->SE_With_MyActions.size() > 0)
				this->SE_With_MyActions.erase(this->SE_With_MyActions.begin(), this->SE_With_MyActions.begin() + count_word_in_sensiv);
		}
		else {
			if (this->SE_With_MyActions.size() >= SpaRcle::max_size_SEWMA * SpaRcle::count_word_in_sensiv)
				for (short t = 0; t < SpaRcle::count_word_in_sensiv; t++)
					this->SE_With_MyActions.erase(this->SE_With_MyActions.begin() + t);

			//if (IDoIt) for (short i = 0; i < event_name.size(); i++) event_name[i] = toupper(event_name[i]);
			/// \see %Определяем %принадлежность %этого %события %к %нам (!сделали !его !мы !или !нет)

			this->SE_With_MyActions += Synapse::GetSensivityOfName(_event, IDoIt);
		}
		//event_name.clear(); event_name.~basic_string();
	}
	void CentralCore::NewEvent(Consequence& event, std::string& Situation) {
		//if(event.name != Settings::EmptyName) Debug::Log("CentralCore : New event = "+ event.name, Info);
		Events_conq.push_back(event);
		Events_sens.push_back(Situation); 
	}
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
		//Process = std::thread(CentralSolution, &DelayCPU, this);
		Process = std::thread(&CentralCore::CentralSolution, this);
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
