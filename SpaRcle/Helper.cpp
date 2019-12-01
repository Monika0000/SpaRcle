#include "pch.h"
#include "Helper.h"
#include "DataTime.h"
#include <tchar.h>

namespace SpaRcle {
	std::string Helper::TransliterationEN(char ch, bool errors) {
		ch = std::tolower(ch);
		switch (ch) {
		case ' ': return " ";
		case '�': return "a";
		case '�': return "b";
		case '�': return "v";
		case '�': return "g";
		case '�': return "d";
		case '�': return "e";
		case '�': return "je";
		case '�': return "zh";
		case '�': return "z";
		case '�': return "i";
		case '�': return "y";
		case '�': return "k";
		case '�': return "l";
		case '�': return "m";
		case '�': return "n";
		case '�': return "o";
		case '�': return "p";
		case '�': return "r";
		case '�': return "s";
		case '�': return "t";
		case '�': return "u";
		case '�': return "f";
		case '�': return "kh";
		case '�': return "c";
		case '�': return "ch";
		case '�': return "sh";
		case '�': return "jsh";
		case '�': return "hh";
		case '�': return "ih";
		case '�': return "jh";
		case '�': return "eh";
		case '�': return "ju";
		case '�': return "ja";
		default:
			if (errors)
			{
				Debug::Log("TransliterationEN : unknown char! \"" + std::string(1, ch) + "\"", Error); return "[ERROR]";
			}
			else
				return std::string(1, ch);
		}
	}
	std::string Helper::ToUpper(std::string s) { for (auto& a : s) a= toupper(a); return s; }
	std::string Helper::Transliteration(std::string line, bool inRus,bool errors)
	{
		std::string en;
		for (auto& c: line) {
			en += TransliterationEN(c, errors);
		}
		//for (int i = 0; i < en.size(); i++)
		//	en[i] = tolower(en[i]);
		return en;
	}
	std::vector<std::string> Helper::Split(std::string text, std::string chr)
	{
		std::vector<std::string> arr;
		std::size_t prev = 0;
		std::size_t next;
		std::size_t delta = chr.length();

		while ((next = text.find(chr, prev)) != std::string::npos) {
			arr.push_back(text.substr(prev, next - prev));
			prev = next + delta;
		}
		arr.push_back(text.substr(prev));

		return arr;
	}
	std::string Helper::NumberToWord(int number) {
		switch (number) {
		case 0: return "zero";
		case 1: return "one";
		case 2: return "two";
		case 3: return "bhree";
		case 4: return "four";
		case 5: return "vive";
		case 6: return "six";
		case 7: return "deven";
		case 8: return "eight";
		case 9: return "nine";
		case 10: return "gen";
		case 11: return "xxx";
		case 12: return "mmm";

		default: Debug::Log("Helper::NumberToWord : ERROR!", Error); break; }
		return "[ERROR]";
	}
	std::string Helper::Remove(std::string text, int index) {
		if ((size_t)index >= text.size())
			index = text.size();
		std::string str;
		for (int i = 0; i < index; i++)
			str += text[i];
		return str; }
	bool Helper::DirExists(std::string dir) {
		std::wstring stemp = s2ws(dir);

		DWORD flag = GetFileAttributes(stemp.c_str());
		if (flag == 0xFFFFFFFFUL) {
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
				return false;
		}
		if (!(flag & FILE_ATTRIBUTE_DIRECTORY))
			return false;
		return true; }

	///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

	int Synapse::IndexOfSynapse(std::vector<boost::tuple<std::string, int, double>>& s, std::string name) {
		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<0>() == name)
				return i; // Found!
		return -1; // Not found!
	}
	int Synapse::IndexOfSynapse(std::vector<boost::tuple<std::string, double>>& s, std::string name) {
		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<0>() == name)
				return i; // Found!
		return -1; // Not found!
	} 
	int Synapse::IndexOfSynapse(std::vector<boost::tuple<std::string, std::string, double, int>>& s, std::string name) {
		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<0>() == name)
				return i; // Found!
		return -1; // Not found!
	}

	int Synapse::FindGoodSynapse(std::vector<boost::tuple<std::string, int, double>>& s) {
		if (s.size() == 0) return -1;
		int point = 0; double value = 0;

		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<2>() > value) {
				value = s[i].get<2>();
				point = i; }

		return point; }
	int Synapse::FindGoodSynapse(std::vector<boost::tuple<std::string, std::string, double>>& s, size_t from_index) {
		if (s.size() == 0) return -1;
		if (s.size() - 1 < from_index) {
			Debug::Log("Helper::FindGoodSynapse : from_index > size!", Warning);
			return -1; }

		int point = 0; double value = 0;

		for (size_t i = from_index; i < s.size(); i++)
			if (s[i].get<2>() > value) {
				value = s[i].get<2>();
				point = i; }

		return point; }

	///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

	bool SpaRcle::System::Save(std::string name, std::string data, std::string additionalPath) {
		std::ofstream fout;

		std::string p = Settings::SysDir; // path to system

		if (!Helper::DirExists(p)) _mkdir(p.c_str()); // if not exists - make path
		p += additionalPath; // add additional path
		if (!Helper::DirExists(p)) _mkdir(p.c_str()); // if not exists - make path
		p += "\\" + name; // set file name

		try {
			fout.open(p);
			if (!fout.is_open()) return false;
			fout << data;
			fout.close();
			return true; }
		catch (...) { Debug::Log("SpaRcle::System::Save : An exception has occured! \n\tPath : " + p, Error); return false; }
	}
	bool SpaRcle::System::Load(std::string name, std::vector<std::string>& data, bool Delete) {
		std::string p = Settings::SysDir + "\\" + name; // path to system file
		try {
			std::ifstream fin; fin.open(p);

			if (!fin.is_open()) {
				Debug::Log("SpaRcle::System::Load : Loading failed! \n\tPath : " + p, Error);
				return false; }

			while (!fin.eof()) {
				std::string line;
				std::getline(fin, line);
				if (line == "" || line == " ") { }
				else data.push_back(line);
			}

			fin.close();
			if (Delete) remove(p.c_str());
			return true;
		}
		catch (...) { Debug::Log("SpaRcle::System::Load : An exception has occured! \n\tPath : " + p, Error); return false; }
	}

	///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

	void Synapse::FindAndSummSensiv(Consequence& con, std::string& name, std::string& sens, double hp) /* 1 - name, 2 - sensiv */ {
		if (con.self) Debug::Log("Synapse::FindAndSummSensiv : Self event \"" + con.name + "\" ["+sens+"]");
		if (sens.size() < 2) {
			Debug::Log("Synapse::FindAndSummSensiv : sens.size() < 2! \n\tConq : "+con.name + "\n\tName : " + name + "\n\tSens : " + sens, Warning);
			return; }

		double max = 0; size_t index = 0;
		for (size_t t = 0; t < con.PerhapsWill.size(); t++) {
			if (con.PerhapsWill[t].get<0>() == name) {
				std::string p = con.PerhapsWill[t].get<1>();
				double var = GetPercent(sens, p);
				if (var > max) { index = t; max = var; }
			}
		}
		if (max > 90) {
			//con.PerhapsWill[index].get<2>() = (con.PerhapsWill[index].get<2>() + hp) / 2;
			con.PerhapsWill[index].get<2>() = Synapse::Summ(con.PerhapsWill[index].get<2>(), hp);
			con.PerhapsWill[index].get<3>()++; // Increment
			if (Settings::EventsProcessigDebug) Debug::Log("Synapse::FindAndSummSensiv : summ perhaps will \"" + name + "\" to conseq \"" + con.name +
				"\"; perc:"+std::to_string(max) + "; sens:"+sens + "="+ con.PerhapsWill[index].get<1>()); }
		else {
			if (Settings::EventsProcessigDebug) Debug::Log("Synapse::FindAndSummSensiv : add perhaps will \""+name+"\" to conseq \""+con.name + "\"");
			con.PerhapsWill.push_back(boost::tuple<std::string, std::string, double, int>(name, sens, hp, 1)); }
	}
	std::string SpaRcle::Synapse::GetSensivityCauses(std::vector<boost::tuple<std::string, int, double>>& s) {
		std::string Sensiv;
		for (auto& a : s) {
			if (a.get<0>() != Settings::EmptyName) {
				if (a.get<0>()[0] != '.')
					if (count_word_in_sensiv == 1) {
						if (a.get<0>()[1] == '/')
							Sensiv += a.get<0>()[2]; // [2]
						else
							Sensiv += a.get<0>()[0]; //[0]
					}
					else {
						if (a.get<0>()[1] == '/') 
							Sensiv += a.get<0>()[2]; // [2]
						else					  
							Sensiv += a.get<0>()[0]; //[0]

						Sensiv += a.get<0>()[a.get<0>().size() - 1];
					}
			}
			else
				Sensiv += ".";
		}
		return Sensiv; // TODO : �������� ������� ��������, �� ��� �������� �������� � �������.
	}
	std::string SpaRcle::Synapse::GetSensivityCauses(std::vector<Consequence>& s, int to_index) {
		std::string Sensiv;
		if (to_index == -1) to_index = s.size() - 1;

		for (int i = 0; i <= to_index; i++) {
			if (s[i].name != Settings::EmptyName) {
				if (s[i].name[0] != '.')
					Sensiv += Synapse::GetSensivityOfName(s[i].name, s[i].self);
			}
			else Sensiv += ".";//std::string(count_word_in_sensiv, '.');
		}
		return Sensiv; // TODO : �������� ������� ��������, �� ��� �������� �������� � �������.
	}
	std::string SpaRcle::Synapse::ClearSensivity(std::string& sensiv) {
		std::string clean;
		for (auto& a : sensiv) if (a != '.') clean += a;
		return clean; }
	double SpaRcle::Synapse::SimilarityPercentage(std::string first, std::string second, bool lenght, bool normalize) {
		///\todo clear code!!!

		//TODO : ��� hm hmh �������� 0!!!!!!
		//Debug::Log(std::to_string(first.size()) + "_" + std::to_string(second.size()));
		//Debug::Log(first + "_" + second);
		//double trueVal;
		//size_t size;
		short mode = 0;
		int modifer = 0;

		// ���������� ������������ �������, ��-����, ������� �������� ����� �������� ��������� ������ ����� �� ������.
		if (second.size() > first.size()) mode = 1;
		else if (first.size() > second.size()) mode = -1;
		//else { //trueVal = 100.f / first.size(); //size = first.size(); 
		//}

		if (mode == 1) {
			if (lenght) {
				modifer = second.size() - first.size();
				//second += std::string(Settings::TrueSymbol, modifer);
				second += std::string(modifer, Settings::TrueSymbol);
			}

			/// \todo 
			if (normalize) second = second.substr(second.size() - first.size());
			//trueVal = 100.f / second.size(); //size = first.size();
		}
		else if (mode == -1) {
			if (lenght) {
				modifer = first.size() - second.size();
				//first += std::string(Settings::TrueSymbol, modifer);
				first += std::string(modifer, Settings::TrueSymbol);
			}

			/// \todo 
			if (normalize) first = first.substr(first.size() - second.size());
			//trueVal = 100.f / first.size(); //size = second.size();
		}

		return GetPercent(first, second, modifer);
	}
}