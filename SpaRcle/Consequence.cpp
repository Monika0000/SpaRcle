#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Consequence.h"
#include "Settings.h"
#include "Debug.h"
#include "Helper.h"
#include <direct.h>

namespace SpaRcle {
	bool Consequence::Save(Consequence* conseq, const bool Diagnostic)
	{
		std::string p;
		if (!Diagnostic)
			p = Settings::Logic; // path to logic
		else
			p = Settings::SysDir + "\\TestingData";

		if (!Helper::DirExists(p)) _mkdir(p.c_str()); // if not exists - make path
		p += "\\"; p += ToString(conseq->action.type); // add additional path
		if (!Helper::DirExists(p)) _mkdir(p.c_str()); // if not exists - make path
		if (!Diagnostic)
			p += "\\" + conseq->name + Settings::exp_conseq; // set file name
		else
			p += "\\debug_" + conseq->name + Settings::exp_conseq; // set file name

		try {
			std::ofstream fout;

			fout.open(p);

			if (!fout.is_open()) {
				Debug::Log("Consequence::Save = Cant't saving file! \n\tPath : " + p);
				return false;
			}

			//double bad = Round((*conseq).Bad);
			//std::cout << (*conseq).Bad << std::endl;
			//std::cout << bad << std::endl;
			//std::cout << Round((*conseq).Bad) << std::endl;
			//std::cout << std::to_string(bad) << std::endl;
			std::string bad = Helper::Remove(std::to_string((*conseq).Bad), Settings::SaveNumbers);
			if (bad[bad.size() - 1] == ',') bad.resize(bad.size() - 2);

			std::string good = Helper::Remove(std::to_string((*conseq).Good), Settings::SaveNumbers);
			if (good[good.size() - 1] == ',') good.resize(good.size() - 2);

			fout << "h:" << bad << ";" << good << std::endl;
			fout << "m:" << (*conseq).meetings << std::endl;

			if ((*conseq).Causes.size() > 0)
			{
				fout << "cas:" << (*conseq).Causes.size() << std::endl;
				for (size_t i = 0; i < (*conseq).Causes.size(); i++)
					fout << (*conseq).Causes[i].get<0>() << ";" << (*conseq).Causes[i].get<1>() << ";" << (*conseq).Causes[i].get<2>() << std::endl;
			} // Запись причин в файл

			if ((*conseq).PerhapsWill.size() > 0)
			{
				fout << "prw:" << (*conseq).PerhapsWill.size() << std::endl;
				for (size_t i = 0; i < (*conseq).PerhapsWill.size(); i++)
					fout << (*conseq).PerhapsWill[i].get<0>() << ";" << (*conseq).PerhapsWill[i].get<1>() << ";"
					<< (*conseq).PerhapsWill[i].get<2>() << ";" << (*conseq).PerhapsWill[i].get<3>() << std::endl;
			} // Запись вероятных последущих следствий

			if ((*conseq).Synapses.size() > 0)
			{
				fout << "syn:" << (*conseq).Synapses.size() << std::endl;
				for (size_t i = 0; i < (*conseq).Synapses.size(); i++) {
					//fout << (*conseq).Synapses[i].get<0>() << ";" << (*conseq).Synapses[i].get<1>() << ";" << (*conseq).Synapses[i].get<2>() << std::endl;
					fout << (*conseq).Synapses[i].get<0>() << ";" << (*conseq).Synapses[i].get<1>() << std::endl;
				}
			} // Запись синапсов в файл

			fout << conseq->action.GetSaveData();
			fout.close();
			return true;
		}
		catch (...) {
			Debug::Log("SpaRcle::Consequence::Save : An exception has occured!", Error);
			return false;
		}
	}
	bool Consequence::Save(const bool Diagnostic) { return Save(this, Diagnostic); }

	bool SpaRcle::Consequence::Load(std::string name, AType atype)
	{
		return this->Load(name, atype, true, false);
	}
	bool Consequence::Load(std::string name, AType atype, bool notFoundIsError, bool Diagnostic)
	{
		std::string path;
		if (!Diagnostic)
			path = Settings::Logic + "\\" + ToString(atype) + "\\" + name + Settings::exp_conseq;
		else
			path = Settings::SysDir + "\\TestingData\\" + ToString(atype) + "\\" + name + Settings::exp_conseq;

		std::ifstream fin;
		try { fin.open(path); }
		catch (...) { Debug::Log("Consequence::Load : Openning failed! \n	" + path, Error); }

		if (!fin.is_open()) {
			if (notFoundIsError)
				Debug::Log("SpaRcle::Consequence::Load : Loading failed! \n	" + path, Error);
			return false;
		}

		bool findType = false;
		this->name = name;
		while (!fin.eof()) {
			try {
				std::string line;
				std::getline(fin, line);
				if (!findType) {
					int n;
					std::string pref = ReadUpToChar(line, ':', n);
					std::string post = line.substr(n);

					SWITCH(pref)
					{
						CASE("m") : {
							meetings = std::atoi(post.c_str());
							//sreturn false;
							break;
						}
						CASE("cas") : {
							size_t leng = std::atoi(post.c_str());
							for (size_t i = 0; i < leng; i++)
							{
								boost::tuple<std::string, int, double> t;
								std::string l; int n2;
								std::getline(fin, l);

								t.get<0>() = ReadUpToChar(l, ';', n2);
								l = l.substr(n2);

								t.get<1>() = std::atoi(ReadUpToChar(l, ';', n2).c_str());
								l = l.substr(n2);
								t.get<2>() = std::stod(l);
								Causes.push_back(t);
							}
							break;
						}
						CASE("prw") : {
							size_t leng = std::atoi(post.c_str());
							for (size_t i = 0; i < leng; i++)
							{
								boost::tuple<std::string, std::string, double, int> t;
								std::string l; int n2;

								std::getline(fin, l);
								t.get<0>() = ReadUpToChar(l, ';', n2);
								l = l.substr(n2);
								t.get<1>() = ReadUpToChar(l, ';', n2);
								l = l.substr(n2);
								t.get<2>() = std::stof(ReadUpToChar(l, ';', n2));
								l = l.substr(n2);
								t.get<3>() = std::atoi(l.c_str());
								PerhapsWill.push_back(t);
							}
							break;
						}
						CASE("syn") : {
							size_t leng = std::atoi(post.c_str());
							for (size_t i = 0; i < leng; i++)
							{
								//boost::tuple<std::string, std::string, double> t;
								boost::tuple<std::string, double> t;
								std::string l; int n2;

								std::getline(fin, l);
								t.get<0>() = ReadUpToChar(l, ';', n2);
								l = l.substr(n2);
								//t.get<1>() = ReadUpToChar(l, ';', n2);
								//l = l.substr(n2);
								//t.get<2>() = std::stof(post.c_str());
								t.get<1>() = std::stof(post.c_str());
								Synapses.push_back(t);
							}
							break;
						}
						CASE("h") :{
							int n2;
							std::string bad = ReadUpToChar(post, ';', n2);

							Bad = std::stof(bad);
							Good = std::stof(post.substr(n2));
							break;
						}

						CASE("t") :{
							if (post == "Speech")
								action.type = Speech;
							else if (post == "Visual")
								action.type = VisualData;
							else
								Debug::Log("SpaRcle::Consequence::Load::CASE(t) = WARNING : Unknown type! \n\tPath : " + path +
									"\n\tLine : " + line +
									"\n\tSymbol : " + post, Warning);
							//std::cout << "SWITCH::CASE : t" << std::endl;
							findType = true;
							break;
						}

					DEFAULT:
						Debug::Log("SpaRcle::Consequence::Load::SWITCH = WARNING : Uncorrect char! \n\tPath : " + path +
							"\n\tLine : " + line +
							"\n\tSymbol : " + pref, Warning);
						break;
					}
				}
				else
					if (!action.ApplyLine(line))
						std::cout << "\n\t" + path << std::endl;
			}
			catch (...) {
				Debug::Log("Consequence::Load : Loading failed! \n\t" + path, Error);
				Settings::Status = -3;
				fin.close();
				return false;
			}
		}
		fin.close();

		return true;
	}

	Consequence::Consequence() {
		meetings = 1;
		EventData = DataTime();
		action = Action();
	}
	Consequence::Consequence(std::string name)
	{
		this->name = name;
		this->action = Action();
		this->EventData = DataTime();
		this->meetings = 1;
	}
	Consequence::Consequence(std::string name, AType atype)
	{
		Load(name, atype);
	}
	Consequence::Consequence(Sound speech)
	{
		this->name = speech.text;
		this->meetings = 1;
		this->EventData = DataTime();
		this->action = Action(speech);
	}
	Consequence::Consequence(Visual visual)
	{
		this->name = visual.tag;
		this->meetings = 1;
		this->EventData = DataTime();
		this->action = Action(visual);
	}
	Consequence::Consequence(std::string name, Action action) {
		this->name = name;
		this->action = action;
		this->EventData = DataTime();
		this->meetings = 1;
	}
	Consequence::Consequence(std::string name, Action action, int meets) {
		this->name = name;
		this->action = action;
		this->meetings = meets;
		this->EventData = DataTime();
	}
	Consequence::Consequence(std::string name, Action action, int meets, double Bad, double Good) {
		this->name = name;
		this->action = action;
		this->meetings = meets;
		this->Good = Good;
		this->Bad = Bad;
		this->EventData = DataTime();
	}
	Consequence::~Consequence() { }

	const Consequence operator+(Consequence& left, Consequence& right)
	{
		/* INFO : Приоритет идет следущим образом : TODO преписать описание!!!!!!!!!!!!
				  Action = right
				  DataTime = right
				  PerhapsWill<Sensiv> = left + right
				  Synapses<Sensiv> = left + right
		*/

		Consequence result; //double div = 1.5f;

		result.action = left.action;
		if (!Helper::SummActionConseq(result, right))
			return left;

		result.name = left.name; // берем имя левого элемента (без разницы, они идентичны, в противном случае - ошибка выше)
		result.Bad = (left.Bad + right.Bad) / Div;
		result.Good = (left.Good + right.Good) / Div;
		result.EventData = left.EventData; // INFO: possible BUG
		result.meetings = left.meetings + right.meetings;

#pragma region [========== Summ ==========]
#if 0 
		result.Causes = left.Causes; /* Складываем причины, с учетом поиска идентичных элементов */
		if (result.Causes.size() == 0) result.Causes = right.Causes;
		else for (size_t i = 0; i < right.Causes.size(); i++) {
			int indx = Helper::IndexOfSynapse(result.Causes, right.Causes[i].get<0>());
			if (indx == -1)
				result.Causes.push_back(right.Causes[i]);
			else {
				result.Causes[indx].get<1>() = result.Causes[indx].get<1>() + right.Causes[i].get<1>(); // Суммируем количество встреч данного события

				result.Causes[indx].get<2>() = (result.Causes[indx].get<2>() + right.Causes[i].get<2>()) / div; /* Суммируем полезность */
			}
		}

		result.PerhapsWill = left.PerhapsWill; /* Складываем будущие следствия, с учетом поиска идентичных элементов */
		if (result.PerhapsWill.size() == 0) result.PerhapsWill = right.PerhapsWill;
		else for (size_t i = 0; i < right.PerhapsWill.size(); i++) {
			int indx = Helper::IndexOfSynapse(result.PerhapsWill, right.PerhapsWill[i].get<0>());
			if (indx == -1) result.PerhapsWill.push_back(right.PerhapsWill[i]);
			else {
				result.PerhapsWill[indx].get<1>() = Helper::SummSensivity(result.PerhapsWill[indx].get<1>(), right.PerhapsWill[i].get<1>()); // Суммируем ситуацию данных событий
				result.PerhapsWill[indx].get<2>() = (result.PerhapsWill[indx].get<2>() + right.PerhapsWill[i].get<2>()) / div; /* Суммируем полезность */
				// TODO : Нужно складывать ситуацию - то, что было до синапса
				// 0 - Name, 1 - Sensiv, 2 - Helpfulness
			}
		}

		result.Synapses = left.Synapses; /* Складываем синапсы, с учетом поиска идентичных элементов */
		if (result.Synapses.size() == 0) result.Synapses = right.Synapses;
		else for (size_t i = 0; i < right.Synapses.size(); i++) {
			int indx = Helper::IndexOfSynapse(result.Synapses, right.Synapses[i].get<0>());
			if (indx == -1) result.Synapses.push_back(right.Synapses[i]);
			else {
				result.Synapses[indx].get<1>() = Helper::SummSensivity(result.Synapses[indx].get<1>(), right.Synapses[i].get<1>()); // Суммируем ситуацию данных событий
				result.Synapses[indx].get<2>() = (result.Synapses[indx].get<2>() + right.Synapses[i].get<2>()) / div; /* Суммируем полезность */
				// TODO : Нужно складывать ситуацию - то, что было до синапса
				// 0 - Name, 1 - Sensiv, 2 - Helpfulness
			}
		}
#endif 

		Helper::SimpleSummConseq(result, right);
#pragma endregion

		return result;
	}
}