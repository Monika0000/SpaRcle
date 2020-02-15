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

	static int _cdecl r;
	//static std::ofstream fout; ///\TODO WTF it is STATIC?!?!?!?!?!?!?!?!??! 

	bool Consequence::Save(Consequence* conseq, const std::string region, const bool Diagnostic) {
		//Debug::Log("Save " + region + " = " + conseq->name, DType::System);

	ret: if (isWrite) { Debug::Log("Consequence::Save : file already use! ["+conseq->name+"]", Warning); Sleep(5); goto ret; }
		isWrite = true;

		std::ofstream fout;
		std::string p;
		if (!Diagnostic)
			p = Settings::Logic; // path to logic
		else
			p = Settings::SysDir + "\\TestingData";

		if (!Helper::DirExists(p)) r = _mkdir(p.c_str()); // if not exists - make path
		p += "\\"; p += ToString(conseq->action.type); // add additional path
		if (!Helper::DirExists(p)) r = _mkdir(p.c_str()); // if not exists - make path
		if (!Diagnostic)
			p += "\\" + conseq->name + Settings::exp_conseq; // set file name
		else
			p += "\\debug_" + conseq->name + Settings::exp_conseq; // set file name

		try {
			fout.open(p);

			if (!fout.is_open()) {
				Debug::Log("Consequence::Save = Cant't saving file! \n\tPath : " + p);
				isWrite = false;
				p.clear(); p.~basic_string();
				return false; }
			p.clear(); p.~basic_string();

			std::string bad = Helper::Remove(std::to_string((*conseq).Bad), Settings::SaveNumbers);
			if (bad[bad.size() - 1] == ',') bad.resize(bad.size() - 2);

			std::string good = Helper::Remove(std::to_string((*conseq).Good), Settings::SaveNumbers);
			if (good[good.size() - 1] == ',') good.resize(good.size() - 2);

			fout << "h:" << bad << ";" << good << std::endl << "m:" << (*conseq).meetings << std::endl;

			bad.clear(); bad.~basic_string();
			good.clear(); good.~basic_string();

			if ((*conseq).Causes.size() > 0)
			{
				fout << "cas:" << (*conseq).Causes.size() << std::endl;
				for (size_t i = 0; i < (*conseq).Causes.size(); i++)
					fout << std::get<0>((*conseq).Causes[i]) << ";" << std::get<1>((*conseq).Causes[i]) << ";" << std::get<2>((*conseq).Causes[i]) << std::endl;
			} // Запись причин в файл

			if ((*conseq).PerhapsWill.size() > 0)
			{
				fout << "prw:" << (*conseq).PerhapsWill.size() << std::endl;
				for (size_t i = 0; i < (*conseq).PerhapsWill.size(); i++)
					fout << std::get<0>((*conseq).PerhapsWill[i]) << ";" << std::get<1>((*conseq).PerhapsWill[i]) << ";"
					<< std::get<2>((*conseq).PerhapsWill[i]) << ";" << std::get<3>((*conseq).PerhapsWill[i]) << std::endl;
			} // Запись вероятных последущих следствий

			if ((*conseq).Synapses.size() > 0)
			{
				fout << "syn:" << (*conseq).Synapses.size() << std::endl;	
				for (size_t i = 0; i < (*conseq).Synapses.size(); i++) {
					//fout << (*conseq).Synapses[i].get<0>() << ";" << (*conseq).Synapses[i].get<1>() << ";" << (*conseq).Synapses[i].get<2>() << std::endl;
					//fout << std::get<0>((*conseq).Synapses[i]) << ";" << std::get<1>((*conseq).Synapses[i]) << std::endl;
					fout << std::get<0>((*conseq).Synapses[i]) << ";" << conseq->GetSN_HP(i) << std::endl;
				}
			} // Запись синапсов в файл

			//data += "t:"; data += ToString(action->type); data += "\n";
			//fout << conseq->action.GetSaveData();
			//fout << "t:" << ToString(conseq->action.type);
			fout.close(); 
			isWrite = false;
			p.clear(); p.~basic_string();
			return true;
		}
		catch (...) {
			Debug::Log("SpaRcle::Consequence::Save : An exception has occured! ["+conseq->name+"]", Error);
			isWrite = false;
			p.clear(); p.~basic_string();
			return false;
		}
	}
	bool Consequence::Save(const std::string region, const bool Diagnostic) { return Save(this, region, Diagnostic); }

	char SpaRcle::Consequence::Load(std::string name, AType atype, std::string Block, load_mode mode) {
		return this->Load(name, atype, true, false, "Load_" + Block, mode); }
	char Consequence::Load(std::string name, AType atype, bool notFoundIsError, bool Diagnostic, std::string Block, load_mode mode) {
		//Debug::Log("Load " + Block + " = " + name, DType::System);
		if (Settings::isUseMemory) {
			Consequence* con = Memory::GetMemory()->GetFragment(name, atype);
			if (con != NULL) {
				this->action = con->action;
				this->Bad = con->Bad;
				this->Causes = con->Causes;
				this->Good = con->Good;
				this->meetings = con->meetings;
				this->name = con->name;
				this->PerhapsWill = con->PerhapsWill;
				this->Synapses = con->Synapses;
				return true;
			}
			else {
				char result = LoadFile(name, atype, notFoundIsError, Diagnostic, Block, mode);
				if (result == 1) {
					Memory::GetMemory()->AddFragment(this);
					return 1;
				}
				else
					return result;
			}

		} 
		else
		 	return LoadFile(name, atype,notFoundIsError, Diagnostic, Block, mode);
	}
	char Consequence::LoadFile(std::string name, AType atype, bool notFoundIsError, bool Diagnostic, std::string Block, load_mode mode) {
	ret: if (isRead) { Debug::Log("Consequence::Load : file already use! [" + name + "] {"+ Block + "}", Warning); Sleep(60); goto ret; }
		isRead = true;
		std::string path;
		if (!Diagnostic)
			path = Settings::Logic + "\\" + ToString(atype) + "\\" + name + Settings::exp_conseq;
		else
			path = Settings::SysDir + "\\TestingData\\" + ToString(atype) + "\\" + name + Settings::exp_conseq;

		std::ifstream fin;
		try { fin.open(path); }
		catch (...) { Debug::Log("Consequence::Load (1) [" + Block + "] : Openning failed! \n	" + path, Error); fin.close(); path.clear(); path.~basic_string(); return -1; isRead = false; }

		if (!fin.is_open()) {
			if (notFoundIsError)
			Debug::Log("SpaRcle::Consequence::Load (2) [" + Block + "] : File is not exists! \n	" + path, Error);
			isRead = false; 
			fin.close(); path.clear(); path.~basic_string();
			return 0;
		}
		short n = 0, n2 = 0, number = 0, leng = 0; float method = 0;
		bool isBreak = false; //findType = false,
		this->name = name;

		std::string temp;
		std::tuple<std::string, std::string, double, int> t_prw; std::string l_prw; 
		std::tuple<std::string, int, double> t_cas;	std::string l_cas; 
		std::tuple<std::string, double> t_syn; std::string l_syn; 

		this->action.type = atype;

		while (!fin.eof() && !isBreak) {
			try {
				std::string line;
				std::getline(fin, line); number++;
				if (!line.empty()) {
					method = 0;
					std::string pref = ReadUpToChar(line, ':', n);
					std::string post = line.substr(n);

					SWITCH(pref) {
						CASE("h") :{ method = 1;
							std::string bad = ReadUpToChar(post, ';', n2);

							Bad = std::stof(bad);
							Good = std::stof(post.substr(n2));
							bad.clear(); bad.~basic_string();

							if (mode == load_mode::to_hp)
								isBreak = true;
							break;
						}
						CASE("m") : { method = 2;
							meetings = std::atoi(post.c_str());
							break;
						}
						CASE("cas") : { method = 3;
							leng = std::atoi(post.c_str());
							for (short i = 0; i < leng; i++) {
								std::getline(fin, l_cas); if(l_cas.empty()) std::getline(fin, l_cas); temp = l_cas;

								method = 3.1f;
								std::get<0>(t_cas) = ReadUpToChar(l_cas, ';', n2); 
								method = 3.15f;
								l_cas = l_cas.substr(n2);

								method = 3.2f;
								std::get<1>(t_cas) = std::atoi(ReadUpToChar(l_cas, ';', n2).c_str());
								l_cas = l_cas.substr(n2);

								method = 3.3f;
								std::get<2>(t_cas) = std::stod(l_cas);
								Causes.push_back(t_cas);

								number++;
							}
							break;
						}
						CASE("prw") : { method = 4;
							leng = std::atoi(post.c_str());
							for (short i = 0; i < leng; i++)
							{
								std::getline(fin, l_prw); if(l_prw.empty()) std::getline(fin, l_prw); temp = l_prw;

								method = 4.1f;
								std::get<0>(t_prw) = ReadUpToChar(l_prw, ';', n2);

								method = 4.2f;
								l_prw = l_prw.substr(n2);
								std::get<1>(t_prw) = ReadUpToChar(l_prw, ';', n2);

								method = 4.3f;
								l_prw = l_prw.substr(n2);
								std::get<2>(t_prw) = std::stof(ReadUpToChar(l_prw, ';', n2, 8));

								method = 4.4f;
								l_prw = l_prw.substr(n2);
								std::get<3>(t_prw) = std::atoi(l_prw.c_str());

								method = 4.5f;
								PerhapsWill.push_back(t_prw); // Load 

								number++;
							}
							break;
						}
						CASE("syn") : { method = 5;
							leng = std::atoi(post.c_str());
							for (short i = 0; i < leng; i++)
							{
								std::getline(fin, l_syn);
								std::get<0>(t_syn) = ReadUpToChar(l_syn, ';', n2);
								//l_syn = l_syn.substr(n2);
								//std::get<1>(t_syn) = std::stod(post.c_str());
								std::get<1>(t_syn) = std::stod(l_syn.substr(n2));
								//Debug::Log(l_syn + " = " + std::get<0>(t_syn) + " " + std::to_string(std::get<1>(t_syn)) + " " + l_syn.substr(n2), DType::Info);

								Synapses.push_back(t_syn);

								number++;
							}
							break;
						}


						/*
						CASE("t") :{ method = 6;
							if (post == "Speech")
								action.type = AType::Speech;
							else if (post == "Visual")
								action.type = AType::VisualData;
							else if (post == "Move")
								action.type = AType::Move;
							else
								Debug::Log("SpaRcle::Consequence::Load::CASE(t) = WARNING : Unknown type! \n\tPath : " + path +
									"\n\tLine : " + line +
									"\n\tSymbol : " + post +
									"\n\tNumber : " + std::to_string(number), Warning);
							//std::cout << "SWITCH::CASE : t" << std::endl;
							findType = true;
							break; }
						*/
					DEFAULT:
						Debug::Log("SpaRcle::Consequence::Load::SWITCH = WARNING ["+ Block +"] : Uncorrect char! \n\tPath : " + path +
							"\n\tLine : " + line +
							"\n\tSymbol : " + pref +
							"\n\tNumber : " + std::to_string(number), Warning);
						break;
					}
				}
				//else
				//	if (!action.ApplyLine(line))
				//		std::cout << "\n\t" + path << std::endl;
			}
			catch (std::exception e) {
				Debug::Log("Consequence::Load (3) ["+Block+"] {"+std::to_string(number)+"} : Loading failed! \n\tPath : " + path
					+ "\n\tMethod = " + std::to_string(method) + "\n\tReason : "+ e.what()+"\n\tBlock : "+Block, Error);
				if (method >= 3 && method < 4) Debug::Log("Consequence::Load : cas = " + l_cas + "; temp = " + temp, DType::Error);
				if (method >= 4 && method < 5) Debug::Log("Consequence::Load : prw = " + l_prw + "; temp = " + temp, DType::Error);
				//Settings::IsActive = false;
				fin.close();
				path.clear(); path.~basic_string();
				this->Causes.clear(); 
				this->PerhapsWill.clear();
				this->Synapses.clear();

				Sleep(500);
				isRead = false;

				goto ret;
				//return -1;
			}
		}
		fin.close();
		isRead = false;

		path.~basic_string();
		l_prw.~basic_string();
		l_cas.~basic_string();
		l_syn.~basic_string();
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
	Consequence::Consequence(std::string name, AType atype, const std::string Block) { Load(name, atype,"Constructor_"+Block); }
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
	Consequence::Consequence(Motion move) {
		this->Bad = 0; this->Good = 0;
		this->name = move.part;
		this->meetings = 1;
		this->EventData = DataTime();
		this->action = Action(move); }
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