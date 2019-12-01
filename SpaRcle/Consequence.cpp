#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Consequence.h"
#include "Settings.h"
#include "Memory.h"
#include "Debug.h"
#include "Helper.h"
#include <direct.h>

namespace SpaRcle {
	bool Consequence::isWrite = false;
	bool Consequence::isRead = false;

	static std::ofstream fout;
	bool Consequence::Save(Consequence* conseq, const bool Diagnostic)
	{
	ret: if (isWrite) { Debug::Log("Consequence::Save : file already use! ["+conseq->name+"]", Warning); Sleep(1); goto ret; }
		isWrite = true;

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
			fout.open(p);

			if (!fout.is_open()) {
				Debug::Log("Consequence::Save = Cant't saving file! \n\tPath : " + p);
				isWrite = false;
				return false;
			}

			std::string bad = Helper::Remove(std::to_string((*conseq).Bad), Settings::SaveNumbers);
			if (bad[bad.size() - 1] == ',') bad.resize(bad.size() - 2);

			std::string good = Helper::Remove(std::to_string((*conseq).Good), Settings::SaveNumbers);
			if (good[good.size() - 1] == ',') good.resize(good.size() - 2);

			fout << "h:" << bad << ";" << good << std::endl << "m:" << (*conseq).meetings << std::endl;

			if ((*conseq).Causes.size() > 0)
			{
				fout << "cas:" << (*conseq).Causes.size() << std::endl;
				for (size_t i = 0; i < (*conseq).Causes.size(); i++)
					fout << (*conseq).Causes[i].get<0>() << ";" << (*conseq).Causes[i].get<1>() << ";" << (*conseq).Causes[i].get<2>() << std::endl;
			} // ������ ������ � ����

			if ((*conseq).PerhapsWill.size() > 0)
			{
				fout << "prw:" << (*conseq).PerhapsWill.size() << std::endl;
				for (size_t i = 0; i < (*conseq).PerhapsWill.size(); i++)
					fout << (*conseq).PerhapsWill[i].get<0>() << ";" << (*conseq).PerhapsWill[i].get<1>() << ";"
					<< (*conseq).PerhapsWill[i].get<2>() << ";" << (*conseq).PerhapsWill[i].get<3>() << std::endl;
			} // ������ ��������� ���������� ���������

			if ((*conseq).Synapses.size() > 0)
			{
				fout << "syn:" << (*conseq).Synapses.size() << std::endl;
				for (size_t i = 0; i < (*conseq).Synapses.size(); i++) {
					//fout << (*conseq).Synapses[i].get<0>() << ";" << (*conseq).Synapses[i].get<1>() << ";" << (*conseq).Synapses[i].get<2>() << std::endl;
					fout << (*conseq).Synapses[i].get<0>() << ";" << (*conseq).Synapses[i].get<1>() << std::endl;
				}
			} // ������ �������� � ����

			fout << conseq->action.GetSaveData();
			fout.close();
			isWrite = false;
			return true;
		}
		catch (...) {
			Debug::Log("SpaRcle::Consequence::Save : An exception has occured! ["+conseq->name+"]", Error);
			isWrite = false;
			return false;
		}
	}
	bool Consequence::Save(const bool Diagnostic) { return Save(this, Diagnostic); }

	char SpaRcle::Consequence::Load(std::string name, AType atype, std::string Block) {
		return this->Load(name, atype, true, false, "Load_" + Block); }
	char Consequence::Load(std::string name, AType atype, bool notFoundIsError, bool Diagnostic, std::string Block) {
	//ret: if (isRead) { Debug::Log("Consequence::Load : file already use!", Warning); Sleep(1); goto ret; }
		//if (Settings::isUseMemory) {
		//	auto* a = Memory::GetMemory()->GetFragment(name, atype);
		//	if (a != NULL) {
		//		Set(*a); }
		//}


		isRead = true;
		std::string path;
		if (!Diagnostic)
			path = Settings::Logic + "\\" + ToString(atype) + "\\" + name + Settings::exp_conseq;
		else
			path = Settings::SysDir + "\\TestingData\\" + ToString(atype) + "\\" + name + Settings::exp_conseq;

		std::ifstream fin;
		try { fin.open(path); }
		catch (...) { Debug::Log("Consequence::Load (1) [" + Block + "] : Openning failed! \n	" + path, Error); return -1; isRead = false; }

		if (!fin.is_open()) {
			if (notFoundIsError)
			Debug::Log("SpaRcle::Consequence::Load (2) [" + Block + "] : File is not exists! \n	" + path, Error);
			isRead = false;
			return 0;
		}
		short n = 0, n2 = 0, number = 0, leng = 0;
		bool findType = false;
		this->name = name;

		boost::tuple<std::string, std::string, double, int> t_prw;
		std::string l_prw;// int n2;

		boost::tuple<std::string, int, double> t_cas;
		std::string l_cas;// int n2;

		boost::tuple<std::string, double> t_syn;
		std::string l_syn; //int n2;

		while (!fin.eof()) {
			try {
				std::string line;
				std::getline(fin, line); number++;
				if (!findType) {
					//int n;
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
							leng = std::atoi(post.c_str());
							for (short i = 0; i < leng; i++)
							{
								std::getline(fin, l_cas);

								t_cas.get<0>() = ReadUpToChar(l_cas, ';', n2);
								l_cas = l_cas.substr(n2);

								t_cas.get<1>() = std::atoi(ReadUpToChar(l_cas, ';', n2).c_str());
								l_cas = l_cas.substr(n2);
								t_cas.get<2>() = std::stod(l_cas);
								Causes.push_back(t_cas);

								number++;
							}
							break;
						}
						CASE("prw") : {
							leng = std::atoi(post.c_str());
							for (short i = 0; i < leng; i++)
							{
								std::getline(fin, l_prw);
								t_prw.get<0>() = ReadUpToChar(l_prw, ';', n2);
								l_prw = l_prw.substr(n2);
								t_prw.get<1>() = ReadUpToChar(l_prw, ';', n2);
								l_prw = l_prw.substr(n2);
								t_prw.get<2>() = std::stof(ReadUpToChar(l_prw, ';', n2, 8));
								l_prw = l_prw.substr(n2);
								t_prw.get<3>() = std::atoi(l_prw.c_str());
								PerhapsWill.push_back(t_prw);

								number++;
							}
							break;
						}
						CASE("syn") : {
							leng = std::atoi(post.c_str());
							for (short i = 0; i < leng; i++)
							{
								std::getline(fin, l_syn);
								t_syn.get<0>() = ReadUpToChar(l_syn, ';', n2);
								//l_syn = l_syn.substr(n2);
								t_syn.get<1>() = std::stof(post.c_str());
								Synapses.push_back(t_syn);

								number++;
							}
							break;
						}
						CASE("h") :{
							//int n2;
							std::string bad = ReadUpToChar(post, ';', n2);

							Bad = std::stof(bad);
							Good = std::stof(post.substr(n2));
							bad.clear();
							bad.~basic_string();
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
									"\n\tSymbol : " + post +
									"\n\tNumber : " + std::to_string(number), Warning);
							//std::cout << "SWITCH::CASE : t" << std::endl;
							findType = true;
							break;
						}

					DEFAULT:
						Debug::Log("SpaRcle::Consequence::Load::SWITCH = WARNING : Uncorrect char! \n\tPath : " + path +
							"\n\tLine : " + line +
							"\n\tSymbol : " + pref +
							"\n\tNumber : " + std::to_string(number), Warning);
						break;
					}
				}
				else
					if (!action.ApplyLine(line))
						std::cout << "\n\t" + path << std::endl;
			}
			catch (...) {
				Debug::Log("Consequence::Load (3) ["+Block+"] {"+std::to_string(number)+"} : Loading failed! \n\t" + path , Error);
				Settings::IsActive = false;
				fin.close();
				isRead = false;
				return -1;
			}
		}
		fin.close();
		isRead = false;

		return true;
	}

	Consequence::Consequence() {
		this->Bad = 0; this->Good = 0;
		meetings = 1;
		EventData = DataTime();
		action = Action(); }
	//Consequence::Consequence(const Consequence& con) : meetings(con.meetings), action(con.action), Causes(con.Causes), PerhapsWill(con.PerhapsWill), 
	//	Synapses(con.Synapses), Bad(con.Bad), Good(con.Good), self(con.self), name(con.name), EventData(con.EventData) { }
	void Consequence::Set(const Consequence& con) {
		meetings = con.meetings; 
		action = con.action; 
		Causes = con.Causes;
		PerhapsWill = con.PerhapsWill;
		Synapses = con.Synapses;
		Bad = con.Bad;
		Good = con.Good;
		self = con.self;
		name = con.name;
		EventData = con.EventData; }
	Consequence::Consequence(std::string name) {
		this->Bad = 0; this->Good = 0;
		this->name = name;
		this->action = Action();
		this->EventData = DataTime();
		this->meetings = 1; }
	Consequence::Consequence(std::string name, AType atype) { Load(name, atype,"Constructor"); }
	Consequence::Consequence(Sound speech, bool self, float hp) {
		this->Bad = -hp;
		this->Good = hp;

		this->self = self;
		this->name = speech.text;
		this->meetings = 1;
		this->EventData = DataTime();
		this->action = Action(speech); }
	Consequence::Consequence(Visual visual) {
		this->Bad = 0; this->Good = 0;
		this->name = visual.tag;
		this->meetings = 1;
		this->EventData = DataTime();
		this->action = Action(visual); }
	Consequence::Consequence(std::string name, Action action) {
		this->Bad = 0; this->Good = 0;
		this->name = name;
		this->action = action;
		this->EventData = DataTime();
		this->meetings = 1; }
	Consequence::Consequence(std::string name, Action action, int meets) {
		this->Bad = 0; this->Good = 0;
		this->name = name;
		this->action = action;
		this->meetings = meets;
		this->EventData = DataTime(); }
	Consequence::Consequence(std::string name, Action action, int meets, double Bad, double Good) {
		this->name = name;
		this->action = action;
		this->meetings = meets;
		this->Good = Good;
		this->Bad = Bad;
		this->EventData = DataTime(); }
	Consequence::~Consequence() {
		this->Causes.clear();
		this->Causes.~vector();

		this->PerhapsWill.clear();
		this->PerhapsWill.~vector();

		this->Synapses.clear();
		this->Synapses.~vector();

		this->action.~Action();
		this->EventData.~DataTime();
		this->name.~basic_string(); }
}