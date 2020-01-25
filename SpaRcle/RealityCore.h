#pragma once
#include "Core.h"
#include "Action.h"
#include "Sound.h"

namespace SpaRcle {
	class CentralCore;

	class RealityCore : public Core
	{
	public:
		RealityCore(int cpuSpeed = 1500);
		~RealityCore();
		CentralCore* core;

		void Start();
		void DoAction(Action& action, std::string&Situation, std::string HelpName = "[MISSING]");
	};
}

