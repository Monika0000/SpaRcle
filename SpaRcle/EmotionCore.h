#pragma once
#include "Core.h"
#include "Action.h"
#include "Debug.h"
#include "Helper.h"

namespace SpaRcle {
	struct Emotion {
		Emotion();
		~Emotion();
	};

	class CentralCore;

	class EmotionCore : public Core
	{
	public:
		EmotionCore(int cpuSpeed = 1500);
		~EmotionCore();

		void EmotionHelpfulness(Action & act, double& bad, double& good);
		
		std::vector<Consequence> events;
		std::vector<std::string> sensivs;

		Emotion current_em;
		CentralCore* core;
		int MindActivity;
		bool IsSleep;

		void Start();
		void AddEvent(Consequence& conq, std::string& sens);
	};
}