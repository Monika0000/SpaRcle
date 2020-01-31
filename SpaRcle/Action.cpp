#include "pch.h"
#include "Action.h"
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
	

	// Neen to change/delete
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
	*/

	/*
	std::string Action::GetSaveData() { return GetSaveData(this); }

	bool Action::ApplyLine(std::string line) {
		short n;
		std::string pref = ReadUpToChar(line, ':', n);
		std::string post = line.substr(n);
		SWITCH(pref) {
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

	/*
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
	*/

	static int _cdecl r;

	bool Action::SetData(Neuron* nr, std::string& sit) {
		if (nr->size == 0)
			return false;

		int index = -1, temp = 0; float hp = 0.f;
		std::string temp_sit;

		if (Settings::ActionDebug) Debug::Log("Action::SetData : variants = " + std::to_string(nr->size), DType::Module);

		if (nr->size > 1) {
			//////////////////// [FIND THE BEST VARIANT] //////////////////// 

			for (size_t i = 0; i < nr->size; i++) {
				temp = (size_t)std::stoul(nr->sensitivities[i]);
				temp_sit = sit + Synapse::GetSensivityOfName(nr->data[i], false);
				size_t hash = Synapse::ToInt(temp_sit);

				if (temp == hash) {
					if(Settings::ActionDebug) Debug::Log("Action::SetData : find variant = " + nr->sensitivities[i] + " " +
						nr->data[i] + " " + std::to_string(nr->value_1[i]) + " " + std::to_string(nr->value_2[i]), DType::Module);
					index = (int)i; break;
				}
				else {
					if (Settings::ActionDebug) 
						Debug::Log("Action::SetData : check [" + temp_sit + "] (" + std::to_string(hash) + ") != " + std::to_string(temp), DType::Module);
				}
			}

			if (index == -1) {
				index = std::rand() % nr->size;
				if (Settings::ActionDebug) Debug::Log("Action::SetData : not found solution. Using random... [" + std::to_string(index) + "]", DType::Module);
			}
			if (index < 0 || index >= nr->size) { Debug::Log("Action::SetData : random error!", DType::Error); index = 0; }

			//////////////////// [FIND THE BEST VARIANT] //////////////////// 
		} else index = 0;

		switch (this->type) { 
		case SpaRcle::AType::Undefined:
			Debug::Log("Action::SetData : \"Undefined\" type!", DType::Error);
			break;
		case SpaRcle::AType::Speech:
			this->sound.volime = nr->value_1[index];
			this->sound.tone = nr->value_2[index];
			this->sound.text = nr->data[index];
			break;
		case SpaRcle::AType::Move:
			this->motion.part = nr->data[index];
			this->motion.value = nr->value_1[index];
			break;
		//case SpaRcle::AType::VisualData:
		//	break;
		default:
			Debug::Log("Action::SetData : unknown type! (TODO)", DType::Error);
			break;
		}
		delete nr;
		return true;
	}

	void Action::SaveNeuron(Consequence& conq, std::string& Situation) {
		Action::SaveNeuron(ToATypeChar(conq.action.type) + "/" + conq.name, conq, Situation, Situation, true);
	}

	void Action::SaveNeuron(std::string synapse, Consequence& conq, std::string& PW_situation, std::string Situation, bool Base) {
		//Debug::Log("Action::SaveNeuron : saving :\n\t\tevent = " + conq.name + ";\n\t\tsyn = " + synapse + ";\n\t\tsit = " + Situation, DType::System);

		if (conq.action.type == AType::Move) {
			Motion m = conq.action;
			if (m.part == Settings::EmptyName) {
				Debug::Log("Action::SaveNeuron : Empty part's name!!!!", DType::Error);
				Settings::IsActive = false;
			}
		}

		std::string hash_sit = std::to_string(Synapse::ToInt(Situation));
		std::string hash;
		if(!Base) hash = "~" + Action::GetHashSumm(conq.name, PW_situation); 
		else hash = "~Base"; 

		size_t index = 0; std::string temp, line, line2;

		bool find = false;

		std::string p = Settings::Actions;

		std::ofstream fout; std::ifstream fint;

		if (!Helper::DirExists(p)) r = _mkdir(p.c_str()); // if not exists - make path
		p += "\\"; p += ToPath(synapse[0]); //ToString(conq.action.type); // add additional path
		if (!Helper::DirExists(p)) r = _mkdir(p.c_str()); // if not exists - make path
		p += "\\" + synapse.substr(2) + Settings::exp_action; // set file name

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

				std::vector<std::string> spl = Helper::Split(line2, ";");
				if (spl[0] == hash_sit)
					find = true;

				switch (conq.action.type) {
					case AType::VisualData: {
						//std::vector<std::string> spl = Helper::Split(line, ";");
						//if (spl[0] == conq.action.visual.tag) {
						//	if (spl[1] == std::to_string(conq.action.visual.pos)) {
						//	}
						//}
						//spl.clear(); 
						break; };
					case AType::Move: {

						break; };
					case AType::Speech: {

						break; };
					default: Debug::Log("Action::SaveNeuron : unknown type!"); break;
				}

				spl.clear();

				if (find) break;
				else if(!line2.empty()) temp += line2 + "\n";
			}
			
			if (!find) {
				temp += hash_sit + ";";
				//Debug::Log("Action::Save : event = "+conq.name + "\n\tHash sit = "+std::string(hash_sit) + "\n\tSit = "+Situation, DType::System);
				switch (conq.action.type) {
				case AType::VisualData: {
					Visual v = conq.action.visual;
					temp += v.tag + ";" + std::to_string(v.pos);
					v.~Visual(); break; };
				case AType::Move: {
					Motion m = conq.action.motion;
					temp += m.part + ";" + std::to_string(m.value) + ";0,000000";
					m.~Motion(); break; };
				case AType::Speech: {
					Sound s = conq.action.sound;
					temp += s.text + ";" + std::to_string(s.volime) + ";" + std::to_string(s.tone);
					//temp += std::to_string(s.volime) + ";" + std::to_string(s.tone);
					//Debug::Log("\t" + s.text + ";" + std::to_string(s.volime) + ";" + std::to_string(s.tone), DType::System);
					s.~Sound(); break; };
				default: Debug::Log("Action::SaveNeuron : unknown type!"); break;
				}
				temp += "\n";
			}

			if(!line2.empty() && line2 != "") temp += line2 + "\n";

			while (!fint.eof()) {
				std::getline(fint, line); 
				if (!line.empty()) temp += line + "\n";
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
			fout.close(); fint.close();
			synapse.clear(); hash.clear();
			return;
		}

		temp.clear(); line.clear(); line2.clear();
		synapse.clear(); hash.clear(); Situation.clear();
		p.clear(); p.~basic_string();
	}

	Neuron* Action::LoadNeuron(Consequence& conq, std::string& PW_situation) {
		return LoadNeuron(conq.name, conq.action.type, PW_situation);
	}

	Neuron* Action::LoadNeuron(std::string _Synapse, AType type, std::string& PW_situation) {

		std::ifstream fint; bool exit = false, find = false;
		//std::string temp;
		size_t method = 0, numline = 0	;
		std::string p = Settings::Actions + "\\" + ToString(type) + "\\" + _Synapse + Settings::exp_action;
		std::string line; size_t index;
		//Debug::Log("Action::LoadNeuron : " + _Synapse + "\n\tPath : " + p, DType::System);

		//return NULL; ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		try {
			fint.open(p);
			if (fint.is_open()) {
				Neuron* n = new Neuron();
				std::string hash = Action::GetHashSumm(_Synapse, PW_situation);
				////////////////////////

				while (!fint.eof() && !exit) {
					std::getline(fint, line); numline++;
					if (line[0] == '~') {
						if ((line.substr(1)) == hash) {
							find = true;
							while (!fint.eof()) {
								std::getline(fint, line);
								if (line[0] == '~') {
									exit = true; break;
								}
								else {
									if (line.empty()) break;

									method = 1;

									n->size++;

									n->sensitivities.push_back(Helper::Remove(line, ';', index));
									line = line.substr(index + 1);

									method = 2;

									n->data.push_back(Helper::Remove(line, ';', index));
									line = line.substr(index + 1);

									method = 3;

									n->value_1.push_back(std::stod(Helper::Remove(line, ';', index).c_str()));

									method = 4;

									line = line.substr(index + 1);
									n->value_2.push_back(std::stod(line.c_str()));
								}
							}
						}
					}
					else continue;
				}

				if (!find) {
					//Debug::Log("Action::LoadNeuron : loading base neuron...");

					fint.clear();
					fint.seekg(0);

					std::getline(fint, line); numline++;
					
					method = 6;

					while (!fint.eof()) {
						std::getline(fint, line); numline++;
						//Debug::Log(line);
						if (line[0] == '~' || line.empty()) break;

						n->size++;

						method = 7;

						n->sensitivities.push_back(Helper::Remove(line, ';', index));
						line = line.substr(index + 1);

						method = 8;

						n->data.push_back(Helper::Remove(line, ';', index));
						line = line.substr(index + 1);

						method = 9;

						n->value_1.push_back(std::atof(Helper::Remove(line, ';', index).c_str()));

					
						method = 10;

						line = line.substr(index + 1);
						n->value_2.push_back(std::atof(line.c_str()));
					}
				}

				////////////////////////
				line.clear(); //line.~basic_string();
				hash.clear(); //hash.~basic_string();
				p.clear(); //p.~basic_string();
				fint.close(); _Synapse.clear(); //temp.clear();
				return n;
			}
			else {
				Debug::Log("Action::LoadNeuron : file is not exists!\n\tPath : " + p, DType::Warning);
			}
		}
		catch (const std::exception & ex) {
			Debug::Log("Action::LoadNeuron (1) : An exception has been occured!\n\tPath : " + p + "\n\tInfo : " 
				+ ex.what() + "\n\tMethod : " + std::to_string(method) + "\n\tLine : "+ line + " (" +std::to_string(numline)+")", DType::Error);
		}
		catch (const std::string & ex) {
			Debug::Log("Action::LoadNeuron (2) : An exception has been occured!\n\tPath : " + p + "\n\tInfo : " + ex, DType::Error);
		}
		catch (...) {
			Debug::Log("Action::LoadNeuron (3) : An exception has been occured!\n\tPath : " + p, DType::Error);
		}

		p.clear(); p.~basic_string(); fint.close(); _Synapse.clear();
		return nullptr;
	}

	Neuron::Neuron() {
		this->size = 0;
		//this->hash = std::vector<size_t>();
		this->name = "";
		//this->syn_name = std::vector<std::string>();
		this->value_1 = std::vector<float>();
		this->value_2 = std::vector<float>();
		this->data = std::vector<std::string>();
		this->sensitivities = std::vector<std::string>();
	}

	Neuron::Neuron(float value_1, float value_2) {
		this->size = 1;
		this->value_1.push_back(value_1);
		this->value_2.push_back(value_2);
		this->data.push_back(Settings::EmptyName);
	}

	Neuron::~Neuron() {
		value_1.clear(); value_1.~vector();
		data.clear(); data.~vector();
		value_2.clear(); value_2.~vector();
		name.clear(); name.~basic_string();
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