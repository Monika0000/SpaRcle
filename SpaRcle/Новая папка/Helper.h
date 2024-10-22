#pragma once
//#define CATCH_CONFIG_MAIN /* Useless */
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include "Consequence.h"
#include <windows.h>
#include <ctime>
#include <boost/tuple/tuple.hpp>
#include <direct.h>

namespace SpaRcle {
	enum ECom {
		Synp, PerhWill
	};

	class Helper {
	public : 
		static std::wstring s2ws(const std::string& s)
		{
			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			return r;
		}

		static std::vector<std::string> Split(std::string text, std::string chr);

		static inline std::string GetNameWithType(Consequence& conq) {
			std::string n;
			n += ToString(conq.action.type)[0];
			return n + "/" + conq.name;
		}
		
		static std::string NumberToWord(int number);

		static std::string Remove(std::string text, int index);

		static bool DirExists(std::string dir);

		static int IndexOfSynapse(std::vector<boost::tuple<std::string, int, double>>& s, std::string name);
		static int IndexOfSynapse(std::vector<boost::tuple<std::string, std::string, double>>& s, std::string name);
		static int IndexOfSynapse(std::vector<boost::tuple<std::string, std::string, double, int>>& s, std::string name);

		static int FindGoodSynapse(std::vector<boost::tuple<std::string, int, double>>& s);
		static int FindGoodSynapse(std::vector<boost::tuple<std::string, std::string, double>>& s, size_t from_index = 0);
	 
		inline static void SimpleSummConseq(Consequence& left, Consequence& right) {
			if (left.Causes.size() == 0) left.Causes = right.Causes;
			else if (right.Causes.size() == 0) right.Causes = left.Causes;
			else for (size_t i = 0; i < right.Causes.size(); i++) {
				int indx = Helper::IndexOfSynapse(left.Causes, right.Causes[i].get<0>());
				if (indx == -1)
					left.Causes.push_back(right.Causes[i]);
				else {
					left.Causes[indx].get<1>() = (left.Causes[indx].get<1>() + right.Causes[i].get<1>()) / 2; // ��������� ���������� ������ ������� �������

					left.Causes[indx].get<2>() = (left.Causes[indx].get<2>() + right.Causes[i].get<2>()) / Div; /* ��������� ���������� */
				}
			}

			left.PerhapsWill = left.PerhapsWill; /* ���������� ������� ���������, � ������ ������ ���������� ��������� */
			if (left.PerhapsWill.size() == 0) left.PerhapsWill = right.PerhapsWill;
			else if (right.PerhapsWill.size() == 0) right.PerhapsWill = left.PerhapsWill;
			else for (size_t i = 0; i < right.PerhapsWill.size(); i++) {
				int indx = Helper::IndexOfSynapse(left.PerhapsWill, right.PerhapsWill[i].get<0>());
				if (indx == -1) left.PerhapsWill.push_back(right.PerhapsWill[i]);
				else {
					//left.PerhapsWill[indx].get<1>() = Helper::SummSensivity(left.PerhapsWill[indx].get<1>(), right.PerhapsWill[i].get<1>()); // ��������� �������� ������ �������
					//Helper::SummSensivity(left.PerhapsWill[indx].get<1>(), right.PerhapsWill[i].get<1>()); // ��������� �������� ������ �������
					Helper::SummSensivity(left, indx, right.PerhapsWill[i].get<1>(), ECom::PerhWill); // ��������� �������� ������ �������
					left.PerhapsWill[indx].get<2>() = (left.PerhapsWill[indx].get<2>() + right.PerhapsWill[i].get<2>()) / Div; /* ��������� ���������� */

					left.PerhapsWill[indx].get<3>()++;
					//left.PerhapsWill[indx].get<3>() = (left.PerhapsWill[indx].get<3>() + right.PerhapsWill[i].get<3>()) / 2;
					// TODO : ����� ���������� �������� - ��, ��� ���� �� �������
					// 0 - Name, 1 - Sensiv, 2 - Helpfulness, 3 - meets
				}
			}

			left.Synapses = left.Synapses; /* ���������� �������, � ������ ������ ���������� ��������� */
			if (left.Synapses.size() == 0) left.Synapses = right.Synapses;
			else if (right.Synapses.size() == 0) right.Synapses = left.Synapses;
			else for (size_t i = 0; i < right.Synapses.size(); i++) {
				int indx = Helper::IndexOfSynapse(left.Synapses, right.Synapses[i].get<0>());
				if (indx == -1) left.Synapses.push_back(right.Synapses[i]);
				else {
					//left.Synapses[indx].get<1>() = Helper::SummSensivity(left.Synapses[indx].get<1>(), right.Synapses[i].get<1>()); // ��������� �������� ������ �������
					//Helper::SummSensivity(left.Synapses[indx].get<1>(), right.Synapses[i].get<1>()); // ��������� �������� ������ �������
					Helper::SummSensivity(left, indx, right.Synapses[i].get<1>(), ECom::Synp); // ��������� �������� ������ �������
					left.Synapses[indx].get<2>() = (left.Synapses[indx].get<2>() + right.Synapses[i].get<2>()) / Div; /* ��������� ���������� */
					// TODO : ����� ���������� �������� - ��, ��� ���� �� �������
					// 0 - Name, 1 - Sensiv, 2 - Helpfulness
				}
			}
		}
		inline static bool SummActionConseq(Consequence& left, Consequence& right) {
			if (left.action.type != right.action.type) {
				Debug::Log("Helper::SummActionConseq : Discrepancy types! \n	Left : "
					+ std::string(ToString(left.action.type)) + "\n	Right : "
					+ std::string(ToString(right.action.type)), Error);
				return false;
			}

			if (left.name != right.name) {
				Debug::Log("Helper::SummActionConseq : Discrepancy names! \n	Left : "
					+ left.name + "\n	Right : " + right.name, Error);
				return false;
			}

			switch (left.action.type)
			{
			case AType::Undefined: {
				break;
			}
			case AType::Speech: {
				break;
			}
			default:
				Debug::Log("Helper::SummActionConseq : Unknown type! \n\tType : \"" + std::string(ToString(left.action.type)) 
					+ "\"\n\t Name : "+ left.name, Error);
				break;
			}
			return true;
		}

		inline static int IndexOfCause(std::vector<boost::tuple<std::string, int, double>>& Causes, std::string name){
			for (size_t t = 0; t < Causes.size(); t++) 
				if (Causes[t].get<0>() == name)
					return (int)t;
			return -1;
		}
		inline static int IndexOfPerhapsWill(std::vector<boost::tuple<std::string, std::string, double, int>> Perhaps, std::string name)
		{
			for (size_t t = 0; t < Perhaps.size(); t++)
				if (Perhaps[t].get<0>() == name)
					return (int)t;
			return -1;
		}

		///!Depend !of !size !sensivity
		inline static double GetPercent(std::string& first, std::string& second, short modifer = -1) {
			double percent = 0;
			size_t size = 0; double trueVal;
			if (second.size() > first.size()) { size = first.size(); trueVal = 100.f / second.size(); }
			else { size = second.size(); trueVal = 100.f / first.size(); }
			for (size_t t = 0; t < size; t++) {
				if (first[size - t - 1] == '*' || second[size - t - 1] == '*') {
					percent += trueVal;continue;}
				if (first[size - t - 1] == second[size - t - 1])
					percent += trueVal;
				else if (modifer != -1)
					if (first[size - t - 1] == '_' || second[size - t - 1] == '_')
						percent += trueVal / (4 + modifer);
			}
			return percent;
		}
		static void FindAndSummSensiv(Consequence&con, std::string name, std::string sens, double hp);
		static std::string GetSensivityCauses(std::vector<boost::tuple<std::string,int, double>>& s);
		static std::string GetSensivityCauses(std::vector<Consequence>& s, int to_index = -1);
		static bool SummSensivity(std::string& left, std::string& right, bool check = false);
		static void SummSensivity(Consequence& left, size_t index, std::string& right, ECom com);
		inline static std::string GetSensivityOfName(std::string name) {
			if (count_word_in_sensiv == 1) { name.resize(1); return name; }
			else if (count_word_in_sensiv == 2) {
				std::string newName;
				if (name.size() < 2) Debug::Log("Helper::GetSensivityOfName : size of name < 2! \n\tName : "+name, Error);
				newName += name[0]; newName += name[name.size() - 1];
				return newName;
			}
			else {
				Debug::Log("Helper::GetSensivityOfName : unknow variant!", Error);
				return "[ERROR]"; }
			//if (name.size() > SpaRcle::count_word_in_sensiv)
			//	name.resize(SpaRcle::count_word_in_sensiv);
		}
		static double SimilarityPercentage(std::string first, std::string second, bool lenght = false, bool normalize = false);
		///!Depend !of !size !sensivity

		static std::string ClearSensivity(std::string& sensiv);

		template<typename T> inline static void Sort(std::vector<int>& keys, std::vector<T>& data)
		{
			if (keys.size() != data.size()) {
				Debug::Log("Helper::Sort = ERROR : Sizes differ! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData size : " + std::to_string(data.size()), Error);
				return;
			}

			int temp;

			for (size_t i = 0; i < keys.size() - 1; i++) {
				for (size_t j = 0; j < keys.size() - i - 1; j++) {
					if (keys[j] > keys[j + 1]) {
						// ������ �������� �������
						temp = keys[j];
						auto temp_data = data[j];

						keys[j] = keys[j + 1];
						data[j] = data[j + 1];

						keys[j + 1] = temp;
						data[j + 1] = temp_data;
					}
				}
			}
		}

		template<typename T> inline static void SelectionSort(std::vector<int>& keys, std::vector<T>& data)
		{
			if (keys.size() != data.size()) {
				Debug::Log("Helper::SelectionSort = ERROR : Sizes differ! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData size : " + std::to_string(data.size()), Error);
				return;
			}

			if (keys.size() == 0 && data.size() == 0)
			{
				Debug::Log("Helper::SelectionSort = ERROR : Sizes equal zero! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData size : " + std::to_string(data.size()), Error);
				return;
			}

			for (size_t startIndex = 0; startIndex < keys.size() - 1; ++startIndex)
			{
				// � ���������� smallestIndex �������� ������ ����������� ��������, ������� �� ����� � ���� ��������
				// �������� � ����, ��� ���������� ������� � ���� �������� - ��� ������ ������� (������ 0)
				int smallestIndex = startIndex;

				// ����� ���� ������� �������� � ��������� ����� �������
				for (size_t currentIndex = startIndex + 1; currentIndex < keys.size(); ++currentIndex)
				{
					// ���� �� ����� �������, ������� ������ ������ ����������� ��������,
					if (keys[currentIndex] < keys[smallestIndex])
						// �� ���������� ���
						smallestIndex = currentIndex;
				}

				// smallestIndex ������ ���������� ������� 
						// ������ ������� ���� ��������� ���������� ����� � ���, ������� �� ����������
				std::swap(keys[startIndex], keys[smallestIndex]);
				std::swap(data[startIndex], data[smallestIndex]);
			}
		}

		inline static void SelectionSort(std::vector<boost::tuples::tuple<std::string, std::string, double>>& Synapses) {
			for (size_t startIndex = 0; startIndex < Synapses.size() - 1; ++startIndex)
			{
				// � ���������� smallestIndex �������� ������ ����������� ��������, ������� �� ����� � ���� ��������
				// �������� � ����, ��� ���������� ������� � ���� �������� - ��� ������ ������� (������ 0)
				int smallestIndex = startIndex;

				// ����� ���� ������� �������� � ��������� ����� �������
				for (size_t currentIndex = startIndex + 1; currentIndex < Synapses.size(); ++currentIndex)
				{
					// ���� �� ����� �������, ������� ������ ������ ����������� ��������,
					if (Synapses[currentIndex].get<2>() < Synapses[smallestIndex].get<2>())
						// �� ���������� ���
						smallestIndex = currentIndex;
				}

				// smallestIndex ������ ���������� ������� 
						// ������ ������� ���� ��������� ���������� ����� � ���, ������� �� ����������
				std::swap(Synapses[startIndex], Synapses[smallestIndex]);
			}
		}

		inline static void SelectionSort(std::vector<boost::tuples::tuple<std::string, int, double>>& Causes, bool warning = true) { // Sort to meetings
			if (Causes.size() >= 3)
				for (size_t startIndex = 0; startIndex < Causes.size() - 1; ++startIndex)
				{
					int smallestIndex = startIndex;

					for (size_t currentIndex = startIndex + 1; currentIndex < Causes.size(); ++currentIndex)
						if (Causes[currentIndex].get<1>() < Causes[smallestIndex].get<1>())
							smallestIndex = currentIndex;

					std::swap(Causes[startIndex], Causes[smallestIndex]);
				}
			else if (warning)
				Debug::Log("SelectionSort (Causes) : size < 3!", Warning);
		}

		inline static AType GetConseqType(std::string conseq) {
			std::string head = Helper::Remove(conseq, 2);

			SWITCH(head) {
				CASE("S/") :{
					return AType::Speech;
				}
			DEFAULT: {
				Debug::Log("GetConseqType : Unknown type! Name : " + conseq, Error);
				return AType::Undefined;
				}
			}

			return AType::Undefined;
		}

		template<typename T> inline static bool SelectionSort(std::vector<int>& keys, std::vector<T>& data_1, std::vector<T>& data_2)
		{
			if (keys.size() != data_1.size() || keys.size() != data_2.size()) {
				Debug::Log("Helper::SelectionSort (int) : Sizes differ! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData_1 size : " 
					+ std::to_string(data_1.size()) + "\n\tData_2 size : " + std::to_string(data_2.size()), Error);
				return false;
			}

			if (keys.size() == 0 && data_1.size() == 0 && data_2.size() == 0)
			{
				Debug::Log("Helper::SelectionSort (int) : Sizes equal zero! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData_1 size : "
					+ std::to_string(data_1.size()) + "\n\tData_2 size : " + std::to_string(data_2.size()), Error);
				return false;
			}

			for (size_t startIndex = 0; startIndex < keys.size() - 1; ++startIndex)
			{
				// � ���������� smallestIndex �������� ������ ����������� ��������, ������� �� ����� � ���� ��������
				// �������� � ����, ��� ���������� ������� � ���� �������� - ��� ������ ������� (������ 0)
				int smallestIndex = startIndex;

				// ����� ���� ������� �������� � ��������� ����� �������
				for (size_t currentIndex = startIndex + 1; currentIndex < keys.size(); ++currentIndex)
				{
					// ���� �� ����� �������, ������� ������ ������ ����������� ��������,
					if (keys[currentIndex] < keys[smallestIndex])
						// �� ���������� ���
						smallestIndex = currentIndex;
				}

				// smallestIndex ������ ���������� ������� 
						// ������ ������� ���� ��������� ���������� ����� � ���, ������� �� ����������
				std::swap(keys[startIndex], keys[smallestIndex]);
				std::swap(data_1[startIndex], data_1[smallestIndex]);
				std::swap(data_2[startIndex], data_2[smallestIndex]);
			}

			return true;
		}


		class format { // ����������
			std::stringstream ss;
		public:
			template<typename T>
			format &operator <<(const T &x) { ss << x; return *this; }
			operator std::string() { return ss.str(); }
		};
	};

	class System {
	public:
		static bool Save(std::string name, std::string data, std::string additionalPath = "\\temp");
		static bool Load(std::string name, std::vector<std::string>& data, bool Delete = false);
	};

	template <typename T>
	const bool Contains(std::vector<T>& Vec, const T& Element)
	{
		if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end())
			return true;

		return false;
	};

	inline std::string ReadUpToChar(std::string line, char ch, int& num)
	{
		std::string result;
		for (int i = 0; i < (int)line.size(); i++) {
			if (line[i] != ch)
				result += line[i];
			else {
				num = i + 1;
				return result;
			}
		}

		return result;
	}

	template<typename T> inline std::vector<T> Remove(std::vector<T> arr, size_t index)
	{
		if (index > arr.size())
			Debug::Log("SpaRcle::Remove : Index > Array.Size()!", Warning);
		else
			arr.resize(index);
		return arr;
	}

	inline double Round(double x) {
		return ((x * 100) / 100);
		//return floor(x * 100) / 100;
	}
}