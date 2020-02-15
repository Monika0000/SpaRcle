#pragma once
#include <string>

#include "Settings.h"
#include "Debug.h"

#include "Sound.h"
#include "Visual.h"
#include "Motion.h"

namespace SpaRcle {
	struct Consequence;

	enum class AType {
		Undefined, Speech, Move, VisualData };

	inline const std::string ToPath(char c) {
		switch (c) {
		case 'S': return "Speech";
		case 'V': return "Visual";
		case 'M': return "Move";
		default:
			return "Undefined";
		}
	}
	inline const char* ToString(AType a) {
		switch (a) {
		case AType::Undefined:   return "Undefined";
		case AType::Speech:   return "Speech";
		case AType::Move: return "Move";
		case AType::VisualData: return "Visual";
		default:      return "[Unknown]";
		}
	}

	inline const std::string ToATypeChar(AType a)
	{
		switch (a) {
		case AType::Undefined:   return "U";
		case AType::Speech:   return "S";
		case AType::Move: return "M";
		case AType::VisualData: return "V";
		default: Debug::Log("ToATypeChar : unknown type!");  return "[Unknown]";
		}
	}

	inline const AType ToAType(char c) {
		switch (c) {
		case 'S': return AType::Speech;
		case 'V': return AType::VisualData;
		case 'M': return AType::Move;
		default:
			//Debug::Log("ToAType : Unknown type! \"" + std::string(1, c) + "\"");
			return AType::Undefined;
		}
	}
	
	struct Neuron {
		// M/event;HASHSUMM;value_1;value_2
	public:
		Neuron();
		Neuron(float value_1, float value_2);
		~Neuron();

		bool base = false;

		/* Read only! */
		size_t size;
		//std::string name;
		//std::vector<std::string> syn_name;
		//std::vector<size_t> hash;
		const void Remove(size_t index);

		std::vector<float> value_1;
		std::vector<float> value_2;
		std::vector<std::string> sensitivities;
		std::vector<std::string> data;
	};

	struct Action {
	private:
		inline static const std::string GetHashSumm(std::string& name, std::string& sit) {
			//return std::to_string(std::hash<std::string>{}(name + sit));
			return (name + "-" + sit);
		}
		//std::map<size_t, Neuron> neurons;
	public:
		Action();
		Action(Sound sound);
		Action(Motion sound);
		Action(Visual visual);
		~Action();
		//void Save(std::string path);
		//bool Save(std::string path);
		//static bool Save(std::string path, Action& action);

		//static std::string GetSaveData(Action* action);
		//std::string GetSaveData();

		//bool ApplyLine(std::string line); // Only for loading consequence!

		//static void SaveNeuron(Consequence&conq, std::string& situation);
		//static void SaveNeuron(std::string& name, Action& action, std::string& situation);

		/* После вызова функции, переданный аргумент удаляется */
		bool SetData(Neuron * nr, std::string& sit, Consequence& conq);

		static void SaveNeuron(std::string synapse, Consequence& conq, std::string& PW_situation, std::string Situation, bool Base = false);
		static void SaveNeuron(Consequence& conq, std::string& Situation);

		static Neuron* LoadNeuron(Consequence& conq, std::string& PW_situation);
		static Neuron* LoadNeuron(std::string Synapse, AType type, std::string& PW_situation);

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