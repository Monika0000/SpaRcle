#include "pch.h"
#include "EmotionCore.h"
#include "Debug.h"
#include "Helper.h"
#include "Settings.h"
#include <string>
#include <Windows.h>

namespace SpaRcle {
	EmotionCore::EmotionCore(int cpuSpeed)
	{
		this->core = NULL;
		DelayCPU = cpuSpeed;
		Debug::Log("-> Creating the emotion core are successful!");
	}

	EmotionCore::~EmotionCore()
	{
		if (Process.joinable())
			Process.detach();
		Debug::Log("-> The emotion core has completed it's work!", Info);
	}

	inline void Paradigm_Emotion(double& Bad, double &Good, Sound& s) {
		/* ============= [ Begin Paradigm ] ============= */
		if (s.volime > 22) {	   /* [Need to test] */
			Bad += (double)((s.volime - 18) * 1.6); } // bad
		else if (s.volime < 18 && s.volime > 10) {
			Good += (double)((s.volime - 10) * 1.15); } // good
		else if (s.volime < 10 && s.volime > 0) {
			Bad += (double)((10 - s.volime) / 3); } // bad

		if (s.tone < -10) { /* [TODO] */
			Bad += (double)((s.tone + 10) * 1.4); }
		else if (s.tone < 15 && s.tone > 5) {
			Good += (double)((s.tone - 5) * 1.1);
			Bad -= (double)((s.tone - 5) * 1.2); }
		/* ============== [ End Paradigm ] ============== */
	}

	boost::tuple<double, double> EmotionCore::EmotionHelpfulness(Action & act)
	{
		boost::tuple<double, double> hp(0, 0);

		switch (act.type)
		{
		case AType::Speech: {
			Sound s = act;
			Paradigm_Emotion(hp.get<0>(),hp.get<1>(), s); // Using emotional paradigm
			break; }
		case AType::VisualData: {
			Visual v = act;
			hp.get<1>() = 10;
			/// \TODO Add method mathing helpfulness in visual data
			break; }
		default:
			Debug::Log("EmotionCore::EmotionHelpfulness = ERROR : unknown token \"" + std::string(ToString(act.type)) + "\"", Error);
			break; }

		return hp;
	}

	void EmotionSolution(int* delay, EmotionCore* core) {
		while (true){
			if (!Settings::IsActive) break;
			Sleep(*delay);
			if (Settings::CoreDebug)
				Debug::Log("Processing emotion...");
		}
	}
	
	void EmotionCore::Start()
	{
		Process = std::thread(EmotionSolution, &DelayCPU, this);
		//union d { int f = 5; };
		Debug::Log("-> The emotion core is started!");
	}
}
