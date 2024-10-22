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
#include <unordered_map>

namespace SpaRcle {
	LogicalCore::LogicalCore(int cpuSpeed) {
		mono_nam = std::vector<std::string>(); mono_nam.resize(Settings::Monotone_size);
		mono_sit = std::vector<std::string>(); mono_sit.resize(Settings::Monotone_size);

		core = NULL;
		CoreLoad = 0;
		DelayCPU = cpuSpeed;

		Tasks = std::vector<Task>(); Tasks.resize(0);

		Debug::Log("-> Creating the logical core are successful!"); }
	LogicalCore::~LogicalCore() {
		if (Process.joinable()) Process.detach();
		this->Causes.clear();
		this->mono_nam.clear();
		this->mono_sit.clear();
		Debug::Log("-> The logical core has completed it's work!", Info); }

	void LogicalCore::LoverPrioritySynapse(std::string event_name, size_t bad_syn_index, size_t good_syn_index) {
		this->lower_size++;
		this->lower_names.push_back(event_name);
		this->lower_id_bad.push_back(bad_syn_index);
		this->lower_id_good.push_back(good_syn_index);
	}

	void LogicalCore::DoLowerPriopity(Consequence& conq) {
		for (size_t t = 0; t < lower_size; t++) {
			if (lower_names[t].substr(2) == conq.name) { // ������������� �������!!!!!!!
				//if (conq->Load(lower_names[t].substr(2), ToAType(lower_names[t][0]), "DoLowerPriopity")) {
				size_t bad_syn_index = conq.GetSN_Index(conq.GetPW_Name(lower_id_bad[t]));
				size_t good_syn_index = conq.GetSN_Index(conq.GetPW_Name(lower_id_good[t]));

				std::get<2>(conq.PerhapsWill[lower_id_bad[t]]) = std::get<2>(conq.PerhapsWill[lower_id_bad[t]]) * double(1.0 / 2.3); // 3.0 //6.0
				std::get<2>(conq.PerhapsWill[lower_id_good[t]]) = std::get<2>(conq.PerhapsWill[lower_id_good[t]]) * double(7.0 / 3.6);

				std::get<1>(conq.Synapses[bad_syn_index]) = std::get<1>(conq.Synapses[bad_syn_index]) * double(1.0 / 3.0);
				std::get<1>(conq.Synapses[good_syn_index]) = std::get<1>(conq.Synapses[good_syn_index]) * double(7.0 / 6.0); //6.0  // 3.0

				Debug::Log("LogicalCore::DoLowerPriopity : change priority syanpses in event \"" + conq.name + "\"\n\tBad : "
					+ conq.GetSN_Name(bad_syn_index) + "\n\tGood : "
					+ conq.GetSN_Name(good_syn_index), DType::Info);
				

				//}

				this->lower_names.erase(this->lower_names.begin() + t);
				this->lower_id_bad.erase(this->lower_id_bad.begin() + t);
				this->lower_id_good.erase(this->lower_id_good.begin() + t);

				this->lower_size--; t--;
			}
			//else Debug::Log(lower_names[t] + " " + conq.name);
		}

		conq.Save("DoLowerPriopity");
		//lower_event.erase(lower_event.begin());
	}

	void LogicalCore::LogicalSolution() {
		CentralCore& core = *this->core;
		RealityCore& real = *core._reality;
		CausalityCore& causal = *core._causality;
		char timer = 0, load = 0;

		Debug::Log("-> The logical core is started!");
		size_t& check_ev_size= causal.size_check_ev, uncheck_ev_size=causal.size_unchk_ev;

		while (Settings::IsActive) {
			if (timer >= 100) { timer = 0; this->CoreLoad = load; load = 0; } else timer++;
			try {
				if (check_ev_size <= Settings::Size_SCP * 2 + 1 && uncheck_ev_size == 0 && core.Events_sens.size() == 0) {
					if (this->Causes.size() > 0) {
						load++;
						LogicalCore::CauseReputation(this->Causes[0]); ///\see Get first element

						std::get<0>(this->Causes[0]).clear();
						std::get<1>(this->Causes[0]).clear();
						std::get<2>(this->Causes[0]).clear();
						std::get<4>(this->Causes[0]).clear();
						this->Causes.erase(this->Causes.begin());   /// Delete first element
						this->size_causes--;
					}
				}
				//else if (this->lower_event.size() > 0 && causal.UncheckedEvents.size() == 0){
					//DoLowerPriopity();
				//}
				else Sleep(this->DelayCPU);
			}
			catch (...) { Debug::Log("LogicalSolution : An exception has ocurred!", Error); break; }
			if (Settings::CoreDebug) Debug::Log("Processing logical... ");
		}
	}
	void LogicalCore::Start() { Process = std::thread(&LogicalCore::LogicalSolution, this); /*union d { int f = 5; };*/ }

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
		this->Causes.push_back(std::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, std::string>
			(Causes, Meets, Sensivitys, conq.GetSummHP(), name)); // ��������� ������� � �����
		this->size_causes++;
		name.~basic_string();
		Sensivitys.clear();
		Sensivitys.~vector();
	}
	std::vector<std::string> LogicalCore::DecomposeConsequence(Consequence& conseq) {
		/*
			������������ ��������� �� ������������
			������ : ������ ������� �� :
				��� + ���
				��� + ����
				������ + ����
		*/
		std::string name = Helper::GetNameWithType(conseq);

		std::vector<std::string> decomps;
		for (size_t t = 0; t < conseq.Causes.size(); t++) {
			if (name == conseq.GetCS_Name(t)) { continue; }

			Consequence load;
			if (load.Load(conseq.GetCS_Name(t).substr(2), Helper::GetConseqType(conseq.GetCS_Name(t))) == 1) {
				for (size_t t2 = 0; t2 < load.PerhapsWill.size(); t2++) {

					if (conseq.GetCS_Name(t) == load.GetPW_Name(t2))//.PerhapsWill[t2].get<0>())
					{
						decomps.push_back(conseq.GetCS_Name(t)); break;
					}
				}
			}
			load.~Consequence();
		}

		return decomps;
	}

	bool LogicalCore::CauseReputation(std::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, 
		std::string>& Cause, const bool Diagnostic) /* CauseReputation */ {
		//// ����� ��� ����� �� ������. ������������ ������ ����� ����� ���������.
		if (std::get<0>(Cause).size() == 0 && std::get<1>(Cause).size() == 0 && std::get<2>(Cause).size() == 0) {
			if(Settings::CauseReputationDebug) Debug::Log("LogicalCore::CauseReputation : Size causes equal zero...", Info);
			return false; }

		if (!Helper::SelectionSort(std::get<1>(Cause), std::get<0>(Cause), std::get<2>(Cause))) { // Sorting...
			Settings::IsActive = false; return false; }

		static Consequence loaded;

		unsigned short size = unsigned short(std::get<0>(Cause).size());
		for (unsigned short i = 0; i < size; i++) {
			loaded.~Consequence();

			try {
				unsigned short last_index = size - 1 - i; // ������������ � ����� ������� � ������ (��������). ��� ������ ������������ �� �������� � ��������
				AType t = ToAType(std::get<0>(Cause)[last_index][0]); ///\error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (t != AType::Undefined) { // Error
					if (!loaded.Load(std::get<0>(Cause)[last_index].substr(2), t, true, Diagnostic, "Logic")) {
						if (Diagnostic) return false;
						else continue;
					}
					else { // is all ok
						//for(size_t t = 0; t < loaded.Synapses.size(); t++)
						//	Debug::Log(loaded.GetSN_Name(t) + "\t" + std::to_string(loaded.GetSN_HP(t)));

						///\^DONT ^WORK!!!!!!
						/*
						for (size_t t = 0; t < this->lower_size; t++) {
							Debug::Log("lowerPriority : "+ loaded.name + " == " + lower_names[t], System);
							if (loaded.name == lower_names[t]) {
								size_t bad_syn_index  = loaded.GetSN_Index(loaded.GetPW_Name(lower_id_bad[t]));
								size_t good_syn_index = loaded.GetSN_Index(loaded.GetPW_Name(lower_id_good[t]));

								std::get<2>(loaded.PerhapsWill[lower_id_bad[t]]) = std::get<2>(loaded.PerhapsWill[lower_id_bad[t]]) * double(7.0 / 3.6); //6.0
								std::get<2>(loaded.PerhapsWill[lower_id_good[t]]) = std::get<2>(loaded.PerhapsWill[lower_id_good[t]]) * double(1.0 / 2.3); // 3.0

								std::get<1>(loaded.Synapses[bad_syn_index])  = std::get<1>(loaded.Synapses[bad_syn_index])  * double(7.0 / 6.0); //6.0
								std::get<1>(loaded.Synapses[good_syn_index]) = std::get<1>(loaded.Synapses[good_syn_index]) * double(1.0 / 3.0); // 3.0

								Debug::Log("LogicalCore::CauseReputation : change priority syanpses in event \"" + loaded.name + "\"\n\tBad : " 
									+ loaded.GetSN_Name(bad_syn_index) + "\n\tGood : "
									+ loaded.GetSN_Name(good_syn_index), DType::Info);

								//////////////////////////////////////////////////////////

								this->lower_names.erase(this->lower_names.begin() + t);
								this->lower_id_bad.erase(this->lower_id_bad.begin() + t);
								this->lower_id_good.erase(this->lower_id_good.begin() + t);

								this->lower_size--; t--;
							}
						}
						*/
						/* ���� ���������� ���� �������, ����� �������� ��������� ��� ��������, ������� ���� � ������, � ��� �� �������� ��������� ������. */

					}
				}
				else { Debug::Log("LogicalCore::CauseReputation : Unknown type! \"" + std::get<0>(Cause)[last_index] + "\"", Warning); continue; }

				///\TODO :
				///	�������� ������ ����, ��� ��������� ������� ����� ������ �� ���� ��������� � �������� - �������� ����������.
				///	��� �� ������� �������� ���� ���������� ������ ������� � ���� ���������.
			}
			catch (...) { Debug::Log("LogicalCore::CauseReputation : An exception has occured! [First Block]", Error); }

			unsigned short percent = (unsigned short)((double)size * 0.6f);
			if (size - i > percent) {
				try {
					if (std::get<3>(Cause) < 0) {
						loaded.Bad -= (std::get<3>(Cause) / 3) * (size - i);
						loaded.Good -= loaded.Bad * 0.25f;
						// ��� ���� ����������� ����, ��� ��� ��������� �������� �������� �������, ��� ������� �������� ��� ��������� 
					}
					else if (std::get<3>(Cause) > 0) {
						loaded.Good += (std::get<3>(Cause) / 3) * (size - i);
						loaded.Bad -= loaded.Good * 0.25f; }
					else Debug::Log("LogicalCore::CauseReputation : �������������� ��������... \n\tHelpfulness = 0 \n\tName conseq : " + std::get<4>(Cause), Warning); }
				catch (...) {
					Debug::Log("LogicalCore::CauseReputation : An exception has occured! [Second Block]", Error); }

				try {
					Synapse::FindAndSummSensiv(loaded, std::get<4>(Cause), &std::get<2>(Cause)[i], std::get<3>(Cause), false);
					loaded.meetings++;

					if (!Diagnostic) loaded.Save("Logical"); // Saving
					else Consequence::Save(&loaded, "Logical_diag", true); // Debug-Save
				}
				catch (...) { Debug::Log("LogicalCore::CauseReputation : An exception has occured! [Third Block]", Error); }

				loaded.~Consequence();
			}
			else { loaded.~Consequence(); break; }
		}

		return true;
	}

	bool LogicalCore::GetOpposite(Consequence & opposite, Consequence & ev, bool Diagnostic)
	{
		Helper::SelectionSort(ev.Causes, false);
		bool find = false;
		std::string cause = "";
		for (size_t t = 0; t < ev.Causes.size(); t++) {
			cause = ev.GetCS_Name(t);//.Causes[t].get<0>();
			if (cause.substr(2) == ev.name)
				continue;
			//std::cout << cause << std::endl;
			int m_cs = ev.GetCS_Meet(t); // ��, ������� ��� ����������� ��� ������� ����� ������� ����������.

			// �������� ����� ��������� ����� ����� ������� � ���� ���������,
			// ����� ���������� ���������� ������, �������� ������������ ���� ���������.

			if (opposite.Load(cause.substr(2), Helper::GetConseqType(cause), true, Diagnostic, "Oppos")) {
				//std::cout << ev.name << std::endl;
				std::string name;
				name += ToString(ev.action.type)[0];
				name += "/" + ev.name;

				int index = Helper::IndexOfPerhapsWill(opposite.PerhapsWill, name);
				
				if (index != -1) {

					int m_po = opposite.GetPW_Meet(index);

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
			opposite.action.type = AType::Undefined; }

		return find;
	}

	float LogicalCore::IsMonotone(std::string& name, std::string& situation) {
		//this->temp_hash = std::hash<std::string>{}(situation); //\!Hash !summ
		this->temp_percent = 0;

		this->mono_nam.erase(mono_nam.begin()); this->mono_sit.erase(mono_sit.begin());
		///////////////////////////////////////////////////////////////////////////////
		this->mono_nam.push_back(name); this->mono_sit.push_back(situation);

		//^TODO analyze (foreach)

		for (short s = 0; s < Settings::Monotone_size; s++) {
			if (this->mono_nam[s] == name)
				temp_percent += (50.f / (float)Settings::Monotone_size);

			temp_percent += ((float)Synapse::SimilarityPercentage(mono_sit[s], situation)  / 2.f / (float)Settings::Monotone_size);
		}

		return this->temp_percent;
	}
}