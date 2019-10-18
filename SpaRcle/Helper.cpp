#include "pch.h"
#include "Helper.h"
#include "DataTime.h"
#include <tchar.h>

namespace SpaRcle {
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

	std::string Helper::NumberToWord(int number)
	{
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

		default:
			Debug::Log("Helper::NumberToWord : ERROR!", Error);
			break;
		}
		return "[ERROR]";
	}

	std::string Helper::Remove(std::string text, int index)
	{
		if ((size_t)index >= text.size())
			index = text.size();
		std::string str;
		for (int i = 0; i < index; i++)
			str += text[i];
		return str;
	}

	bool Helper::DirExists(std::string dir)
	{
		std::wstring stemp = s2ws(dir);

		DWORD flag = GetFileAttributes(stemp.c_str());
		if (flag == 0xFFFFFFFFUL) {
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
				return false;
		}
		if (!(flag & FILE_ATTRIBUTE_DIRECTORY))
			return false;
		return true;

	}

	///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

	int Helper::IndexOfSynapse(std::vector<boost::tuple<std::string, int, double>>& s, std::string name)
	{
		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<0>() == name)
				return i; // Found!
		return -1; // Not found!
	}
	int Helper::IndexOfSynapse(std::vector<boost::tuple<std::string,  double>>& s, std::string name)
	{
		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<0>() == name)
				return i; // Found!
		return -1; // Not found!
	} //std::string,
	int Helper::IndexOfSynapse(std::vector<boost::tuple<std::string, std::string, double, int>>& s, std::string name)
	{
		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<0>() == name)
				return i; // Found!
		return -1; // Not found!
	}

	int Helper::FindGoodSynapse(std::vector<boost::tuple<std::string, int, double>>& s)
	{
		if(s.size() == 0)
			return -1;

		int point = 0;
		double value = 0;

		for (size_t i = 0; i < s.size(); i++)
			if (s[i].get<2>() > value)
			{
				value = s[i].get<2>();
				point = i;
			}

		return point;
	}
	int Helper::FindGoodSynapse(std::vector<boost::tuple<std::string, std::string, double>>& s, size_t from_index)
	{
		if (s.size() == 0) return -1;
		if (s.size() - 1 < from_index) {
			Debug::Log("Helper::FindGoodSynapse : from_index > size!", Warning);
			return -1;
		}

		int point = 0;
		double value = 0;

		for (size_t i = from_index; i < s.size(); i++)
			if (s[i].get<2>() > value)
			{
				value = s[i].get<2>();
				point = i;
			}

		return point;
	}

	///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

	bool SpaRcle::System::Save(std::string name, std::string data, std::string additionalPath)
	{
		std::ofstream fout;

		std::string p = Settings::SysDir; // path to system

		if (!Helper::DirExists(p)) _mkdir(p.c_str()); // if not exists - make path
		p += additionalPath; // add additional path
		if (!Helper::DirExists(p)) _mkdir(p.c_str()); // if not exists - make path
		p += "\\" + name; // set file name

		try {
			fout.open(p);

			if (!fout.is_open())
				return false;

			fout << data;

			fout.close();

			return true;
		}
		catch (...){
			Debug::Log("SpaRcle::System::Save : An exception has occured! \n\tPath : "+ p, Error);
			return false;
		}
	}
	bool SpaRcle::System::Load(std::string name, std::vector<std::string>& data, bool Delete)
	{
		std::string p = Settings::SysDir + "\\" + name; // path to system file
		try {
			std::ifstream fin; fin.open(p);

			if (!fin.is_open()) {
				Debug::Log("SpaRcle::System::Load : Loading failed! \n\tPath :" + p, Error);
				return false;
			}

			while (!fin.eof()) {
				std::string line;
				std::getline(fin, line);
				if(line == "" || line == " ")
				{ }
				else
					data.push_back(line);
			}

			fin.close();

			if(Delete)
				remove(p.c_str());

			return true;
		}
		catch (...) {
			Debug::Log("SpaRcle::System::Load : An exception has occured! \n\tPath : " + p, Error);
			return false;
		}
	}

	///%IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

	void Synapse::FindAndSummSensiv(Consequence& con, std::string name, std::string sens, double hp) // 1 - name, 2 - sensiv
	{
		if (sens.size() < 2)
			return;

		double max = 0; size_t index = 0;
		for (size_t t = 0; t < con.PerhapsWill.size(); t++)
		{
			std::string p = con.PerhapsWill[t].get<1>();
			double var = GetPercent(sens, p);
			if (var > max) {
				index = t; max = var;
			}
			//Helper::SummSensivity(con, index, Cause.get<1>(), ECom::PerhWill);
			//loaded.PerhapsWill[index].get<2>() = (loaded.PerhapsWill[index].get<2>() + Cause.get<3>()) / 2;
			//loaded.PerhapsWill[index].get<3>()++; // Increment
		}
		if (max > 90) {
			con.PerhapsWill[index].get<2>() = (con.PerhapsWill[index].get<2>() + hp) / 2;
			con.PerhapsWill[index].get<3>()++; // Increment
		}
		else
		{
			con.PerhapsWill.push_back(boost::tuple<std::string, std::string, double, int>(name, sens, hp, 1));
		}
	}
	std::string SpaRcle::Synapse::GetSensivityCauses(std::vector<boost::tuple<std::string, int, double>>& s)
	{
		std::string Sensiv;
		for (auto& a : s) {
			if (a.get<0>() != Settings::EmptyName) {
				if (a.get<0>()[0] != '.')
					if (count_word_in_sensiv == 1) {
						if (a.get<0>()[1] == '/')
							//Sensiv += a.get<0>().substr(2, count_word_in_sensiv + 2); // [2]
							Sensiv += a.get<0>()[2]; // [2]
						else
							//Sensiv += a.get<0>().substr(0, count_word_in_sensiv); //[0]
							Sensiv += a.get<0>()[0]; //[0]
					}
					else
					{
						if (a.get<0>()[1] == '/') { Sensiv += a.get<0>()[2]; Sensiv += a.get<0>()[a.get<0>().size() - 1]; }// [2]
						else { Sensiv += a.get<0>()[0]; Sensiv += a.get<0>()[a.get<0>().size() - 1]; }//[0]
					}
			}
			else
				Sensiv += ".";
		}
		return Sensiv; // TODO : Возможно следует доделать, ну или обратить внимание в будущем.
	}
	std::string SpaRcle::Synapse::GetSensivityCauses(std::vector<Consequence>& s, int to_index)
	{
		std::string Sensiv;

		if (to_index == -1)
			to_index = s.size() - 1;

		for (int i = 0; i <= to_index; i++) {
			if (s[i].name != Settings::EmptyName) {
				if (s[i].name[0] != '.')
					if (count_word_in_sensiv == 1) {
						if (s[i].name[1] == '/') Sensiv += s[i].name[2]; //Sensiv += s[i]. name.substr(2, Settings::count_sens + 2); // [2]
						else Sensiv += s[i].name[0];					 //. substr(0, Settings::count_sens); //[0]
					}
					else {
						if (s[i].name[1] == '/') { Sensiv += s[i].name[2]; Sensiv += s[i].name[s[i].name.size() - 1]; } // [2]
						else { Sensiv += s[i].name[0]; Sensiv += s[i].name[s[i].name.size() - 1]; }//[0]
					}
			}
			else
				Sensiv += ".";//std::string(count_word_in_sensiv, '.');
		}
		return Sensiv; // TODO : Возможно следует доделать, ну или обратить внимание в будущем.
	}
	bool SpaRcle::Synapse::SummSensivity(std::string& left, std::string& right, bool check) // std::string
	{
		if (right.size() > left.size())
		{
			//return (left + right.substr(left.size())); //TODO
			left = left + right.substr(left.size());
			return true;
		}
		else if (left.size() > right.size())
		{

		}

		if (check) {
			//if (left.size() == right.size()) {
				//size_t p = 0;
				//for (size_t t = 0; t < left.size(); t++)
				//	if (left[t] == right[t])
				//		p++;

				//Debug::Log("=========================================================", Error);
				//Debug::Log(100.f / left.size() * p, Error);

				//if (100.f / left.size() * p >= 90)
				//	return true;
				//else
				//	return false;
			//}
			//else {}
		}

		return true;
	}
	void Synapse::SummSensivity(Consequence& left, size_t index, std::string& right, ECom com)
	{
		if (com == ECom::PerhWill) {
			auto& a = left.PerhapsWill[index].get<1>();
			//if (a == "S/equal")
			if (SummSensivity(a, right, true))
			{
				//if (a.size() == right.size())
				//	if (GetPercent(a, right) < 75)
				//	{
				//		left.PerhapsWill.push_back(boost::tuple<std::string, std::string, double, int>(left.PerhapsWill[index].get<0>(), right,
				//			left.PerhapsWill[index].get<2>(), left.PerhapsWill[index].get<3>()));
				//	}
				if (count_word_in_sensiv == 1) {
					if (left.name[0] == right[right.size() - 1]) {
						for (size_t t = 0; t < a.size(); t++) {
							//if (a[t] != right[t]) a[t] = '*';
						}
					}
					//Debug::Log("dasdsdasdadasd");
					//	}

				}
			}
		}
		else if (com == ECom::Synp)
			/*
				Портит синапсы, добавляя в их имена чувствительность, которая была удалена в новых вурсиях из синапсов
			*/
			//if (!SummSensivity(left.Synapses[index].get<0>(), right, true)) 
			//{
				//left.Synapses.push_back(boost::tuple<std::string, std::string, double>(left.Synapses[index].get<0>(), right,
				//	left.Synapses[index].get<2>()));
			//}
		{

		}
	}
	std::string SpaRcle::Synapse::ClearSensivity(std::string& sensiv)
	{
		std::string clean;
		for (auto& a : sensiv)
			if (a != '.')
				clean += a;

		return clean;
	}
	double SpaRcle::Synapse::SimilarityPercentage(std::string first, std::string second, bool lenght, bool normalize)
	{
		//TODO : при hm hmh схожесть 0!!!!!!
		//Debug::Log(std::to_string(first.size()) + "_" + std::to_string(second.size()));
		//Debug::Log(first + "_" + second);
		double trueVal;
		//size_t size;
		short mode = 0;
		int modifer = 0;

		// Сравниваем относительно первого, то-есть, процент схожести будет говорить насколько второй похож на первый.
		if (second.size() > first.size()) mode = 1;
		else if (first.size() > second.size()) mode = -1;
		else { //trueVal = 100.f / first.size(); //size = first.size(); 
		}

		if (mode == 1) {
			if (lenght) {
				modifer = second.size() - first.size();
				second += std::string("_", modifer);
			}

			/// \todo 
			if (normalize) second = second.substr(second.size() - first.size());
			//trueVal = 100.f / second.size(); //size = first.size();
		}
		else if (mode == -1) {
			if (lenght) {
				modifer = first.size() - second.size();
				first += std::string("_", modifer);
			}

			/// \todo 
			if (normalize) first = first.substr(first.size() - second.size());
			//trueVal = 100.f / first.size(); //size = second.size();
		}

		//double percent = 0;
		//size_t max = second.size() >= first.size() ? first.size() : second.size();
		//Debug::Log(modifer);

		//Debug::Log(first + " " + second);


		/*
		for (size_t i = 0; i < max; i++) {
			if (first[i] == second[i])
				percent += trueVar;
			else if (i + 1 < max)
				if (first[i] == second[i + 1])
					percent += (trueVar / 1.5f);
		}
		*/

		return GetPercent(first, second, modifer);
	}
}