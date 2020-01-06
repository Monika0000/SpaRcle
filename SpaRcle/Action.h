#pragma once
#include <string>
#include "Sound.h"
#include "Visual.h"
#include "Settings.h"
#include "Debug.h"
#include "Motion.h"

namespace SpaRcle {
	enum class AType {
		Undefined, Speech, Move, VisualData };

	inline const char* ToString(AType a)
	{
		switch (a) {
		case AType::Undefined:   return "Undefined";
		case AType::Speech:   return "Speech";
		case AType::Move: return "Move";
		case AType::VisualData: return "Visual";
		default:      return "[Unknown]";
		}
	}
	inline const AType ToAType(char c) {
		switch (c) {
		case 'S':
			return AType::Speech;
		case 'V':
			return AType::VisualData;
		case 'M':
			return AType::Move;
		default:
			//Debug::Log("ToAType : Unknown type! \"" + std::string(1, c) + "\"");
			return AType::Undefined;
		}
	}

	struct Action {
	public:
		Action();
		Action(Sound sound);
		Action(Motion sound);
		Action(Visual visual);
		~Action();
		void Save(std::string path);
		//bool Save(std::string path);
		//static bool Save(std::string path, Action& action);

		static std::string GetSaveData(Action* action);
		std::string GetSaveData();

		bool ApplyLine(std::string line); // Only for loading consequence!

		AType type;

		operator Sound() {
			if (type == AType::Speech) return sound;
			else Debug::Log("Action::GetSound() = ERROR : trying get to type \"Sound\", where has : " + std::string(ToString(type)) + "!", Error);
			return Sound(); }
		operator Visual() {
			if (type == AType::VisualData) return visual;
			else Debug::Log("Action::GetVisualData() = ERROR : trying get to type \"VisualData\", where has : " + std::string(ToString(type)) + "!", Error);
			return Visual(); }
		operator Motion() {
			if (type == AType::Move) return motion;
			else Debug::Log("Action::GetMove() = ERROR : trying get to type \"Move\", where has : " + std::string(ToString(type)) + "!", Error);
			return Motion(); }
	private:
		Sound sound;
		Visual visual;
		Motion motion;
		/* [reality data container] */
	};
}