#include "pch.h"
#include <iostream>
#include <algorithm>
#include "CausalityCore.h"
#include "EmotionCore.h"
#include "RealityCore.h"
#include "CentralCore.h"
#include "windows.h"
#include "Debug.h"
#include "Helper.h"
#include "Settings.h"
#include "Memory.h"

namespace SpaRcle {
	CausalityCore::CausalityCore(int cpuSpeed)
	{
		//CoreLoad_1 = 0;
		CoreLoad = 0;
		core = NULL;
		std::vector<std::string> vector;
		if (System::Load("temp%load\\causality_data.temp", vector, false))
			for (std::string a : vector)
				if (a[0] == 'S') {
					Consequence conseq("");

					char ok = conseq.Load(a.substr(2), AType::Speech, "Caus_constr");
					if (ok == 1)
						this->CheckedEvents.push_back(conseq);
					else
						Debug::Log("CausalityCore::Constructor::Load : Unknown error! Type : " + a, Error);
				}
				else if (a == "U/" + Settings::EmptyName) {
					this->CheckedEvents.push_back(Consequence(Settings::EmptyName));
				} else {
					std::string line = "CausalityCore::Constructor::Load : Unknown type! Type : \"" + a + "\"";
					Debug::Log(line, Warning);
				}
		else
			CheckedEvents = std::vector<Consequence>();

		vector.~vector();

		DelayCPU = cpuSpeed;
		Debug::Log("-> Creating the causality core are successful! \n\t Was loaded : " + std::to_string(CheckedEvents.size()) + " elements.");
	}
	CausalityCore::~CausalityCore() {
		if (Process.joinable())
			Process.detach();

		std::string save;
		for (auto& a : CheckedEvents) {
			save += ToString(a.action.type)[0]; 
			save += "/" + a.name + "\n";
		}

		//System::Save("causality_data.temp", save, "\\temp%load"); useless

		UncheckedEvents.~vector();
		CheckedEvents.~vector();
		Debug::Log("-> The causality core has completed it's work!", Info);
	}
 
	inline bool SpaRcle::CausalityCore::CheckedEventsProcessing(std::vector<Consequence>& Ref_ev, std::vector<std::string>&Temp_Causes, std::vector<int>& Temp_Meets, const bool Diagnostic)
	{
		// Здесь все вроде-бы готово. Пока-что лучше больше не трогать данный участок кода.
		Consequence& conq = Ref_ev[Settings::Size_SCP]; // 6-ый element

		for (size_t t = 0; t < Settings::Size_SCP; t++) {
			/* Здесь мы суммируем и добавляем в нейрон синапсы, ведущие к другим нейронам */
			if (Ref_ev[t].name == Settings::EmptyName)  continue; 
			
			#pragma region [========== CAUSES =========] 
			std::string C_name; C_name += ToString(Ref_ev[t].action.type)[0];
			C_name += "/" + Ref_ev[t].name; // Генерируем имя
			
			if (int C_index = Synapse::IndexOfSynapse(conq.Causes, C_name); C_index == -1) { // Ищем схожую причину
				// Добавляем синапс на нейрон являющийся причиной данного следствия (нейрона)
				conq.Causes.push_back(std::tuple<std::string, int, double>(C_name, 1, Ref_ev[t].GetSummHP()));

				Temp_Causes.push_back(C_name); // Добавляем причину для дальнейшей обработки
				Temp_Meets.push_back(1); // Добавляем колличество встреч данной причины

				if(Settings::EventsProcessigDebug) Debug::Log("CausalityCore : add cause \"" + C_name + "\" to \"" + conq.name + "\"");
			}
			else {
				if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : summ cause \"" + conq.name + "\" with \"" + C_name + "\"");
				conq.GetCS_HP(C_index) = (conq.GetCS_HP(C_index) + Ref_ev[t].GetSummHP()) / 1.5f; /* Суммируем полезность */
				conq.GetCS_Meet(C_index)++; // Увеличиваем количество встреч данного события на 1

				Temp_Causes.push_back(C_name); // Добавляем причину для дальнейшей обработки
				Temp_Meets.push_back(conq.GetCS_Meet(C_index)); // Добавляем колличество встреч данной причины
			}
			#pragma endregion
		}
		/* Для начала узнаем, что является причиной этого следствия */
		for (size_t t = 0; t < Settings::Size_SCP + 1; t++) {
			#pragma region [ ========= SYNAPSE ========= ]
			Consequence& ref_will = Ref_ev[t + Settings::Size_SCP + 1];

			if (ref_will.name == Settings::EmptyName) continue;

			std::string Sensivity = Synapse::GetSensivityCauses(Ref_ev, t + Settings::Size_SCP);
			Sensivity = Synapse::ClearSensivity(Sensivity);
			/* Получаем то, что описывает нашу текущую ситуацию относительно будущего следствия */

			std::string S_name; S_name += ToString(ref_will.action.type)[0];
			S_name += "/"; S_name += ref_will.name; // Генерируем имя

			if (ref_will.GetSummHP() > 0) { // Ищем синапсы, чтобы можно было продолжить действия после этого нейрона
				int S_index = Synapse::IndexOfSynapse(conq.Synapses, S_name); // Получаем индекс
				if (S_index == -1) { 
					// Добавляем синапс на новый нейрон, который нужно будет выпоолнить после этого
					conq.Synapses.push_back(std::tuple<std::string, double>(S_name, ref_will.GetSummHP())); // ======== Добавляем новый снапс ========
					if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : add synapse \"" + S_name + "\" to \"" + conq.name + "\"");
				}
				else { // Summ consequences (synapses)
					if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : summ synapse \"" + conq.name + "\" with \"" + S_name + "\"");
					Synapse::SummHpSyns(conq.Synapses[S_index], ref_will.GetSummHP());
				}
			}

			#pragma endregion

			Synapse::FindAndSummSensiv(conq, S_name, Sensivity, ref_will.GetSummHP()); // Perhaps will
		}
		/* А теперь  */

		if (!Diagnostic)
			conq.Save(); // Re-write file
		else
			if (!Consequence::Save(&conq, true))
				return false;

		return true;
	}

	void CausalitySolution(int* delay, CausalityCore* _core) {
		CentralCore& C_ref = *(*_core).core;
		EmotionCore& E_ref = *C_ref._emotion;
		RealityCore& R_ref = *C_ref._reality;
		LogicalCore& L_ref = *C_ref._logic;

		std::string& Current_sensivity = (*_core).Current_sensivity;
		Consequence _event, helpData;
		short timer = 0, load_1 = 0; 
		double bad = 0, good = 0;
		Consequence empty_conq(Settings::EmptyName);
		std::string empty_sens = Settings::EmptyName;
		std::string Situation = "";
		std::vector<std::string> Temp_Causes;
		std::vector<int> Temp_Meets;
		Consequence load_conq;
		size_t size_temp = 0;
		unsigned char count_sens = 0;
		std::vector<std::string> clean_sensiv;

		/*
		#pragma region [===== Pre-Processing =====]

		/// \see В данном регионе мы определяем начальную ситуацию, при которой программа должна начать работать.

		std::vector<std::string> temp;
		//temp.push_back(std::string(count_word_in_sensiv, '.'));
		temp.push_back(".");

		if ((*_core).CheckedEvents.size() == 0)
			for (size_t i = 0; i < 10; i++) {
				//Sensivity_List.push_back(std::string(".", i + 1));
				(*_core).CheckedEvents.push_back(Consequence(Settings::EmptyName));
			}

		if ((*_core).CheckedEvents.size() > 0) {
			if ((*_core).CheckedEvents.size() < 10) {
				Debug::Log("CausalitySolution : CheckedEvents < 10! Possible ERROR!", Error);
				//return;
			}
			for (size_t i = 0; i < (*_core).CheckedEvents.size() - 1; i++) {
				temp.push_back(Synapse::GetSensivityCauses((*_core).CheckedEvents, i));
			}
			for (size_t i = 0; i < Settings::Size_SCP; i++) ///\bug TODO : ERRORS
				(*_core).Sensivity_List.push_back(temp[Settings::Size_SCP + i]); // Я хз зачем это, но лучше не трогай.
		}

		(*_core).Sensivity_List = temp;
		Current_sensivity = (*_core).Sensivity_List[(*_core).Sensivity_List.size() - 1];
#pragma endregion 
*/ 

		if (true) {
			if (false) {
				double tone = 8, volime = 13;
				for (size_t t = 0; t < 3; t++)
				{
					(*_core).UncheckedEvents.push_back(Consequence(Sound("test", tone, volime)));

					(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));

					for (size_t tt = 0; tt < 10; tt++)
						(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));
				}
			}
			if (false) {
				for (size_t t = 0; t < 30; t++) {
					(*_core).UncheckedEvents.push_back(Consequence("door", Action(Sound("door")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence("is", Action(Sound("is")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence("open", Action(Sound("open")), 1, 13, 0));

					(*_core).UncheckedEvents.push_back(Consequence("close", Action(Sound("close")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence("door", Action(Sound("door")), 1, 13, 0));

					(*_core).UncheckedEvents.push_back(Consequence("door", Action(Sound("door")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence("is", Action(Sound("is")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence("close", Action(Sound("close")), 1, 13, 0));

					(*_core).UncheckedEvents.push_back(Consequence("open", Action(Sound("open")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence("door", Action(Sound("door")), 1, 13, 0));
				}
			}
			if (false) {
				for (size_t t = 0; t < 10; t++) {
					(*_core).UncheckedEvents.push_back(Consequence("open", Action(Sound("open")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence("close", Action(Sound("close")), 1, 13, 0));
					(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));
				}
			}
			if (true) {
				double good = 20;
				for (size_t t = 0; t < 3; t++) {
					(*_core).UncheckedEvents.push_back(Consequence("hello", Action(Sound("hello")), 1, 0, good));
					(*_core).UncheckedEvents.push_back(Consequence("monika", Action(Sound("monika")), 1, 0, good));

					(*_core).UncheckedEvents.push_back(Consequence("hello", Action(Sound("hello")), 1, 0, good));
					(*_core).UncheckedEvents.push_back(Consequence("nikita", Action(Sound("nikita")), 1, 0, good));

					for (size_t tt = 0; tt < 10; tt++)
						(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));
				}
				for (size_t tt = 0; tt < 15; tt++)
					(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));
			}
			if (true) {
				double tone = 10;
				double volime = 15;

				for (size_t t = 0; t < 3; t++)
				{
					(*_core).UncheckedEvents.push_back(Consequence(Sound("how", tone, volime)));
					(*_core).UncheckedEvents.push_back(Consequence(Sound("are", tone, volime)));
					(*_core).UncheckedEvents.push_back(Consequence(Sound("you", tone, volime)));

					(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));

					(*_core).UncheckedEvents.push_back(Consequence(Sound("i", tone, volime)));
					(*_core).UncheckedEvents.push_back(Consequence(Sound("am", tone, volime)));
					(*_core).UncheckedEvents.push_back(Consequence(Sound("okay", tone, volime)));

					for (size_t tt = 0; tt < 10; tt++)
						(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));
				}
			}
			if (true) {
				double tone = 8;
				double volime = 13;

				for (int a = 1; a < 6; a++)
					for (int b = 1; b < 6; b++)
						for (size_t t = 0; t < 3; t++)
						{
							(*_core).UncheckedEvents.push_back(Consequence(Sound(Helper::NumberToWord(a), tone, volime)));
							(*_core).UncheckedEvents.push_back(Consequence(Sound("plus", tone, volime)));
							(*_core).UncheckedEvents.push_back(Consequence(Sound(Helper::NumberToWord(b), tone, volime)));
							(*_core).UncheckedEvents.push_back(Consequence(Sound("equal", tone, volime)));

							(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));

							(*_core).UncheckedEvents.push_back(Consequence(Sound(Helper::NumberToWord(a + b), tone, volime)));

							for (size_t tt = 0; tt < 10; tt++)
								(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));
						}
			}
			for (size_t tt = 0; tt < 10; tt++)
				(*_core).UncheckedEvents.push_back(Consequence(Settings::EmptyName));
		}

		Memory::GetMemory()->LoadStaticMemory();

		Debug::Log("-> The causality core is started!");

		while (Settings::IsActive) {
			if (timer >= 100) { timer = 0; _core->CoreLoad = load_1; load_1 = 0; }
			else timer++;

			_core->size_unchk_ev = _core->UncheckedEvents.size();
			if (_core->size_unchk_ev == 0) Sleep(*delay);
			else { 
			//if ((*_core).UncheckedEvents.size() > 0) {
				_event = (*_core).UncheckedEvents[0]; // Get first _event

				if (_event.name.empty()) {
					Debug::Log("CausalityCore : Unknown ERROR => conseq.name == \"\"!", Error);
					(*_core).UncheckedEvents.erase((*_core).UncheckedEvents.begin()); // System
					_core->size_unchk_ev--;
				}
				else {
					char found = 0;
					if (_event.name != Settings::EmptyName) {
						load_1++;
						// Воспомогательная информация [find counsequence -> get info from consequence]
						found = helpData.Load(_event.name, _event.action.type, false, false, "U_Caus");

						if (found >= 0) {
							//boost::tuple<double, double> hp(0, 0);
							E_ref.EmotionHelpfulness(_event.action, bad, good); // Эмоционально реагируем на событие
							_event.Bad = _event.Bad + bad; _event.Good = _event.Good + good;

							if (found) {//helpData.~Consequence();
								Helper::SummActionConseq(_event, helpData);
								Helper::SimpleSummConseq(_event, helpData);
								_event.meetings = _event.meetings + helpData.meetings;
							}

							Situation.clear();
							Situation = Synapse::GetSensivityCauses((*_core).CheckedEvents);
							Situation += Synapse::GetSensivityOfName(_event.name, _event.self);
							Situation = Synapse::ClearSensivity(Situation);

							if (!_event.self) {
								C_ref.NewEvent(_event, Situation);
								E_ref.AddEvent(_event, Situation);
							}

							if (found) {
								_event.Bad = (_event.Bad + helpData.Bad) / Div;
								_event.Good = (_event.Good + helpData.Good) / Div;
							}
						}
					}
					else if (!_event.self) C_ref.NewEvent(empty_conq, empty_sens);

					if (found >= 0)
						(*_core).CheckedEvents.push_back(_event);						  // System
					(*_core).UncheckedEvents.erase((*_core).UncheckedEvents.begin()); // System
					_core->size_unchk_ev--;

					if (_event.name != Settings::EmptyName && found >= 0) {
						_event.Save();

						//if (!_event.self) 
						//	Current_sensivity += Synapse::GetSensivityOfName(_event.name, false);  // System
						//else{
							//Current_sensivity += Helper::ToUpper(Synapse::GetSensivityOfName(_event.name, false));  // System
						Current_sensivity += Synapse::GetSensivityOfName(_event.name, _event.self);  // System
						if (_event.self)
							Debug::Log("CausalityCore : self _event \"" + _event.name + "\" [" + Current_sensivity + "]");
						//}

						(*_core).Sensivity_List.push_back(Current_sensivity); // System
					}
					else {
						Current_sensivity += std::string(count_word_in_sensiv, '.');						 	  // System
						(*_core).Sensivity_List.push_back(Current_sensivity); // System
					}
				}
				_event.~Consequence();
				helpData.~Consequence();
			}

			/* Пост-процессинг. Здесь мы объединям некоторую информацию и проводим синапсы между нейронами. */

			_core->size_check_ev = _core->CheckedEvents.size();
			if (_core->size_check_ev > ((Settings::Size_SCP * 2) + 1)) {
				Consequence& conq = (*_core).CheckedEvents[Settings::Size_SCP]; // 6-ый element

				if (!conq.name._Equal(Settings::EmptyName)) {
					
					if (load_conq.Load(conq.name, conq.action.type, "C_Caus")) {
						conq.Bad = (conq.Bad + load_conq.Bad) / Div;
						conq.Good = (conq.Good + load_conq.Good) / Div;

						Helper::SimpleSummConseq(conq, load_conq);
						Helper::SummActionConseq(conq, load_conq);
					}

					CausalityCore::CheckedEventsProcessing((*_core).CheckedEvents, Temp_Causes, Temp_Meets);
					/* Обрабатываем следствия устанавливая между ними взаимосвязи и добавляем в них причины. */


					size_temp = Temp_Causes.size();

					if (size_temp != 0) {
						for (auto a : (*_core).Sensivity_List)
							if (a[a.size() - 1] != '.') {
								for (size_t t = 0; t < a.size(); t++)
									if (a[t] == '.') {
										a.erase(a.begin() + t);
										t--;
									}
								clean_sensiv.push_back(a);
								count_sens++;
								if (count_sens == size_temp) break;
							}
						// Избавляемся от точек. (Пустых следствий)

						//Высокая верятность неизвестной ошибки!!!!!!!!!!!!!!!!!

						//====================================================================================
							//Утечка памяти!!!!!!
						//====================================================================================

						//L_ref.EditCauses(Temp_Causes, Temp_Meets, Remove<std::string>(clean_sensiv, Temp_Causes.size()), conq);

						L_ref.EditCauses(Temp_Causes, Temp_Meets, clean_sensiv, conq);
					}

					Temp_Causes.clear();
					Temp_Meets.clear();
					clean_sensiv.clear();
					load_conq.~Consequence();
					count_sens = 0;
					/* Изменяем репутацию причин, которые только что произошли отталкитваясь от следствий в которых они находятся */
				}

				(*_core).CheckedEvents.erase((*_core).CheckedEvents.begin()); // Удаляем первый еэлемент
				_core->size_check_ev--;
				Current_sensivity.erase(Current_sensivity.begin(), Current_sensivity.begin() + count_word_in_sensiv);    // $Удаляем $одну $причину
				(*_core).Sensivity_List.erase((*_core).Sensivity_List.begin());											 // $Удаляем $одну $причину
			}
		}
		if (Settings::CoreDebug) Debug::Log("Processing causality... ");
	}
	void CausalityCore::Start() { Process = std::thread(CausalitySolution, &DelayCPU, this); }
	void CausalityCore::NewEvent(Consequence event, bool debug) {
		if(debug) Debug::Log("CausalityCore::NewEvent = " + std::string(ToString(event.action.type)) + " : " + event.name, Info);
		UncheckedEvents.push_back(event); }
}
