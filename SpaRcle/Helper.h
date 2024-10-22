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
#include <rpcndr.h>
#include <ctime>
//#include <boost/tuple/tuple.hpp>
#include <direct.h>
#include "Settings.h"
#include <cctype>
#include "Action.h"

namespace SpaRcle {
	class Synapse {
	public:
		static const size_t ToInt(std::string sens);
		static inline double Summ(double& l, double& r) {
			//double d = (l + r) / Div;
			double d = (l + r) / Div;
			if (d > 99999) d = 99999;
			if (d < -99999) d = -99999;
			return d; }
		static inline void SummHpSyns(std::tuple<std::string, double>& left, std::tuple<std::string, double>& right) { std::get<1>(left) = Summ(std::get<1>(left), std::get<1>(right)); }
		static inline void SummHpSyns(std::tuple<std::string, double>& left, double right) { std::get<1>(left) = Summ(std::get<1>(left), right); }
		static std::string ClearSensivity(std::string& sensiv);

		///!Depend !of !size !sensivity
		inline static double GetPercent(std::string& first, std::string& second, short modifer = -1) {
			double percent = 0;
			size_t size = 0; double trueVal = 0;
			if (second.size() > first.size()) { size = first.size(); trueVal = 100.f / second.size(); }
			else { size = second.size(); trueVal = 100.f / first.size(); }
			for (size_t t = 0; t < size; t++) {
				if (first[size - t - 1] == '*' || second[size - t - 1] == '*') {
					percent += trueVal; continue;
				}

				//if (std::isdigit(first[size - t - 1]) && !std::isdigit(second[size - t - 1])) {
				//	first.erase(first.begin() + size - t - 1); first += Settings::TrueSymbol;
				//}
				//else if (std::isdigit(second[size - t - 1]) && !std::isdigit(first[size - t - 1])) {
				//	second.erase(second.begin() + size - t - 1); second += Settings::TrueSymbol;
				//}

				if (first[size - t - 1] == second[size - t - 1])
					percent += trueVal;
				else if (modifer != -1)
					if (first[size - t - 1] == Settings::TrueSymbol || second[size - t - 1] == Settings::TrueSymbol)
						//percent += trueVal / (4 + modifer);
						percent += trueVal / (double(modifer) + 4.0);
			}
			return percent;
		}
		static void FindAndSummSensiv(Consequence& con, std::string& name, std::string* sens, double hp, bool addNew = true);
		//static std::string GetSensivityCauses(std::vector<std::tuple<std::string, int, double>>& s);
		static std::string GetSensivityCauses(std::vector<Consequence>& s, int to_index = -1);
		inline static std::string GetSensivityOfName(std::string name, bool self) {
			if (name.size() > 2)
				if (name[1] == '/') name = name.substr(2);
			if (count_word_in_sensiv == 1) {
				name.resize(1);
				//if (conq.action.type == AType::VisualData) { name += std::to_string(Visual(conq.action).pos); }
				return name;
			}
			else if (count_word_in_sensiv == 2) {
				std::string newName;
				if (name.size() < 2) {
					newName += name[0];
					newName += Settings::TrueSymbol;
				}
				else { newName += name[0]; newName += name[name.size() - 1]; }

				if (self)
					for (unsigned char c = 0; c < name.size(); c++) newName[c] = toupper(newName[c]); // �������������� ������� � ���������
					//Debug::Log("GetSensivityOfName : name = " + name + "; sens = " + newName);

				name.clear(); name.~basic_string();

				//if (conq.action.type == AType::VisualData) { newName += std::to_string(Visual(conq.action).pos); }
				return newName;
			}
			else {
				Debug::Log("Helper::GetSensivityOfName : unknow variant!", Error);
				name.clear(); name.~basic_string();
				return "[ERROR]";
			}
		}
		inline static std::string GetSensivityOfName(Consequence&conq, bool self) {
			return Synapse::GetSensivityOfName(conq.name, self);
		}
		static double SimilarityPercentage(std::string first, std::string second, bool lenght = false, bool normalize = false);
		///!Depend !of !size !sensivity

		static int IndexOfSynapse(std::vector<std::tuple<std::string, int, double>>& s, std::string name);
		static int IndexOfSynapse(std::vector<std::tuple<std::string, double>>& s, std::string name); //std::string,
		static int IndexOfSynapse(std::vector<std::tuple<std::string, std::string, double, int>>& s, std::string name);

		static int FindGoodSynapse(std::vector<std::tuple<std::string, int, double>>& s);
		static int FindGoodSynapse(std::vector<std::tuple<std::string, std::string, double>>& s, size_t from_index = 0);
	};

	class Helper {
	public : 
		inline static const bool Contains(std::string& str, char c) {
			for (size_t t = 0; t < str.size(); t++)
				if (str[t] == c) return true;
			return false;
		}
		static double CPUSpeed(void);

		inline static const bool randomBool() {
			return rand() > (RAND_MAX / 2);
		}
		static float RandomFloat(bool negative = true);
		static std::string SummArray(std::vector<std::string>& _array, char space = '\0');
		static std::string SummArray(std::vector<Consequence>& _array, char space = '\0');

		static std::string Transliteration(std::string line, bool inRus = false, bool errors = true);
		static std::string TransliterationEN(char ch, bool errors = true);
		static std::string TransliterationRU(std::string line);

		static std::string ToUpper(std::string s);

		inline static KeyboardLayout GetLayout() {
			HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD id = GetThreadId(hStdOut);

			switch (LOWORD(GetKeyboardLayout(id))) {
			case 0x409: return KeyboardLayout::EN;
			case 0x419: return KeyboardLayout::RU;
			default: return KeyboardLayout::UNKNOWN; }
		}
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
		static std::vector<std::string> Split(std::string text, std::string chr, std::string end, size_t& index);
		static inline std::string GetNameWithType(Consequence& conq) {
			std::string n;
			n += ToString(conq.action.type)[0];
			return n + "/" + conq.name;
		}
		static std::string NumberToWord(int number);
		static std::string Remove(std::string text, int index);
		static std::string Remove(std::string text, char symbol, size_t& index);
		static bool DirExists(std::string dir);

		inline static void SimpleSummConseq(Consequence& left, Consequence& right, bool onlyAdd = false) {
			if (left.Causes.size() == 0) left.Causes = right.Causes;
			else if (right.Causes.size() == 0) right.Causes = left.Causes;
			else for (size_t i = 0; i < right.Causes.size(); i++) {
				int indx = Synapse::IndexOfSynapse(left.Causes, right.GetCS_Name(i));
				if (indx == -1)
					left.Causes.push_back(right.Causes[i]);
				else {
					left.GetCS_Meet(indx) = (left.GetCS_Meet(indx) + right.GetCS_Meet(i)) / 2; // ��������� ���������� ������ ������� �������
					//left.Causes[indx].get<2>() = (left.Causes[indx].get<2>() + right.Causes[i].get<2>()) / Div; /* ��������� ���������� */
					left.GetCS_HP(indx) = Synapse::Summ(left.GetCS_HP(indx), right.GetCS_HP(i)); /* ��������� ���������� */
				}
			}

			if (left.PerhapsWill.size() == 0)  left.PerhapsWill = right.PerhapsWill;
			else if (right.PerhapsWill.size() == 0) right.PerhapsWill = left.PerhapsWill;
			else {
				size_t index = 0; bool f = false;
				for (size_t i = 0; i < right.PerhapsWill.size(); i++) {
					f = false;
					for (size_t p = index; p < left.PerhapsWill.size(); p++) {
						if (right.GetPW_Name(i) == left.GetPW_Name(p))
							if (right.GetPW_Sens(i) == left.GetPW_Sens(p)) {
								//Debug::Log(std::to_string(left.GetPW_HP(p)) + " " + std::to_string(right.GetPW_HP(i)));

								if (!onlyAdd) {
									left.GetPW_HP(p) = Synapse::Summ(left.GetPW_HP(p), right.GetPW_HP(i)); // ��������� ���������� 
								}
								left.GetPW_Meet(p)++;

								index = p;
								f = true;
								break;
							}
					}
					if (!f)
						left.PerhapsWill.push_back(right.PerhapsWill[i]);
				}
			}
			if (left.Synapses.size() == 0) left.Synapses = right.Synapses;
			else if (right.Synapses.size() == 0) right.Synapses = left.Synapses;
			else for (size_t i = 0; i < right.Synapses.size(); i++) {
				int indx = Synapse::IndexOfSynapse(left.Synapses, right.GetSN_Name(i));
				if (indx == -1) { left.Synapses.push_back(right.Synapses[i]); Debug::Log("SSC : New synapse = "+std::get<0>(right.Synapses[i]) + " at event = " + left.name); }
				else if(!onlyAdd) {
					//Debug::Log(left.name + " ====== " + std::get<0>(left.Synapses[indx]) + " " + std::to_string(std::get<1>(left.Synapses[indx])));
					static int temp = 0;
					temp++;
					//if(temp > 3500)
					Debug::Log("SSC : " + left.name + " ====== " + std::get<0>(right.Synapses[i]) + " " + std::to_string(std::get<1>(right.Synapses[i])));
					Synapse::SummHpSyns(left.Synapses[indx], right.Synapses[i]);
					//Debug::Log(std::get<0>(left.Synapses[indx]) + " " + std::to_string(std::get<1>(left.Synapses[indx])));
				}
			}
		}
		/*
		inline static bool SummActionConseq(Consequence& left, Consequence& right) {
			if (left.action.type != right.action.type) {
				Debug::Log("Helper::SummActionConseq : Discrepancy types! \n	Left : "
					+ left.name + " " + std::string(ToString(left.action.type)) + "\n	Right : "
					+ right.name + " " + std::string(ToString(right.action.type)), Error);
				return false; }

			if (left.name != right.name) {
				Debug::Log("Helper::SummActionConseq : Discrepancy names! \n	Left : "
					+ left.name + "\n	Right : " + right.name, Error);
				return false; }

			switch (left.action.type) {
			case AType::Undefined: {
				/// \TODO
				break; }
			case AType::Speech: {
				/// \TODO
				break; }
			case AType::VisualData: {
				/// \TODO
				break; }
			case AType::Move: {
				/// \TODO
				break; }
			default:
				Debug::Log("Helper::SummActionConseq : Unknown type! \n\tType : \"" + std::string(ToString(left.action.type)) 
					+ "\"\n\t Name : "+ left.name, Error);
				break; }
			return true;
		}
*/
		inline static int IndexOfCause(std::vector<std::tuple<std::string, int, double>>& Causes, std::string& name) {
			for (size_t t = 0; t < Causes.size(); t++) 
				if (std::get<0>(Causes[t]) == name)
					return (int)t;
			return -1; }
		inline static int IndexOfPerhapsWill(std::vector<std::tuple<std::string, std::string, double, int>>& Perhaps, std::string& name) {
			for (size_t t = 0; t < Perhaps.size(); t++)
				if (std::get<0>(Perhaps[t]) == name)
					return (int)t;
			return -1; }

		template<typename T> inline static void Sort(std::vector<int>& keys, std::vector<T>& data) {
			if (keys.size() != data.size()) {
				Debug::Log("Helper::Sort = ERROR : Sizes differ! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData size : " + std::to_string(data.size()), Error);
				return; }
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
		template<typename T> inline static void SelectionSort(std::vector<int>& keys, std::vector<T>& data) {
			if (keys.size() != data.size()) {
				Debug::Log("Helper::SelectionSort = ERROR : Sizes differ! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData size : " + std::to_string(data.size()), Error);
				return; }

			if (keys.size() == 0 && data.size() == 0) {
				Debug::Log("Helper::SelectionSort = ERROR : Sizes equal zero! \n\tKeys size : " + std::to_string(keys.size()) + "\n\tData size : " + std::to_string(data.size()), Error);
				return; }

			for (size_t startIndex = 0; startIndex < keys.size() - 1; ++startIndex) {
				// � ���������� smallestIndex �������� ������ ����������� ��������, ������� �� ����� � ���� ��������
				// �������� � ����, ��� ���������� ������� � ���� �������� - ��� ������ ������� (������ 0)
				int smallestIndex = startIndex;

				// ����� ���� ������� �������� � ��������� ����� �������
				for (size_t currentIndex = startIndex + 1; currentIndex < keys.size(); ++currentIndex) {
					// ���� �� ����� �������, ������� ������ ������ ����������� ��������,
					if (keys[currentIndex] < keys[smallestIndex])
						// �� ���������� ���
						smallestIndex = currentIndex; }

				// smallestIndex ������ ���������� ������� 
						// ������ ������� ���� ��������� ���������� ����� � ���, ������� �� ����������
				std::swap(keys[startIndex], keys[smallestIndex]);
				std::swap(data[startIndex], data[smallestIndex]);
			}
		}
		inline static void SelectionSort(std::vector<std::tuple<std::string, double>>& Synapses) {//, std::string
			for (size_t startIndex = 0; startIndex < Synapses.size() - 1; ++startIndex) {
				// � ���������� smallestIndex �������� ������ ����������� ��������, ������� �� ����� � ���� ��������
				// �������� � ����, ��� ���������� ������� � ���� �������� - ��� ������ ������� (������ 0)
				int smallestIndex = startIndex;

				// ����� ���� ������� �������� � ��������� ����� �������
				for (size_t currentIndex = startIndex + 1; currentIndex < Synapses.size(); ++currentIndex) {
					// ���� �� ����� �������, ������� ������ ������ ����������� ��������,
					//if (Synapses[currentIndex].get<2>() < Synapses[smallestIndex].get<2>())
					if (std::get<1>(Synapses[currentIndex]) < std::get<1>(Synapses[smallestIndex]))
						// �� ���������� ���
						smallestIndex = currentIndex; }

				// smallestIndex ������ ���������� ������� 
						// ������ ������� ���� ��������� ���������� ����� � ���, ������� �� ����������
				std::swap(Synapses[startIndex], Synapses[smallestIndex]);
			}
		}
		inline static void SelectionSort(std::vector<std::tuple<std::string, int, double>>& Causes, bool warning = true) { // Sort to meetings
			if (Causes.size() >= 3)
				for (size_t startIndex = 0; startIndex < Causes.size() - 1; ++startIndex) {
					int smallestIndex = startIndex;

					for (size_t currentIndex = startIndex + 1; currentIndex < Causes.size(); ++currentIndex)
						if (std::get<1>(Causes[currentIndex]) < std::get<1>(Causes[smallestIndex]))
							smallestIndex = currentIndex;

					std::swap(Causes[startIndex], Causes[smallestIndex]);
				}
			else if (warning) Debug::Log("SelectionSort (Causes) : size < 3!", Warning);
		}

		inline static AType GetConseqType(std::string& conseq) {
			AType a = ToAType(conseq[0]);
			if (a == AType::Undefined) Debug::Log("GetConseqType : Unknown type! Name : " + conseq, Error);
			return a; }

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

			for (size_t startIndex = 0; startIndex < keys.size() - 1; ++startIndex) {
				// � ���������� smallestIndex �������� ������ ����������� ��������, ������� �� ����� � ���� ��������
				// �������� � ����, ��� ���������� ������� � ���� �������� - ��� ������ ������� (������ 0)
				int smallestIndex = startIndex;

				// ����� ���� ������� �������� � ��������� ����� �������
				for (size_t currentIndex = startIndex + 1; currentIndex < keys.size(); ++currentIndex) {
					// ���� �� ����� �������, ������� ������ ������ ����������� ��������,
					if (keys[currentIndex] < keys[smallestIndex])
						// �� ���������� ���
						smallestIndex = currentIndex; }

				// smallestIndex ������ ���������� ������� 
						// ������ ������� ���� ��������� ���������� ����� � ���, ������� �� ����������
				std::swap(keys[startIndex], keys[smallestIndex]);
				std::swap(data_1[startIndex], data_1[smallestIndex]);
				std::swap(data_2[startIndex], data_2[smallestIndex]);
			}

			return true;
		}
	};

	class System {
	public:
		static bool Save(std::string name, std::string data, std::string additionalPath = "\\temp");
		static bool Load(std::string name, std::vector<std::string>& data, bool Delete = false);
	};

	template <typename T> const bool Contains(std::vector<T>& Vec, const T& Element) {
		if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end()) return true;
		return false; };

	inline std::string ReadUpToChar(std::string line, char ch, short& num, short max = 80) /* Max 7 elements!!!! */ {
		std::string result;
		for (char i = 0; i < (char)line.size(); i++) {
			if (line[i] == '.') line[i] = ',';
			if (line[i] != ch) {
				if (i < max) { result += line[i]; }
			}
			else {
				num = i + 1;
				if (result[result.size() - 1] == '.')result.resize(result.size() - 1);
				return result;
			} 
		}
		return result; }
	template<typename T> inline std::vector<T> Remove(std::vector<T> arr, size_t index) {
		if (index > arr.size()) Debug::Log("SpaRcle::Remove : Index > Array.Size()!", Warning);
		else arr.resize(index);
		return arr; }
	inline double Round(double x) { return ((x * 100) / 100); /*return floor(x * 100) / 100;*/ }
}