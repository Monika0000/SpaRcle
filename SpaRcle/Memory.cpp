#include "pch.h"
#include <windows.h>
#include <iostream>
#include <wchar.h>
#include "Memory.h"
#include <filesystem>
#include <istream>
#include <fstream>
#include "CentralCore.h"

namespace fs = std::filesystem;

namespace SpaRcle {
	Memory::Memory() {
		this->isCreate = false;
		this->core = NULL;
		this->isInit = false; }
	Memory::~Memory() { Debug::Log("Memory clearing...", System); }
	Consequence* Memory::GetFragment(std::string name, AType type) {
		switch (type)
		{
		case SpaRcle::Undefined:
			return NULL;
		case SpaRcle::Speech:
			iterator = this->conq_speech.find(name);
			if (iterator != this->conq_speech.end())
				return iterator->second;
			else
				return NULL;
		case SpaRcle::Move:
			return NULL;
		case SpaRcle::VisualData:
			return NULL;
		default:
			return NULL;
		}
	}
	void Memory::LoadStaticMemory() {
		if (this->isInit) {
			Debug::Log("Loading static memory...", System);
			for (auto& p : fs::directory_iterator(this->st_mem_path)) {
				std::wstring ws(p.path().c_str());
				std::string path_name(ws.begin(), ws.end());
				char mode = path_name.substr(path_name.size() - 3)[0];
				short lineNum = 0;

				try {
					std::fstream file; // ANSI
					file.open(path_name);

					std::vector<std::string> vec = std::vector<std::string>();
					std::string line, newLine;
				ret:
					std::getline(file, line); lineNum++;
					if (!line.empty()) {
						for (unsigned short i = 0; i < line.size(); i++) {
							if (line[i] == ' ') { vec.push_back(newLine); newLine.clear(); }
							else
								newLine += Helper::TransliterationEN(line[i], false);
							if (i + 1 == line.size()) { vec.push_back(newLine); newLine.clear(); }
						}

						try { switch (mode) {
							case'm': {
								for (auto& a : vec)
									Debug::Log(a);
								Debug::Log("================");
								break; }
							case's': {
								double hp = std::stod(vec[vec.size() - 1]);
								for (unsigned char c = 0; c < vec.size() - 2; c++) 
									this->core->_causality->UncheckedEvents.push_back(Consequence(Sound(vec[c]), false, hp));
								

								for (unsigned char c = 0; c < 10; c++)
									this->core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
								break; }
							default: Debug::Log("Memory::LoadStaticMemory : Unknown mode!\n\tPath : " + path_name + "\n\tContent : " + line
								+ "\n\tLine : " + std::to_string(lineNum), Warning); break;
							}
						} catch (...) { Debug::Log("Memory::LoadStaticMemory : An exception has occured!\n\tPath : " + path_name + "\n\tContent : " + line
							+ "\n\tLine : " + std::to_string(lineNum), Error); }

						line.clear(); vec.clear(); goto ret;
					}

					line.~basic_string();
					newLine.~basic_string();
					vec.~vector();
					file.close();
				}
				catch (...) { Debug::Log("Memory::LoadStaticMemory : failed open file!\n\t" + path_name); }
				//Debug::Log(, Log);

				ws.~basic_string();
				path_name.~basic_string();
			}
		}
	}
	void SpaRcle::Memory::InitMemory(CentralCore* core) {
		if (!isInit) {
			this->st_mem_path = Settings::Memory + "\\Static";
			this->core = core;
			this->isInit = true;
			Debug::Log("Memory is initialized!", System);
		}
		else
			Debug::Log("Memory already initialized!", System);
	}
	Memory* Memory::GetMemory() {
		static Memory* mem = new Memory();
		if ((*mem).isCreate) {
			if (!(*mem).isInit) {
				std::cout << "==================================[MEMORY ERROR]==================================" << std::endl;
				Debug::Log("[FATAL] Memory is not initialized!", Error);
				return NULL;
				Settings::IsActive = false; }
		}
		else (*mem).isCreate = true;

		return mem;
	}
}
 