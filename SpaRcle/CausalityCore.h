#pragma once
#include "Core.h"
#include <list>
#include <vector>
#include <iostream>
#include "Consequence.h"

namespace SpaRcle 
{
	class CentralCore;

	class CausalityCore : public Core
	{
	public:
		CausalityCore(int cpuSpeed = 1500);
		~CausalityCore();

		//int SensitivityInhibitor = 10;
		CentralCore* core;
		std::vector<Consequence> UncheckedEvents = {};
		std::vector<Consequence> CheckedEvents = {};
		std::vector<std::string> Sensivity_List = {};

		void Start();
		void NewEvent(Consequence event);

		bool static CheckedEventsProcessing(std::vector<Consequence>& Ref_ev, std::vector<std::string>&Temp_Causes, std::vector<int>& Temp_Meets, const bool Diagnostic = false);
	private:
	};
}