#pragma once
#include <string>
#include "Sound.h"
#include "Settings.h"
#include "Debug.h"

namespace SpaRcle {
	enum AType
	{
		Undefined, Speech, Move
	};
	inline const char* ToString(AType a)
	{
		switch (a)
		{
		case Undefined:   return "Undefined";
		case Speech:   return "Speech";
		case Move: return "Move";
		default:      return "[Unknown]";
		}
	}

	struct Action
	{
	public:
		Action();
		Action(Sound sound);
		~Action();

		bool Save(std::string path);
		static bool Save(std::string path, Action& action);

		static std::string GetSaveData(Action* action);
		std::string GetSaveData();

		bool ApplyLine(std::string line); // Only for loading consequence!

		AType type;

		//const Sound GetSound();

		operator Sound()
		{
			if (type == AType::Speech)
				return sound;
			else 
				Debug::Log("Action::GetSound() = ERROR : trying get to type \"Sound\", where has : " + std::string(ToString(type)) + "!", Error);

			return Sound();
		}
	private:
		Sound sound;
		/* [reality data container] */
	};
}