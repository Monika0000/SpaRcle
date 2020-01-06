#include "pch.h"
#include "Helper.h"
#include "DataTime.h"
#include <tchar.h>
#include <typeinfo>

namespace SpaRcle {
	std::string Helper::TransliterationEN(char ch, bool errors) {
		ch = std::tolower(ch);
		switch (ch) {
		case ' ': return " ";
		case 'а': return "a";
		case 'б': return "b";
		case 'в': return "v";
		case 'г': return "g";
		case 'д': return "d";
		case 'е': return "e";
		case 'ё': return "je";
		case 'ж': return "zh";
		case 'з': return "z";
		case 'и': return "i";
		case 'й': return "y";
		case 'к': return "k";
		case 'л': return "l";
		case 'м': return "m";
		case 'н': return "n";
		case 'о': return "o";
		case 'п': return "p";
		case 'р': return "r";
		case 'с': return "s";
		case 'т': return "t";
		case 'у': return "u";
		case 'ф': return "f";
		case 'х': return "kh";
		case 'ц': return "c";
		case 'ч': return "ch";
		case 'ш': return "sh";
		case 'щ': return "js";
		case 'ъ': return "jt";
		case 'ы': return "ih";
		case 'ь': return "jh";
		case 'э': return "eh";
		case 'ю': return "ju";
		case 'я': return "ja";
		default:
			if (errors)
			{
				Debug::Log("TransliterationEN : unknown char! \"" + std::string(1, ch) + "\"", Error); return "[ERROR]";
			}
			else
				return std::string(1, ch);
		}
	}
	std::string Helper::TransliterationRU(std::string line) {
		std::string ru = ""; unsigned short size = (unsigned short)line.size();
		//Debug::Log(line);
		for (unsigned short i = 0; i < size; i++) {
			switch (line[i]) {
			case ' ': ru += " "; break;
			case 'a': ru += "a"; break;
			case 'b': ru += "б"; break;
			case 'v': ru += "в"; break;
			case 'g': ru += "г"; break;
			case 'd': ru += "д"; break;
			case 'y': ru += "й"; break;
			case 'l': ru += "л"; break;
			case 'm': ru += "м"; break;
			case 'n': ru += "н"; break;
			case 'o': ru += "о"; break;
			case 'p': ru += "п"; break;
			case 'r': ru += "р"; break;
			case 't': ru += "т"; break;
			case 'u': ru += "у"; break;
			case 'f': ru += "ф"; break;

			case 's': { if (i + 1 < size) { if (line[i + 1] == 'h') { i++; ru += "ш"; } else ru += "с"; } else ru += "с"; break; }; // ш\с
			case 'k': { if (i + 1 < size) { if (line[i + 1] == 'h') { i++; ru += "х"; } else ru += "к"; } else ru += "к"; break; }; // х\к
			case 'i': { if (i + 1 < size) { if (line[i + 1] == 'h') { i++; ru += "ы"; } else ru += "и"; } else ru += "и"; break; }; // ы\и

			case 'z': { if (i + 1 < size) { if (line[i + 1] == 'h') { i++; ru += "ж"; } else ru += "з"; } else ru += "з"; break; }; // ж\з
			case 'c': { if (i + 1 < size) { if (line[i + 1] == 'h') { i++; ru += "ч"; } else ru += "ц"; } else ru += "ц"; break; }; // ч\ц
			case 'e': { if (i + 1 < size) { if (line[i + 1] == 'h') { i++; ru += "э"; } else ru += "е"; } else ru += "е"; break; }; // э\е
			
			case 'j': { if (i + 1 < size) switch (line[i + 1]) {
			case 't': ru += "ъ"; i++; break;
			case 'e': ru += "ё"; i++; break;
			case 'h': ru += "ь"; i++; break;
			case 'u': ru += "ю"; i++; break;
			case 'a': ru += "я"; i++; break;
			case 's': ru += "щ"; i++; break;
			default: ru += "~"; break;
			} break; }; // none

			default: Debug::Log("Helper::TransliterationRU : unknown char! (" + std::string(1, line[i]) + ")", Warning); break;
			}
		}

		return ru; }
	std::string Helper::ToUpper(std::string s) { for (auto& a : s) a= toupper(a); return s; }
	std::string Helper::SummArray(std::vector<std::string>& _array, char space) {
		std::string summ = "";
		for (auto& a : _array) {
			summ += a + std::string(1, space);
		}
		return summ;
	}
	std::string Helper::SummArray(std::vector<Consequence>& _array, char space) {
		std::string summ = "";
		for (auto& a : _array) {
			summ += a.name + std::string(1, space);
		}
		return summ;
	}
	std::string Helper::Transliteration(std::string line, bool inRus,bool errors) {
		std::string en;
		for (auto& c: line) {
			en += TransliterationEN(c, errors);
		}
		//for (int i = 0; i < en.size(); i++)
		//	en[i] = tolower(en[i]);
		return en;
	}
	std::vector<std::string> Helper::Split(std::string text, std::string chr, std::string end, size_t& index) {
		std::vector<std::string> arr;
		std::size_t prev = index;
		std::size_t next;
		std::size_t delta = chr.length();

		if (end != "")
			if ((next = text.find(end, prev)) != std::string::npos) {
				next = next + index;
				if (text[next] == end[0]) {
					text.resize(next); index = next + 1;
				}
				else index = -1;
			}
			else index = -1;

		while ((next = text.find(chr, prev)) != std::string::npos) {
			arr.push_back(text.substr(prev, next - prev));
			prev = next + delta;
		}
		arr.push_back(text.substr(prev));

		return arr;
	}
	std::vector<std::string> Helper::Split(std::string text, std::string chr) {
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

	int Synapse::IndexOfSynapse(std::vector<std::tuple<std::string, int, double>>& s, std::string name) {
		for (size_t i = 0; i < s.size(); i++)
			if (std::get<0>(s[i]) == name)
				return i; // Found!
		return -1; }
	int Synapse::IndexOfSynapse(std::vector<std::tuple<std::string, double>>& s, std::string name) {
		for (size_t i = 0; i < s.size(); i++)
			if (std::get<0>(s[i]) == name)
				return i; // Found!
		return -1; } 
	int Synapse::IndexOfSynapse(std::vector<std::tuple<std::string, std::string, double, int>>& s, std::string name) {
		for (size_t i = 0; i < s.size(); i++)
			if (std::get<0>(s[i]) == name)
				return i; // Found!
		return -1; }

	int Synapse::FindGoodSynapse(std::vector<std::tuple<std::string, int, double>>& s) {
		if (s.size() == 0) return -1;
		int point = 0; double value = 0;

		for (size_t i = 0; i < s.size(); i++)
			if (std::get<2>(s[i]) > value) {
				value = std::get<2>(s[i]);
				point = i; }

		return point; }
	int Synapse::FindGoodSynapse(std::vector<std::tuple<std::string, std::string, double>>& s, size_t from_index) {
		if (s.size() == 0) return -1;
		if (s.size() - 1 < from_index) {
			Debug::Log("Helper::FindGoodSynapse : from_index > size!", Warning);
			return -1; }

		int point = 0; double value = 0;

		for (size_t i = from_index; i < s.size(); i++)
			if (std::get<2>(s[i]) > value) {
				value = std::get<2>(s[i]);
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

	void Synapse::FindAndSummSensiv(Consequence& con, std::string& name, std::string* sens, double hp, bool addNew) /* 1 - name, 2 - sensiv */ {
		if (sens == NULL) { Debug::Log("Synapse::FindAndSummSensiv : sensivity is NULL!", Error); Sleep(1000); return; }
		if (typeid(*sens).name() != typeid(std::string).name()) { Debug::Log("Synapse::FindAndSummSensiv : sensivity var are not string type!", Error); Sleep(1000); return; }
		
		std::string s = (*sens);
		try{
		if (s.size() < 2) {
			Debug::Log("Synapse::FindAndSummSensiv : sens.size() < 2! \n\tConq : "+con.name + "\n\tName : " + name + "\n\tSens : " + *sens, Warning);
			return; }
		} catch (...) { Debug::Log("Synapse::FindAndSummSensiv : fatal error!", Error); return; }
		if (con.self) Debug::Log("Synapse::FindAndSummSensiv : Self event \"" + con.name + "\" [" + *sens + "]");

		double max = 0; size_t index = 0;
		for (size_t t = 0; t < con.PerhapsWill.size(); t++) {
			if (con.GetPW_Name(t) == name) {
				std::string p = con.GetPW_Sens(t);
				double var = GetPercent(*sens, p);
				if (var > max) { index = t; max = var; }
			}
		}
		if (max > 70) { // 90
			//con.PerhapsWill[index].get<2>() = (con.PerhapsWill[index].get<2>() + hp) / 2;
			con.GetPW_HP(index) = Synapse::Summ(con.GetPW_HP(index), hp);
			con.GetPW_Meet(index)++; // Increment
			if (Settings::EventsProcessigDebug) Debug::Log("Synapse::FindAndSummSensiv : summ perhaps will \"" + name + "\" to conseq \"" + con.name +
				"\"; perc:"+std::to_string(max) + "; sens:"+*sens + "="+ con.GetPW_Sens(index)); }
		else if(addNew) {
			if (Settings::EventsProcessigDebug) Debug::Log("Synapse::FindAndSummSensiv : add perhaps will \""+name+"\" to conseq \""+con.name + "\"");
			con.PerhapsWill.push_back(std::tuple<std::string, std::string, double, int>(name, *sens, hp, 1)); }
	}
	std::string SpaRcle::Synapse::GetSensivityCauses(std::vector<std::tuple<std::string, int, double>>& s) {
		std::string Sensiv;
		for (auto& a : s) {
			auto&b = std::get<0>(a);
			if (b != Settings::EmptyName) {
				if (b[0] != '.')
					if (count_word_in_sensiv == 1) {
						if (b[1] == '/')
							Sensiv += b[2]; // [2]
						else
							Sensiv += b[0]; //[0]
					}
					else {
						if (b[1] == '/') 
							Sensiv += b[2]; // [2]
						else					  
							Sensiv += b[0]; //[0]

						Sensiv += b[b.size() - 1];
					}
			}
			else
				Sensiv += ".";
		}
		return Sensiv; // TODO : Возможно следует доделать, ну или обратить внимание в будущем.
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
		return Sensiv; // TODO : Возможно следует доделать, ну или обратить внимание в будущем.
	}
	std::string SpaRcle::Synapse::ClearSensivity(std::string& sensiv) {
		std::string clean;
		for (auto& a : sensiv) if (a != '.') clean += a;
		return clean; }
	double SpaRcle::Synapse::SimilarityPercentage(std::string first, std::string second, bool lenght, bool normalize) {
		///\TODO clear code!!!

		//TODO : при hm hmh схожесть 0!!!!!!
		//Debug::Log(std::to_string(first.size()) + "_" + std::to_string(second.size()));
		//Debug::Log(first + "_" + second);
		//double trueVal;
		//size_t size;
		short mode = 0;
		short modifer = 0;

		// Сравниваем относительно первого, то-есть, процент схожести будет говорить насколько второй похож на первый.
		if (second.size() > first.size()) mode = 1;
		else if (first.size() > second.size()) mode = -1;
		//else { //trueVal = 100.f / first.size(); //size = first.size(); 
		//}

		if (mode == 1) {
			if (lenght) {
				modifer = short(second.size() - first.size());
				//second += std::string(Settings::TrueSymbol, modifer);
				second += std::string(size_t(modifer), Settings::TrueSymbol);
			}

			/// \todo 
			if (normalize) second = second.substr(second.size() - first.size());
			//trueVal = 100.f / second.size(); //size = first.size();
		}
		else if (mode == -1) {
			if (lenght) {
				modifer = short(first.size() - second.size());
				//first += std::string(Settings::TrueSymbol, modifer);
				first += std::string(size_t(modifer), Settings::TrueSymbol);
			}

			/// \todo 
			if (normalize) first = first.substr(first.size() - second.size());
			//trueVal = 100.f / first.size(); //size = second.size();
		}

		double finaly = GetPercent(first, second, modifer);
		first.clear(); first.~basic_string();
		second.clear(); second.~basic_string();
		return finaly;
	}
}