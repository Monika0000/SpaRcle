#include "pch.h"
#include "Action.h"
#include "Sound.h"
#include "Helper.h"

namespace SpaRcle {
	Action::Action() { type = Undefined; }

	Action::Action(Sound sound)
	{
		this->sound = sound;
		this->type = Speech; }
	Action::Action(Visual visual)
	{
		this->visual = visual;
		this->type = VisualData; }

	Action::~Action()
	{
		sound.~Sound();
		visual.~Visual(); }

	std::string Action::GetSaveData(Action * action)
	{
		std::string data;
		data += "t:"; data += ToString(action->type); data += "\n";

		switch (action->type)
		{
		case AType::Undefined:
		{
			break;
		}
		case AType::Speech:
		{
			data += "sd:" + action->sound.text + "\n";
			data += "pr:" + Helper::Remove(std::to_string(action->sound.tone), Settings::SaveNumbers) + ";" + Helper::Remove(std::to_string(action->sound.volime),
				Settings::SaveNumbers);
			break;
		}
		case AType::VisualData:
		{
			data += "vs:" + action->visual.tag;
			break;
		}
		default:
			Debug::Log("Action::GetSaveData : Unknown type!", Error);
			return "[Error]";
		}

		return data;
	}
	std::string Action::GetSaveData() {
		return GetSaveData(this);
	}

	bool Action::ApplyLine(std::string line)
	{
		int n;
		std::string pref = ReadUpToChar(line, ':', n);
		std::string post = line.substr(n);
		SWITCH(pref)
		{
	///^SOUND
			CASE("sd") :{ sound.text = post; break; }
			CASE("pr") :{
				int n2; std::string t = ReadUpToChar(post, ';', n2);
				sound.tone = std::stof(t);
				sound.volime = std::stof(post.substr(n2));
				break; }
	///^SOUND


	///^VISUAL
			CASE("vs") :{ visual.tag = post; break; }
	///^VISUAL

		DEFAULT: {
			Debug::Log("SpaRcle::Action::ApplyLine::SWITCH = WARNING : Uncorrect char! \n\tLine : "  + line +
				"\n\tSymbol : " + pref, Warning);
			return false;
			}
		}
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