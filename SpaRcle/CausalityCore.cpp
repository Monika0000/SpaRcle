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

namespace SpaRcle {
	CausalityCore::CausalityCore(int cpuSpeed)
	{
		core = NULL;
		std::vector<std::string> vector;
		if (System::Load("temp%load\\causality_data.temp", vector, false))
			for (std::string a : vector)
				if (a[0] == 'S') {
					Consequence conseq("");

					bool ok = conseq.Load(a.substr(2), AType::Speech);
					if (ok)
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

		System::Save("causality_data.temp", save, "\\temp%load");

		UncheckedEvents.~vector();
		CheckedEvents.~vector();
		Debug::Log("-> The causality core has completed it's work!", Info);
	}
 
	inline bool SpaRcle::CausalityCore::CheckedEventsProcessing(std::vector<Consequence>& Ref_ev, std::vector<std::string>&Temp_Causes, std::vector<int>& Temp_Meets, const bool Diagnostic)
	{
		// Здесь все вроде-бы готово. Пока-что лучше больше не трогать данный участок кода.\
		
		//Debug::Log("CausalityCore::CheckedEventsProcessing : work");

		Consequence& conq = Ref_ev[Settings::Size_SCP]; // 6-ый element

		for (size_t t = 0; t < Settings::Size_SCP; t++) {
			/* Здесь мы суммируем и добавляем в нейрон синапсы, ведущие к другим нейронам */

			if (Ref_ev[t].name == Settings::EmptyName) {
				continue; }
			
			#pragma region [========== CAUSES =========] 
			std::string C_name; C_name += ToString(Ref_ev[t].action.type)[0];
			C_name += "/" + Ref_ev[t].name; // Генерируем имя
			
			//continue;

			int С_index = Helper::IndexOfSynapse(conq.Causes, C_name); // Ищем схожую причину
			if (С_index == -1) {
				// Добавляем синапс на нейрон являющийся причиной данного следствия (нейрона)
				conq.Causes.push_back(boost::tuple<std::string, int, double>(C_name, 1, Ref_ev[t].GetSummHP()));

				Temp_Causes.push_back(C_name); // Добавляем причину для дальнейшей обработки
				Temp_Meets.push_back(1); // Добавляем колличество встреч данной причины

				if(Settings::EventsProcessigDebug) Debug::Log("CausalityCore : add cause \"" + C_name + "\" to \"" + conq.name + "\"");
			}
			else {
				if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : summ cause \"" + conq.name + "\" with \"" + C_name + "\"");
				// Summ consequences
				conq.Causes[С_index].get<2>() = (conq.Causes[С_index].get<2>() + Ref_ev[t].GetSummHP()) / 1.5f; /* Суммируем полезность */
				conq.Causes[С_index].get<1>()++; // Увеличиваем количество встреч данного события на 1
				//std::cout << conq.Causes[С_index].get<1>() << std::endl;

				Temp_Causes.push_back(C_name); // Добавляем причину для дальнейшей обработки
				Temp_Meets.push_back(conq.Causes[С_index].get<1>()); // Добавляем колличество встреч данной причины
			}
			#pragma endregion
		}
		/* Для начала узнаем, что является причиной этого следствия */
		//if(false)
		for (size_t t = 0; t < Settings::Size_SCP + 1; t++) {
			#pragma region [ ========= SYNAPSE ========= ]
			Consequence& ref_will = Ref_ev[t + Settings::Size_SCP + 1];

			if (ref_will.name == Settings::EmptyName)
				continue;

			std::string Sensivity = Helper::GetSensivityCauses(Ref_ev, t + Settings::Size_SCP);
			Sensivity = Helper::ClearSensivity(Sensivity);
			/* Получаем то, что описывает нашу текущую ситуацию относительно будущего следствия */

			std::string S_name; S_name += ToString(ref_will.action.type)[0];
			S_name += "/"; S_name += ref_will.name; // Генерируем имя

			if (ref_will.GetSummHP() > 0) { // Ищем синапсы, чтобы можно было продолжить действия после этого нейрона
				int S_index = Helper::IndexOfSynapse(conq.Synapses, S_name); // Получаем индекс
				if (S_index == -1) { 
					// Добавляем синапс на новый нейрон, который нужно будет выпоолнить после этого
					//conq.Synapses.push_back(boost::tuple<std::string, std::string, double>(S_name, Sensivity, ref_will.GetSummHP())); // ======== Добавляем новый снапс ========
					conq.Synapses.push_back(boost::tuple<std::string, double>(S_name, ref_will.GetSummHP())); // ======== Добавляем новый снапс ========
					if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : add synapse \"" + S_name + "\" to \"" + conq.name + "\"");
				}
				else { // Summ consequences (synapses)
					if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : (TODO) summ synapse \"" + conq.name + "\" with \"" + S_name + "\"");
					//conq.Synapses[S_index].get<2>() = (conq.Synapses[S_index].get<2>() + ref_will.GetSummHP()) / 1.5f; // Суммируем полезность
					Helper::SummHpSyns(conq.Synapses[S_index], ref_will.GetSummHP());
					//conq.Synapses[S_index].get<1>() = Helper::SummSensivity(conq.Synapses[S_index].get<1>(), Sensivity);
					//Helper::SummSensivity(conq.Synapses[S_index].get<1>(), Sensivity);
					Helper::SummSensivity(conq, S_index, Sensivity, ECom::Synp);
				}
			}

			#pragma region [ ========= PERHAPSWILL ========= ]
			Helper::FindAndSummSensiv(conq, S_name, Sensivity, ref_will.GetSummHP());
			/*
			int P_index = Helper::IndexOfSynapse(conq.PerhapsWill, S_name);
			if (P_index == -1) {
				// Добавляем синапс на новое возможное следствие
				//Debug::Log("\"" + Sensivity + "\"");
				Debug::Log(conq.PerhapsWill.size());
				conq.PerhapsWill.push_back(boost::tuple<std::string, std::string, double, int>(S_name, Sensivity, ref_will.GetSummHP(), 1));
				if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : add perhaps \"" + S_name + "\" to \"" + conq.name + "\"");
				//Debug::Log(conq.PerhapsWill.size());
			}
			else {
				// Summ consequences (PERHAPSWILL)
				if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : summ perhaps \"" + conq.name + "\" with \"" + S_name + "\"");
				conq.PerhapsWill[P_index].get<2>() = (conq.PerhapsWill[P_index].get<2>() + ref_will.GetSummHP()) / 1.5f; // Суммируем полезность 
				//conq.PerhapsWill[P_index].get<1>() = "ADD";

				// TODO : Нужно складывать чувствительность!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//conq.PerhapsWill[P_index].get<1>() = Helper::SummSensivity(conq.PerhapsWill[P_index].get<1>(), Sensivity);
				//Helper::SummSensivity(conq.PerhapsWill[P_index].get<1>(), Sensivity);
				Helper::SummSensivity(conq, P_index, Sensivity, ECom::PerhWill);

				//for (size_t i = 0; i < Situations.size(); i++) std::cout << Situations[i] << std::endl;
				//Debug::Log(Sensivity);
				conq.PerhapsWill[P_index].get<3>()++; //Increment
			} 
			*/
			#pragma endregion

			#pragma endregion
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

		std::string Current_sensivity;

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
				temp.push_back(Helper::GetSensivityCauses((*_core).CheckedEvents, i));
			}
			for (size_t i = 0; i < Settings::Size_SCP; i++)
				(*_core).Sensivity_List.push_back(temp[Settings::Size_SCP + i]); // Я хз зачем это, но лучше не трогай.
		}
		
		(*_core).Sensivity_List = temp;
		Current_sensivity = (*_core).Sensivity_List[(*_core).Sensivity_List.size() - 1];
		#pragma endregion 

		if (true) {
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

				for (size_t t = 0; t < 6; t++)
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

		while (true)
		{
			//break;
			Sleep(*delay);
			try {
				if ((*_core).UncheckedEvents.size() > 0)
				{
					Consequence event = (*_core).UncheckedEvents[0]; // Get first event

					if (event.name != Settings::EmptyName) {
						Consequence helpData; // Воспомогательная информация [find counsequence -> get info from consequence]
						bool found = helpData.Load(event.name, event.action.type, false, false);

						boost::tuple<double, double> hp(0, 0);
						hp = E_ref.EmotionHelpfulness(event.action); // Эмоционально реагируем на событие

						event.Bad = event.Bad + hp.get<0>();
						event.Good = event.Good + hp.get<1>();

						if (false) {
							std::cout << "	n:" << helpData.name << std::endl;
							std::cout << "	m:" << helpData.meetings << std::endl;
							std::cout << "	h:bad:" << helpData.Bad << std::endl;
							std::cout << "	h:good:" << helpData.Good << std::endl;
							std::cout << std::endl;
						}
						if (false) {
							std::cout << "	n:" << event.name << std::endl;
							std::cout << "	m:" << event.meetings << std::endl;
							std::cout << "	h:bad:" << event.Bad << std::endl;
							std::cout << "	h:good:" << event.Good << std::endl;
							std::cout << std::endl;
						}

						if (!found)	helpData.~Consequence();
						else {
							//event = helpData + event; failure

							///event = event + helpData;

							Helper::SummActionConseq(event, helpData);
							Helper::SimpleSummConseq(event, helpData);
							event.meetings = event.meetings + helpData.meetings;

							//Debug::Log(event.Causes.size());
							//Debug::Log(helpData.Causes.size());

							if (false) {
								std::cout << "	n:" << event.name << std::endl;
								std::cout << "	m:" << event.meetings << std::endl;
								std::cout << "	h:bad:" << event.Bad << std::endl;
								std::cout << "	h:good:" << event.Good << std::endl;
								//std::cout << "t:" << ToString(event.action.type) << std::endl;
								//std::cout << "sd:text:" << event.action.GetSound().text << std::endl;
								//std::cout << "sd:tone:" << event.action.GetSound().tone << std::endl;
								//std::cout << "sd:volime:" << event.action.GetSound().volime << std::endl;
							}
						}

						//if (event.name != Settings::EmptyName) {
							// Решаем, что с этим следствием делать.
						std::string Situation = Helper::GetSensivityCauses((*_core).CheckedEvents);
						//Debug::Log(Situation);

						Situation += Helper::GetSensivityOfName(event.name);// event.name[0];
						//Debug::Log(Situation);

						//Situation += Helper::GetSensivityCauses((*_core). CheckedEvents);
						Situation = Helper::ClearSensivity(Situation);
						//Debug::Log(event.name + " " + Helper::GetSensivityCauses((*_core). CheckedEvents));
						C_ref.NewEvent(event, Situation);
						//}

						if (found) {
							event.Bad = (event.Bad + helpData.Bad) / Div;
							event.Good = (event.Good + helpData.Good) / Div;
						}
					}
					else C_ref.NewEvent(Consequence(Settings::EmptyName), Settings::EmptyName);

					(*_core).CheckedEvents.push_back(event);						  // System
					(*_core).UncheckedEvents.erase((*_core).UncheckedEvents.begin()); // System

					if (event.name != Settings::EmptyName) {
						event.Save();

						Current_sensivity += Helper::GetSensivityOfName(event.name); // event.name[0];					  // System
						(*_core).Sensivity_List.push_back(Current_sensivity); // System
					}
					else
					{
						Current_sensivity += std::string(count_word_in_sensiv,'.');						 	  // System
						(*_core).Sensivity_List.push_back(Current_sensivity); // System
					}
				}

				/* Пост-процессинг. Здесь мы объединям некоторую информацию и проводим синапсы между нейронами. */
				//if(false)
				if ((*_core).CheckedEvents.size() > ((Settings::Size_SCP * 2) + 1))
				{
					Consequence& conq = (*_core).CheckedEvents[Settings::Size_SCP]; // 6-ый element

					if (conq.name != Settings::EmptyName) {
						std::vector<std::string> Temp_Causes;
						std::vector<int> Temp_Meets;

						Consequence load;
						if (load.Load(conq.name, conq.action.type))
						{
							conq.Bad = (conq.Bad + load.Bad) / Div;
							conq.Good = (conq.Good + load.Good) / Div;

							Helper::SimpleSummConseq(conq, load);

							Helper::SummActionConseq(conq, load);
						}

						//for (auto& a : (*_core).CheckedEvents)
						//	std::cout << a.name << std::endl;
						
						///std::cout << std::endl;
						CausalityCore::CheckedEventsProcessing((*_core).CheckedEvents, Temp_Causes, Temp_Meets);
						/* Обрабатываем следствия устанавливая между ними взаимосвязи и добавляем в них причины. */

						std::vector<std::string> clean_sensiv;
						for (auto a : (*_core).Sensivity_List)
							if (a[a.size() - 1] != '.') {
								for (size_t t = 0; t < a.size();t++)
									if (a[t] == '.') 
									{
										a.erase(a.begin() + t);
										t--;
									}
								clean_sensiv.push_back(a);
							}
						// Избавляемся от точек. (Пустых следствий)
						
						//Высокая верятность неизвестной ошибки!!!!!!!!!!!!!!!!!
						L_ref.EditCauses(Temp_Causes, Temp_Meets, Remove<std::string>(clean_sensiv, Temp_Causes.size()), conq);
						/* Изменяем репутацию причин, которые только что произошли отталкитваясь от следствий в которых они находятся */
					}

					(*_core).CheckedEvents.erase((*_core).CheckedEvents.begin()); // Удаляем первый еэлемент
					/// \bug Current_sensivity.erase(0); //Удаляем одну причину (Полностью очищает строку)
					Current_sensivity.erase(Current_sensivity.begin(), Current_sensivity.begin()+ count_word_in_sensiv);    // $Удаляем $одну $причину
					(*_core).Sensivity_List.erase((*_core).Sensivity_List.begin());											// $Удаляем $одну $причину
				}
			}
			catch(...) {
				//std::cout << e.bstrDescription << std::endl;
				//std::cout << e.scode << std::endl;
				Debug::Log("CausalityCore : An exception has occured!", Error);
				break;
			}
			if (Settings::CoreDebug)
				Debug::Log("Processing causality... ");
		}
	}

	void CausalityCore::Start()
	{
		//union d { int f = 5; };
		Process = std::thread(CausalitySolution, &DelayCPU, this);
		Debug::Log("-> The causality core is started!");
	}

	void CausalityCore::NewEvent(Consequence event) {
		Debug::Log("CausalityCore::NewEvent = " + std::string(ToString(event.action.type)) + " : " + event.name, Info);
		UncheckedEvents.push_back(event);
		// + "\n\tBad = " + std::to_string(event.Bad)
	}
}
