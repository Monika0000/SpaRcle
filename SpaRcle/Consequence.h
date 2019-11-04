#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Action.h"
#include "DataTime.h"
#include <boost/tuple/tuple.hpp>

namespace SpaRcle {
	struct Consequence
	{
	public:
		std::string name;
		int meetings;
		double Bad = 0, Good = 0;
		Action action; /* Multi-value */
		bool self = false;

		std::vector<boost::tuple<std::string, double>> Synapses;			/* This are ref to path Consequence		|	Max = 5   */
		std::vector<boost::tuple<std::string, std::string, double, int>> PerhapsWill;			/* This are ref to path Consequence		|	Max = 10  */
		std::vector<boost::tuple<std::string, int, double>> Causes;		/* This are ref to path Consequence		|	Max = 15  */

		DataTime EventData; /* Do not save */

		///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
		static bool Save(Consequence* conseq, const bool Diagnostic = false);
		bool Save(const bool Diagnostic = false);

		bool Load(std::string name, AType atype, bool notFoundIsError, bool Diagnostic);
		bool Load(std::string name, AType atype);

		Consequence();
		Consequence(std::string name);
		Consequence(std::string name, AType atype);
		Consequence(Sound speech, bool self = false);
		Consequence(Visual visual);
		Consequence(std::string name, Action action);
		Consequence(std::string name, Action action, int meets);
		Consequence(std::string name, Action action, int meets, double Bad, double Good);
		~Consequence();

		inline double GetSummHP() { return Good - Bad; }
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
		"\n  ������ ����� ����� ����������� � �������� ������ ����������� �������. \n"

		"  Synapses (������) - ��������� �� ��, ��� ����� ������ ����� ����� �������. \n(����� ���� ��������, � �� �����) \n"
		"  ���������� �������� ������ ���� � ������ ������� ������. \n��-����, ��� �� ����� ������� ���������� � ���, ��� ������� ����� ������ ����. \n"

		"  PerhapsWill (�������� ����������) - ��, ���� ������� ������� ����� �����. \n(������ ������� ���� ���������� �� �� ����� ����) \n" 

		"  Causes (�������) - ��, ��� ���� �� ����� �������, � �� ����, �������� ��� ��������. \n";
}
