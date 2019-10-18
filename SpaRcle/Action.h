#pragma once
#include <string>
#include "Sound.h"
#include "Visual.h"
#include "Settings.h"
#include "Debug.h"

namespace SpaRcle {
	enum AType
	{
		Undefined, Speech, Move, VisualData
	};

	inline const char* ToString(AType a)
	{
		switch (a)
		{
		case Undefined:   return "Undefined";
		case Speech:   return "Speech";
		case Move: return "Move";
		case VisualData: return "Visual";
		default:      return "[Unknown]";
		}
	}
	inline const AType ToAType(char c) {
		switch (c)
		{
		case 'S':
			return Speech;
		case 'V':
			return VisualData;
		default:
			//Debug::Log("ToAType : Unknown type! \"" + std::string(1, c) + "\"");
			return Undefined;
		}
	}

	struct Action
	{
	public:
		Action();
		Action(Sound sound);
		Action(Visual visual);
		~Action();

		//bool Save(std::string path);
		//static bool Save(std::string path, Action& action);

		static std::string GetSaveData(Action* action);
		std::string GetSaveData();

		bool ApplyLine(std::string line); // Only for loading consequence!

		AType type;

		operator Sound()
		{
			if (type == AType::Speech)
				return sound;
			else 
				Debug::Log("Action::GetSound() = ERROR : trying get to type \"Sound\", where has : " + std::string(ToString(type)) + "!", Error);

			return Sound();
		}
		operator Visual()
		{
			if (type == AType::VisualData)
				return visual;
			else
				Debug::Log("Action::GetVisualData() = ERROR : trying get to type \"VisualData\", where has : " + std::string(ToString(type)) + "!", Error);

			return Visual();
		}
	private:
		Sound sound;
		Visual visual;
		/* [reality data container] */
	};
}