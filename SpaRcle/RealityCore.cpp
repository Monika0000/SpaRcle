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
#include "TCP.h"

namespace SpaRcle {
	RealityCore::RealityCore(int cpuSpeed) {
		core = NULL;
		DelayCPU = cpuSpeed;
		Debug::Log("-> Creating the reality core are successful!"); }
	RealityCore::~RealityCore() {
		if (Process.joinable())
			Process.detach();
		Debug::Log("-> The reality core has completed it's work!", Info); }

	void RealitySolution(int* delay, RealityCore* rcore) {	
		RealityCore& real = *rcore;
		CentralCore& core = *real.core;
		CausalityCore& causal = *core._causality;
		size_t r_timer = 0;
		
		if (false) {
			for (int i = 0; i < 10; i++) {
				causal.UncheckedEvents.push_back(Consequence(Visual("door")));
				causal.UncheckedEvents.push_back(Consequence(Visual("open_door")));
				causal.UncheckedEvents.push_back(Consequence(Visual("himan")));
				causal.UncheckedEvents.push_back(Consequence(Visual("axe")));
				for (int z = 0; z < 10; z++) causal.UncheckedEvents.push_back(Consequence(Settings::EmptyName));
			}
		}
		if (false) { /// Lerning of the dialogs
			std::vector<std::string> lines;
			System::Load("Dialogs\\dialog_3.txt", lines);
			for (size_t t = 0; t < lines.size(); t++)
			{
				if (lines[t] == "[END]") {
					for (size_t e = 0; e < 15; e++)
						causal.UncheckedEvents.push_back(Consequence(Settings::EmptyName));
					continue; }

				for (auto a : Helper::Split(lines[t].substr(2), " ")) {
					for (auto& w : a)
						w = std::tolower(w);
					causal.UncheckedEvents.push_back(Consequence(Sound(Helper::Transliteration(a), 10, 15)));
				}
			}
		}

		Debug::Log("-> The reality core is started!");

		while (Settings::IsActive) {
			Sleep(*delay);

			if (Settings::isMinding)
				if (r_timer >= size_t(500 / (*delay))) {
					///\see causal.UncheckedEvents.push_back(Consequence(Settings::EmptyName)); 
					r_timer = 0;
				}
				else r_timer++;
			if (Settings::CoreDebug) Debug::Log("Processing reality...");
		}
	}
	void RealityCore::Start() { Process = std::thread(RealitySolution, &DelayCPU, this); }

	void RealityCore::DoAction(Action& action, std::string& Situation, std::string helpName) {
		this->core->GetTCP()->Send(action);
		switch (action.type) {
		case AType::Speech : {
			if (Settings::ExternalInteraction) {

			}
			Sound* s = new Sound(action);//+ s->text
			Debug::Log("|RDA| => Speech : " + s->text + "\n\tVol : " + std::to_string(s->volime) + "\n\tTone : " + std::to_string(s->tone), DType::Mind);
			//this->core->_causality->UncheckedEvents.push_back(Consequence(s, true));
			///\ Отключаем отправку в ядро причинности, ибо теперь это задача симуляции
			delete s;
			break; }
		case AType::Move: {
			//Motion* m = new Motion(action);+ m->part
			Debug::Log("|RDA| => Move : TODO " + helpName, DType::Mind);
			//this->core->_causality->UncheckedEvents.push_back(Consequence(s, true));
			///\ Отключаем отправку в ядро причинности, ибо теперь это задача симуляции
			//delete m;
			break; }
		default:
			Debug::Log("RealityCore::DoAction : Unknown type! \n\tType : " + std::string(ToString(action.type))
				+ "\n\tConseq name : " + helpName
				,Error);
			break; }
		helpName.~basic_string();
	}
}
