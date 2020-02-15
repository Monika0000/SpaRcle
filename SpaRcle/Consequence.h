#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Action.h"
#include "DataTime.h"
#include <tuple>
//#include <boost/tuple/tuple.hpp>

#include "Sound.h"
#include "Visual.h"
#include "Motion.h"

namespace SpaRcle {
	enum class load_mode {
		to_hp, to_cas, to_action, to_all, to_syn, to_meets
	};

	struct Consequence
	{
	public:
		static bool isWrite;
		static bool isRead;

		std::string name;
		int meetings;
		double Bad = 0, Good = 0;
		Action action; /* Multi-value */
		bool self = false;

		inline std::string& GetPW_Name(size_t index) { return std::get<0>(PerhapsWill[index]); };
		inline std::string& GetPW_Sens(size_t index) { return std::get<1>(PerhapsWill[index]); };
		inline double& GetPW_HP(size_t index) { return std::get<2>(PerhapsWill[index]); };
		inline int& GetPW_Meet(size_t index) { return std::get<3>(PerhapsWill[index]); };

		inline std::string& GetCS_Name(size_t index) { return std::get<0>(Causes[index]); };
		inline int& GetCS_Meet(size_t index) { return std::get<1>(Causes[index]); };
		inline double& GetCS_HP(size_t index) { return std::get<2>(Causes[index]); };

		inline std::string& GetSN_Name(size_t index) { return std::get<0>(Synapses[index]); };
		inline double& GetSN_HP(size_t index) { return std::get<1>(Synapses[index]); };
		inline size_t GetSN_Index(const std::string& name) {
			//for (std::tuple<std::string, double>& var : Synapses) {
			for(size_t t = 0; t < Synapses.size(); t++) {
				if (std::get<0>(Synapses[t]) == name)
					return t;
			}
			Debug::Log("GetSN_Index : not found \"" + name + "\" at event = " + name, DType::Error);
		}

		std::vector<std::tuple<std::string, double>> Synapses;			/* This are ref to path Consequence		|	Max = 5   */
		std::vector<std::tuple<std::string, std::string, double, int>> PerhapsWill;			/* This are ref to path Consequence		|	Max = 10  */
		std::vector<std::tuple<std::string, int, double>> Causes;		/* This are ref to path Consequence		|	Max = 15  */

		DataTime EventData; /* Do not save */

		///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
		static bool Save(Consequence* conseq, const std::string region, const bool Diagnostic = false);
		bool Save(const std::string region, bool Diagnostic = false);

		char Load(std::string name, AType atype, bool notFoundIsError, bool Diagnostic, std::string Block = "?", load_mode mode = load_mode::to_all);
		char Load(std::string name, AType atype, std::string Block = "?", load_mode mode = load_mode::to_all);
	private:
		char LoadFile(std::string name, AType atype, bool notFoundIsError, bool Diagnostic, std::string Block = "?", load_mode mode = load_mode::to_all);
	public:
		void Set(const Consequence& con);

		Consequence();
		//Consequence(const Consequence &con);
		Consequence(std::string name);
		Consequence(std::string name, AType atype, const std::string Block);
		Consequence(Sound speech, bool self = false, float hp = 0);
		Consequence(Visual visual);
		Consequence(Motion move);
		Consequence(std::string name, Action action);
		Consequence(std::string name, Action action, int meets);
		Consequence(std::string name, Action action, int meets, double Bad, double Good);
		~Consequence();

		double GetSummHP() { 
			if (Good >= 0 && Bad >= 0)
				return Good - Bad;
			else if (Good > 0 && Bad < 0)
				return Good - Bad;
			else if (Good < 0 && Bad > 0)
				return Bad - Good;
			else if (Good < 0 && Bad < 0)
				return Good - Bad;
			else
				Debug::Log("Consequence::GetSummHP() : unknown situation!\n\tGood = " + std::to_string(Good) + "\n\tBad = " + std::to_string(Bad), Error);
		} // Info : change  "-" to "+" (Good - Bad)

		/*
		double GetSummHP() {
			if (Good > 0 && Bad > 0)
				return Good - Bad;
			else
				return Good + Bad;
		} // Info : change  "-" to "+" (Good - Bad)
		*/
		///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

		bool operator==(Consequence& right) {
			bool n = false;
			if (name == right.name) n = true;
			return n; }
		bool operator!=(Consequence& right) { return !(*this == right); }
		bool operator>(Consequence& right) { return (EventData > right.EventData); }
		bool operator<(Consequence& right) { return (EventData < right.EventData); }
		//friend const Consequence operator+(Consequence& left, Consequence& right);
	};

	inline const char *conseq_info =
		"\n  Данный класс можно расценивать в качестве одного логического нейрона. \n"

		"  Synapses (Синапс) - Указывает на то, что нужно делать после этого события. \n(Может быть рекурсия, и це норма) \n"
		"  Построение синапсов всегда идет в пользу главной задаче. \nТо-есть, они не могут хранить инструкции о том, как сделать плохо самому себе. \n"

		"  PerhapsWill (Возможно произойдет) - То, чего следует ожидать после этого. \n(Данное событие чаще происходит не по нашей воле) \n" 

		"  Causes (Причины) - То, что было до этого нейрона, и по сути, является его причиной. \n";
}
