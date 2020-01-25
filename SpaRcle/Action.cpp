#include "pch.h"
#include "Action.h"
#include "Sound.h"
#include "Helper.h"
#include "Consequence.h"

#include <fstream>
#include <iostream>
#include <conio.h>   //необходимо для getch()
#include <filesystem>

namespace SpaRcle {
	Action::Action() { type = AType::Undefined; }

	Action::Action(Sound sound) {
		this->sound = sound;
		this->type = AType::Speech;
	}
	Action::Action(Motion motion) {
		this->motion = motion;
		this->type = AType::Move;
	}
	Action::Action(Visual visual) {
		this->visual = visual;
		this->type = AType::VisualData;
	}

	Action::~Action() {
		this->sound.~Sound();
		this->visual.~Visual();
		this->motion.~Motion();
	}

	/*
	void Action::Save(std::string path) {
		std::ofstream fout; path = Settings::SysDir + "\\Commands\\" + path + ".act";
		fout.open(path);
		Debug::Log(path);

		if (!fout.is_open())
			Debug::Log("Action::Save = Cant't saving file! \n\tPath : " + path, Error);

		fout << this->GetSaveData() << std::endl;
		fout.close(); }
	*/

	std::string Action::GetSaveData(Action* action) {
		std::string data;
		data += "t:"; data += ToString(action->type); data += "\n";

		switch (action->type) {
		case AType::Undefined: {
			Debug::Log("Action::GetSaveData : trying save undefined action!", Error);
			break;
		}
		case AType::Speech: {
			data += "sd:" + action->sound.text + "\n";
			data += "pr:" + Helper::Remove(std::to_string(action->sound.tone), Settings::SaveNumbers) + ";" + Helper::Remove(std::to_string(action->sound.volime),
				Settings::SaveNumbers);
			break;
		}
		case AType::VisualData: {
			data += "vs:" + action->visual.tag; // visual
			break;
		}
		case AType::Move: {
			data += "pt:" + action->motion.part + "\n"; // part
			data += "vl:" + std::to_string(action->motion.value); // value
			break;
		}
		default:
			Debug::Log("Action::GetSaveData : Unknown type!", Error);
			return "[Error]";
		}

		return data;
	}
	std::string Action::GetSaveData() { return GetSaveData(this); }

	bool Action::ApplyLine(std::string line) {
		short n;
		std::string pref = ReadUpToChar(line, ':', n);
		std::string post = line.substr(n);
		SWITCH(pref)
		{
			///^SOUND
			CASE("sd") :{ sound.text = post; break; }
			CASE("pr") :{
				short n2; std::string t = ReadUpToChar(post, ';', n2);
				sound.tone = std::stof(t);
				sound.volime = std::stof(post.substr(n2));
				t.clear(); t.~basic_string();
				break; }
			///^SOUND


			///^VISUAL
			CASE("vs") :{ visual.tag = post; break; }
			///^VISUAL

			///^MOTION
			CASE("pt") :{ motion.part = post; break; }
			CASE("vl") :{ motion.value = std::stof(post); break; }
			///^MOTION
		DEFAULT: {
			Debug::Log("SpaRcle::Action::ApplyLine::SWITCH = WARNING : Uncorrect char! \n\tLine : " + line +
				"\n\tSymbol : " + pref, Warning);
			return false;
			}
		}
		pref.clear(); pref.~basic_string();
		post.clear(); post.~basic_string();
		line.clear(); line.~basic_string();
		return true;
	}

	void Read(FILE* pFile, size_t& offs) {
		char* buf = NULL;
		fread(buf, 0, SEEK_END, pFile);
		offs += sizeof(buf);// +1;
	}
	char* appendCharToCharArray(char* array, char a)
	{
		//size_t len = strlen(array);
		size_t len = sizeof(array);

		char* ret = new char[len + 2];

		strcpy(ret, array);
		ret[len] = a;
		ret[len + 1] = '\0';

		return ret;
	}
	void Write(FILE* pFile, std::string line) {

	}

	static int _cdecl r;

	void Action::SaveNeuron(std::string& synapse, Consequence& conq, std::string& situation) {
		//Debug::Log("Action::SaveNeuron : saving :\n\t\tevent = " + conq.name + ";\n\t\tsyn = " + synapse + ";\n\t\tsit = " + situation, DType::System);
		
		//std::string hash = "~" + std::to_string(std::hash<std::string>{}(conq.name + situation));
		std::string hash = "~" + conq.name + "-" + situation;

		size_t index = 0; std::string temp, line, line2;

		bool find = false;

		std::string p = Settings::Actions;

		std::ofstream fout; std::ifstream fint;

		if (!Helper::DirExists(p)) r = _mkdir(p.c_str()); // if not exists - make path
		p += "\\"; p += ToString(conq.action.type); // add additional path
		if (!Helper::DirExists(p)) r = _mkdir(p.c_str()); // if not exists - make path
		p += "\\" + conq.name + Settings::exp_action; // set file name

		try {
			fint.open(p);

			if (!fint.is_open()) {
				fint.close();
				FILE* file = fopen(p.c_str(), "wb"); fclose(file);
				fint.open(p);
			}

			while (!fint.eof()) {
				std::getline(fint, line);
				index++;
				if (!line.empty())
					temp += line + "\n";
				if (line == hash) { find = true; break; }
			}
			if (!find) temp += hash + "\n";

			find = false;
			while (!fint.eof()) {
				std::getline(fint, line2);
				if (line2[0] == '~') { break; }

				switch (conq.action.type) {
					case AType::VisualData: {
						std::vector<std::string> spl = Helper::Split(line, ";");
						if (spl[0] == conq.action.visual.tag) {
							if (spl[1] == std::to_string(conq.action.visual.pos)) {
								find = true;
							}
						}
						spl.clear(); break; };
					case AType::Move: {
						find = true;
						break; };
					case AType::Speech: {
						//temp += (Sound(conq.action)).text;
						find = true;
						break; };
					default:
						Debug::Log("Action::SaveNeuron : unknown type!");
						break;
				}

				if (find) break;
				else if(!line2.empty()) temp += line2 + "\n";
			}
			
			if (!find) {
				switch (conq.action.type) {
				case AType::VisualData: {
					Visual v = conq.action.visual;
					temp += v.tag + ";" + std::to_string(v.pos);
					v.~Visual(); break; };
				case AType::Move: {
					Motion m = conq.action.motion;
					temp += m.part + ";" + std::to_string(m.value);
					m.~Motion(); break; };
				case AType::Speech: {
					Sound s = conq.action.sound;
					//temp += s.text + ";" + std::to_string(s.volime) + ";" + std::to_string(s.tone);
					temp += std::to_string(s.volime) + ";" + std::to_string(s.tone);
					s.~Sound(); break; };
				default: Debug::Log("Action::SaveNeuron : unknown type!"); break;
				}
				temp += ";" + synapse + "\n";
			}

			if(!line2.empty() && line2 != "") temp += line2 + "\n";

			while (!fint.eof()) {
				std::getline(fint, line); if (!line.empty()) temp += line + "\n";
			}

			fint.close();

			fout.open(p);
			fout << temp;
			fout.close();

			/*
			FILE* pFile;
			pFile = fopen(p.c_str(), "r+");//открываем для правки
			if (pFile != NULL)//проверяем что удалось открыть {
				//fseek(pFile, 3, SEEK_SET);//в файле pFile перемещаемся на 2 позиции вперёд относительно начала файла
				Write(pFile, "Hellox=X");
				fclose(pFile); }
			*/
		}
		catch (...) {
			Debug::Log("Action::SaveNeuron : Unknown error! \n\tPath : " + p, DType::Error);
			return;
		}

		p.clear(); p.~basic_string();
	}
	
	Neuron* Action::LoadNeuron(std::string name, AType type, size_t hash) {

		return nullptr;
	}

	Neuron::Neuron()
	{
	}

	Neuron::~Neuron()
	{
	}

	/*
	bool Action::Save(std::string path, Action& action) { // 

		switch (action.type)
		{
		case AType::Speech: {
			//Sound s = action->GetSound();
			Sound s = action;
			Debug::Log("Save A : " + s.text, Log);
			return true;
		}
		default:
			Debug::Log("Action::Save : Unknown type! \"" + path + "\"", Error);
			return false;
		}
	}

	bool Action::Save(std::string path) {
		return Save(path, *this);
	}
	*/
}