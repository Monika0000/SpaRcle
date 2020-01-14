#pragma once
#include "pch.h"
#include "Core.h"
#include "RealityCore.h"
#include "CentralCore.h"
#include "CausalityCore.h"
#include "EmotionCore.h"
#include "LogicalCore.h"
#include "Consequence.h"
#include "windows.h"
#include "Debug.h"
#include <iostream>
#include <thread>
#include "TaskTree.h"

namespace SpaRcle {
	class TCP;

	class CentralCore : public Core {
	private:
		void CentralSolution();
		void ProcessingEvent(Consequence& conseq, std::string& Situation);
		bool DoFindSynapse(Consequence& event, int _index);
		bool DoEventOfSynapse(Consequence& _event, std::string& Situation);
		void CheckMonotoneAndDo(Consequence& _event, std::vector<short>& variants);

		bool mono_wait;
		short mono_deep;
		float mono_per_temp, temp_sim_sup_per;


		std::vector<short> normal_variants;
		std::vector<short> super_variants;
	public:
		RealityCore* _reality;
		CausalityCore* _causality;
		LogicalCore* _logic;
		EmotionCore* _emotion;

		CentralCore(int cpuSpeed = 1500);
		~CentralCore();		

		size_t size_events = 0;
		short CoreLoad;

		TaskTree Tree;
		//std::vector<boost::tuple<Consequence, std::string>> Events;
		std::vector<Consequence> Events_conq;
		std::vector<std::string> Events_sens;

		std::string SE_With_MyActions; // Sensivity events with my actions
		void AddSE(std::string event_name, bool IDoIt);
		//void AddSE();

		void NewEvent(Consequence& event, std::string& Situation);

		void Start();
		void ConnectReality(RealityCore* core);
		void ConnectCausality(CausalityCore* core);
		void ConnectLogic(LogicalCore* core);
		void ConnectEmotion(EmotionCore* core);
		void ConnectTCP(TCP* tcp);

		TCP* GetTCP();
	private:
		TCP* _tcp;
	};
}

	