#pragma once
#include "Core.h"
#include "Action.h"

namespace SpaRcle
{
	class CentralCore;

	class EmotionCore : public Core
	{
	public:
		EmotionCore(int cpuSpeed = 1500);
		~EmotionCore();

		boost::tuple<double, double> EmotionHelpfulness(Action & act);

		CentralCore* core;

		void Start();
	};
}