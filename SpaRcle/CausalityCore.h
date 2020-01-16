#pragma once
#include "Core.h"
#include <list>
#include <vector>
#include <iostream>
#include "Consequence.h"

namespace SpaRcle 
{
	class CentralCore;

	class CausalityCore : public Core {
	private:
		void CausalitySolution();

	public:
		CausalityCore(int cpuSpeed = 1500);
		~CausalityCore();

		short CoreLoad;

		size_t size_unchk_ev = 0;
		size_t size_check_ev = 0;

		//int SensitivityInhibitor = 10;
		CentralCore* core;
		std::vector<Consequence> UncheckedEvents = {};
		std::vector<Consequence> CheckedEvents = {};
		std::vector<std::string> Sensivity_List = {};

		std::string Current_sensivity;

		void Start();
		void NewEvent(Consequence event, bool debug = true);

		bool static CheckedEventsProcessing(std::vector<Consequence>& Ref_ev, std::vector<std::string>&Temp_Causes, std::vector<int>& Temp_Meets, Consequence& conq, const bool Diagnostic = false);
	private:
	};
}