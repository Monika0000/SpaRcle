#include "pch.h"
#include "EmotionCore.h"
#include "Debug.h"
#include "Helper.h"
#include "Settings.h"
#include <string>
#include <Windows.h>

namespace SpaRcle {
	EmotionCore::EmotionCore(int cpuSpeed) {
		this->events = std::vector<Consequence>();
		this->sensivs = std::vector<std::string>();
		this->current_em = Emotion();
		this->MindActivity = 0;
		this->IsSleep = false;
		this->core = NULL;
		DelayCPU = cpuSpeed;
		Debug::Log("-> Creating the emotion core are successful!"); }
	EmotionCore::~EmotionCore() {
		if (Process.joinable()) Process.detach();
		Debug::Log("-> The emotion core has completed it's work!", Info); }

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

	void EmotionCore::EmotionHelpfulness(Action & act, double& bad, double& good)
	{
		//boost::tuple<double, double> hp(0, 0);

		switch (act.type)
		{
		case AType::Speech: {
			Sound s = act;
			Paradigm_Emotion(bad, good, s); // Using emotional paradigm
			break; }
		case AType::VisualData: {
			Visual v = act;
			//hp.get<1>() = 10;
			good = 10;
			/// \TODO Add method mathing helpfulness in visual data
			break; }
		default:
			Debug::Log("EmotionCore::EmotionHelpfulness = ERROR : unknown token \"" + std::string(ToString(act.type)) + "\"", Error);
			break; }

		//return hp;
	}

	void EmotionSolution(int* delay, EmotionCore* core) {
		double max_sim = 0, mem = 0; std::vector<short> indexs = std::vector<short>();

		Debug::Log("-> The emotion core is started!");
		while (Settings::IsActive){
			if(core->events.size() == 0) Sleep(*delay);
			else {
				if (Settings::CoreDebug) Debug::Log("Processing emotion...");

				Consequence& conq = core->events[0];
				std::string& sens = core->sensivs[0];

				for (short s = 0; s < conq.PerhapsWill.size(); s++) {
					mem = Synapse::SimilarityPercentage(conq.PerhapsWill[s].get<1>(), sens);
					if (mem > max_sim) { max_sim = mem; indexs.clear(); indexs.push_back(s); }
					else if(mem == max_sim) {
						indexs.push_back(s);
					}
				}

				if (indexs.size() > 0) {
					std::string debug = "EmotionCore : ["+conq.name+"] possible events => ";

					for (auto& a : indexs)
						debug += "\n\t" + conq.PerhapsWill[a].get<0>() + " [" + conq.PerhapsWill[a].get<1>() + "] {"+std::to_string(max_sim)+"}";

					Debug::Log(debug, Info);

					indexs.clear();
				}
				//if (core->events.size() > 0) {
				core->events.erase(core->events.begin()); core->sensivs.erase(core->sensivs.begin()); //}
			}
		}
	}
	
	void EmotionCore::Start() { Process = std::thread(EmotionSolution, &DelayCPU, this); }
	void EmotionCore::AddEvent(Consequence& conq, std::string& sens) { this->events.push_back(conq); this->sensivs.push_back(sens); }

	Emotion::Emotion() { }
	Emotion::~Emotion() { }
}
