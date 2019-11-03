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
		// ����� ��� �����-�� ������. ����-��� ����� ������ �� ������� ������ ������� ����.
		Consequence& conq = Ref_ev[Settings::Size_SCP]; // 6-�� element

		for (size_t t = 0; t < Settings::Size_SCP; t++) {
			/* ����� �� ��������� � ��������� � ������ �������, ������� � ������ �������� */
			if (Ref_ev[t].name == Settings::EmptyName)  continue; 
			
			#pragma region [========== CAUSES =========] 
			std::string C_name; C_name += ToString(Ref_ev[t].action.type)[0];
			C_name += "/" + Ref_ev[t].name; // ���������� ���
			
			if (int C_index = Synapse::IndexOfSynapse(conq.Causes, C_name); C_index == -1) { // ���� ������ �������
				// ��������� ������ �� ������ ���������� �������� ������� ��������� (�������)
				conq.Causes.push_back(boost::tuple<std::string, int, double>(C_name, 1, Ref_ev[t].GetSummHP()));

				Temp_Causes.push_back(C_name); // ��������� ������� ��� ���������� ���������
				Temp_Meets.push_back(1); // ��������� ����������� ������ ������ �������

				if(Settings::EventsProcessigDebug) Debug::Log("CausalityCore : add cause \"" + C_name + "\" to \"" + conq.name + "\"");
			}
			else {
				if (Settings::EventsProcessigDebug) Debug::Log("CausalityCore : summ cause \"" + conq.name + "\" with \"" + C_name + "\"");
				conq.Causes[C_index].get<2>() = (conq.Causes[C_index].get<2>() + Ref_ev[t].GetSummHP()) / 1.5f; /* ��������� ���������� */
				conq.Causes[C_index].get<1>()++; // ����������� ���������� ������ ������� ������� �� 1

				Temp_Causes.push_back(C_name); // ��������� ������� ��� ���������� ���������
				Temp_Meets.push_back(conq.Causes[C_index].get<1>()); // ��������� ����������� ������ ������ �������
			}
			#pragma endregion
		}
		/* ��� ������ ������, ��� �������� �������� ����� ��������� */
		for (size_t t = 0; t < Settings::Size_SCP + 1; t++) {
			#pragma region [ ========= SYNAPSE ========= ]
			Consequence& ref_will = Ref_ev[t + Settings::Size_SCP + 1];

			if (ref_will.name == Settings::EmptyName) continue;

			std::string Sensivity = Synapse::GetSensivityCauses(Ref_ev, t + Settings::Size_SCP);
			Sensivity = Synapse::ClearSensivity(Sensivity);
			/* �������� ��, ��� ��������� ���� ������� �������� ������������ �������� ��������� */

			std::string S_name; S_name += ToString(ref_will.action.type)[0];
			S_name += "/"; S_name += ref_will.name; // ���������� ���

			if (ref_will.GetSummHP() > 0) { // ���� �������, ����� ����� ���� ���������� �������� ����� ����� �������
				int S_index = Synapse::IndexOfSynapse(conq.Synapses, S_name); // �������� ������
				if (S_index == -1) { 
					// ��������� ������ �� ����� ������, ������� ����� ����� ���������� ����� �����
					conq.Synapses.push_back(boost::tuple<std::string, double>(S_name, ref_will.GetSummHP())); // ======== ��������� ����� ����� ========
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
		/* � ������  */

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

		/// \see � ������ ������� �� ���������� ��������� ��������, ��� ������� ��������� ������ ������ ��������.

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
			for (size_t i = 0; i < Settings::Size_SCP; i++)
				(*_core).Sensivity_List.push_back(temp[Settings::Size_SCP + i]); // � �� ����� ���, �� ����� �� ������.
		}
		
		(*_core).Sensivity_List = temp;
		Current_sensivity = (*_core).Sensivity_List[(*_core).Sensivity_List.size() - 1];
		#pragma endregion 

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

		while (true)
		{
			if (!Settings::IsActive) break;
			if((*_core).UncheckedEvents.size() == 0)
				Sleep(*delay);

			if ((*_core).UncheckedEvents.size() > 0)
			{
					Consequence event = (*_core).UncheckedEvents[0]; // Get first event

					if (event.name != Settings::EmptyName) {
						Consequence helpData; // ���������������� ���������� [find counsequence -> get info from consequence]
						bool found = helpData.Load(event.name, event.action.type, false, false);

						boost::tuple<double, double> hp(0, 0);
						hp = E_ref.EmotionHelpfulness(event.action); // ������������ ��������� �� �������

						event.Bad = event.Bad + hp.get<0>();
						event.Good = event.Good + hp.get<1>();

						if (!found)	helpData.~Consequence();
						else {
							Helper::SummActionConseq(event, helpData);
							Helper::SimpleSummConseq(event, helpData);
							event.meetings = event.meetings + helpData.meetings;
						}

						std::string Situation = Synapse::GetSensivityCauses((*_core).CheckedEvents);
						Situation += Synapse::GetSensivityOfName(event.name);
						Situation = Synapse::ClearSensivity(Situation);
						C_ref.NewEvent(event, Situation);

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

						Current_sensivity += Synapse::GetSensivityOfName(event.name);  // System
						(*_core).Sensivity_List.push_back(Current_sensivity); // System
					}
					else
					{
						Current_sensivity += std::string(count_word_in_sensiv,'.');						 	  // System
						(*_core).Sensivity_List.push_back(Current_sensivity); // System
					}
				}

				/* ����-����������. ����� �� ��������� ��������� ���������� � �������� ������� ����� ���������. */
				
				if ((*_core).CheckedEvents.size() > ((Settings::Size_SCP * 2) + 1))
				{
					Consequence& conq = (*_core).CheckedEvents[Settings::Size_SCP]; // 6-�� element

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

						CausalityCore::CheckedEventsProcessing((*_core).CheckedEvents, Temp_Causes, Temp_Meets);
						/* ������������ ��������� ������������ ����� ���� ����������� � ��������� � ��� �������. */

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
						// ����������� �� �����. (������ ���������)
						
						//������� ���������� ����������� ������!!!!!!!!!!!!!!!!!
						L_ref.EditCauses(Temp_Causes, Temp_Meets, Remove<std::string>(clean_sensiv, Temp_Causes.size()), conq);
						/* �������� ��������� ������, ������� ������ ��� ��������� ������������� �� ��������� � ������� ��� ��������� */
					}

					(*_core).CheckedEvents.erase((*_core).CheckedEvents.begin()); // ������� ������ ��������
					Current_sensivity.erase(Current_sensivity.begin(), Current_sensivity.begin() + count_word_in_sensiv);    // $������� $���� $�������
					(*_core).Sensivity_List.erase((*_core).Sensivity_List.begin());											// $������� $���� $�������
				}
			}
			if (Settings::CoreDebug)
				Debug::Log("Processing causality... ");
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
