#include "pch.h"
#include "RealityCore.h"
#include "CausalityCore.h"
#include "Debug.h"
#include <Windows.h>
#include <string>
#include "CentralCore.h"
#include "Consequence.h"
#include "Helper.h"
#include "Settings.h"

namespace SpaRcle {
	RealityCore::RealityCore(int cpuSpeed)
	{
		core = NULL;
		DelayCPU = cpuSpeed;
		Debug::Log("-> Creating the reality core are successful!");
	}

	RealityCore::~RealityCore()
	{
		if (Process.joinable())
			Process.detach();
		Debug::Log("-> The reality core has completed it's work!", Info);
	}

	void RealitySolution(int* delay, RealityCore* rcore)
	{	
		RealityCore& real = *rcore;
		CentralCore& core = *real.core;
		CausalityCore& causal = *core._causality;
		size_t r_timer = 0;
		
		if (true) { /// Lerning of the dialogs
			std::vector<std::string> lines;
			System::Load("Dialogs\\dialog_1.txt", lines);
		}

		while (true)
		{
			Sleep(*delay);

			if (r_timer >= 500 / (*delay)) {
				causal.UncheckedEvents.push_back(Consequence(Settings::EmptyName));
				r_timer = 0;
			}
			else r_timer++;
			//std::string s = Helper::format() << count;
			//rcore->core->_causality->NewEvent(Consequence("TestEvent " + s, Action()));
			if(Settings::CoreDebug)
				Debug::Log("Processing reality...");
			
		}
	}

	void RealityCore::Start()
	{
		Process = std::thread(RealitySolution, &DelayCPU, this);
		//union d { int f = 5; };
		Debug::Log("-> The reality core is started!");
	}

	void RealityCore::DoAction(Action& action, std::string helpName)
	{
		switch (action.type)
		{
		case AType::Speech : {
			Sound s = action;
			//Sound s = action.GetSound();
			//action.Get(action.type, &s);

			Debug::Log(s.text, DType::Mind);
			break;
			}
		default:
			Debug::Log("RealityCore::DoAction : Unknown type! \n\tType : " + std::string(ToString(action.type))
				+ "\n\tConseq name : " + helpName
				,Error);
			break;
		}
	}
}
