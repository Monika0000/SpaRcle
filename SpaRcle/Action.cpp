#include "pch.h"
#include "Action.h"
#include "Sound.h"
#include "Helper.h"

namespace SpaRcle {
	Action::Action() { type = AType::Undefined; }

	Action::Action(Sound sound) {
		this->sound = sound;
		this->type = AType::Speech; }
	Action::Action(Motion motion) {
		this->motion = motion;
		this->type = AType::Move;
	}
	Action::Action(Visual visual) {
		this->visual = visual;
		this->type = AType::VisualData; }

	Action::~Action() {
		this->sound.~Sound();
		this->visual.~Visual(); 
		this->motion.~Motion(); }

	void Action::Save(std::string path) {
		std::ofstream fout; path = Settings::SysDir + "\\Commands\\" + path + ".act";
		fout.open(path);
		Debug::Log(path);

		if (!fout.is_open())
			Debug::Log("Action::Save = Cant't saving file! \n\tPath : " + path, Error);

		fout << this->GetSaveData() << std::endl;
		fout.close(); }

	std::string Action::GetSaveData(Action * action) {
		std::string data;
		data += "t:"; data += ToString(action->type); data += "\n";

		switch (action->type) {
		case AType::Undefined: {
			Debug::Log("Action::GetSaveData : trying save undefined action!", Error);
			break;
		}
		case AType::Speech: {
			data += "sd:" + action->sound.text + "\n";
			data += "pr:" + Helper::Remove(std::to_string(action->sound.tone), Settings::SaveNumbers) + ";" + Helper::Remove(std::to_string(action->sound.volime),
				Settings::SaveNumbers);
			break;
		}
		case AType::VisualData: {
			data += "vs:" + action->visual.tag; // visual
			break;
		}
		case AType::Move: {
			data += "pt:" + action->motion.part + "\n"; // part
			data += "vl:" + std::to_string(action->motion.value); // value
			break;
		}
		default:
			Debug::Log("Action::GetSaveData : Unknown type!", Error);
			return "[Error]"; }

		return data; }
	std::string Action::GetSaveData() { return GetSaveData(this); }

	bool Action::ApplyLine(std::string line) {
		short n;
		std::string pref = ReadUpToChar(line, ':', n);
		std::string post = line.substr(n);
		SWITCH(pref)
		{
	///^SOUND
			CASE("sd") :{ sound.text = post; break; }
			CASE("pr") :{
				short n2; std::string t = ReadUpToChar(post, ';', n2);
				sound.tone = std::stof(t);
				sound.volime = std::stof(post.substr(n2));
				t.clear(); t.~basic_string();
				break; }
	///^SOUND


	///^VISUAL
			CASE("vs") :{ visual.tag = post; break; }
	///^VISUAL

	///^MOTION
			CASE("pt") :{ motion.part = post; break; }
			CASE("vl") :{ motion.value = std::stof(post); break; }
	///^MOTION
		DEFAULT: {
			Debug::Log("SpaRcle::Action::ApplyLine::SWITCH = WARNING : Uncorrect char! \n\tLine : "  + line +
				"\n\tSymbol : " + pref, Warning);
			return false;
			}
		}
		pref.clear(); pref.~basic_string();
		post.clear(); post.~basic_string();
		line.clear(); line.~basic_string();
		return true;
	}

	/*
	bool Action::Save(std::string path, Action& action) { // 

		switch (action.type)
		{
		case AType::Speech: {
			//Sound s = action->GetSound();
			Sound s = action;
			Debug::Log("Save A : " + s.text, Log);
			return true;
		}
		default:
			Debug::Log("Action::Save : Unknown type! \"" + path + "\"", Error);
			return false;
		}
	}

	bool Action::Save(std::string path) {
		return Save(path, *this);
	}
	*/
}