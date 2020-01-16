#include "pch.h"
#include <windows.h>
#include <iostream>
#include <wchar.h>
#include "Memory.h"
#include <filesystem>
#include <istream>
#include <fstream>
#include "CentralCore.h"
#include <algorithm> // for std::copy

namespace fs = std::filesystem;

namespace SpaRcle {
	std::vector<std::string> Memory::GetAllFiles(std::string path) {
		std::vector<std::string> files = std::vector<std::string>();

		for (auto& p : fs::directory_iterator(path)) {
			std::wstring ws(p.path().c_str());
			files.push_back(std::string(ws.begin(), ws.end())); ws.~basic_string(); }

		return files;
	}
	Memory::Memory() {
		this->isCreate = false;
		this->core = NULL;
		this->isInit = false; 
	
		for (auto& a : this->GetAllFiles(Settings::SysDir + "\\Commands")) {
			std::remove(a.c_str());
			a.~basic_string();
		}
	}
	Memory::~Memory() { 
		Debug::Log("Memory clearing...", System); 
		if (Settings::isUseMemory) {
			Settings::isUseMemory = false;
			for (std::map<std::string, Consequence*>::iterator it = this->conq_speech.begin(); it != this->conq_speech.end(); ++it) {
				it->second->Save("Memory");
				delete it->second;
			}
		}
	}
	//int Memory::CopyFile(char const* source, char const* dest, bool overwrite) { return 0; }
	Consequence* Memory::GetFragment(std::string name, AType type) {
		switch (type) {
		case SpaRcle::AType::Undefined: return NULL;
		case SpaRcle::AType::Speech:
			iterator = this->conq_speech.find(name);
			if (iterator != this->conq_speech.end())
				return iterator->second;
			else return NULL;
		case SpaRcle::AType::Move: return NULL;
		case SpaRcle::AType::VisualData: return NULL;
		default: return NULL; }
	}
	void Memory::AddFragment(Consequence* con) {
		switch (con->action.type) {
		case AType::Speech: {
			this->conq_speech.insert(std::pair<std::string, Consequence*>(con->name, con));
		}
		default:
			Debug::Log("Memory::AddFragment : unknown type! (" + std::string(ToString(con->action.type)) + ")", Error);
			break;
		}
	}
	void Memory::LoadStaticMemory() {
		if (this->isInit) {
			Debug::Log("Loading static memory...", System);
			short lineNum = 0;
			bool isBreak = false;
			for (auto& path_name : this->GetAllFiles(this->st_mem_path)) {
				char mode = ' '; for (unsigned short tm = (unsigned short)path_name.size(); tm > 0; tm--)
					if (path_name[tm] == '.') { mode = path_name[tm + 1]; break; }
				lineNum = 0;

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
							else newLine += Helper::TransliterationEN(line[i], false);
							if (i + 1 == line.size()) { vec.push_back(newLine); newLine.clear(); }
						}

						try {
							switch (mode) {
							case'w': { //Debug::Log(Helper::TransliterationRU(vec[t]));
								for (unsigned short t = 0; t < vec.size(); t++) {  this->core->_causality->UncheckedEvents.push_back(Consequence(Sound(vec[t]), false, 0)); }
								for (unsigned short t = 0; t < vec.size(); t++) {  this->core->_causality->UncheckedEvents.push_back(Consequence(Sound(vec[t]), false, 0)); }
								// Double
								for (unsigned char c = 0; c < 10; c++) this->core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
								break; }
							case'm': {
								for (unsigned short t = 0; t < vec.size(); t++) { this->core->_causality->UncheckedEvents.push_back(Consequence(Sound(vec[t]), false, 0)); }
								for (unsigned char c = 0; c < 10; c++) this->core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
								break; }
							case's': {
								for (unsigned short t = 0; t < vec.size(); t++) { this->core->_causality->UncheckedEvents.push_back(Consequence(Sound(vec[t]), false, 0)); }
								break; }
							case'h': {
								float hp = std::stof(vec[vec.size() - 1]);
								for (unsigned char c = 0; c < vec.size() - 2; c++) this->core->_causality->UncheckedEvents.push_back(Consequence(Sound(vec[c]), false, hp));
								for (unsigned char c = 0; c < 10; c++) this->core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
								break; }
							default: Debug::Log("Memory::LoadStaticMemory : Unknown mode! (" + std::string(1, mode) + ")\n\tPath : " + path_name + "\n\tContent : " + line
								+ "\n\tLine : " + std::to_string(lineNum), Warning); isBreak = true; break; }
						} catch (...) { Debug::Log("Memory::LoadStaticMemory : An exception has occured!\n\tPath : " + path_name + "\n\tContent : " + line
							+ "\n\tLine : " + std::to_string(lineNum), Error); }

						line.clear(); vec.clear(); if(!isBreak) goto ret;
					}

					line.~basic_string(); newLine.~basic_string();
					vec.~vector(); file.close();
				}
				catch (...) { Debug::Log("Memory::LoadStaticMemory : failed open file!\n\t" + path_name); }
				//Debug::Log(, Log);

				path_name.~basic_string();
			}
		}
	}
	void SpaRcle::Memory::InitMemory(CentralCore* core) {
		if (!isInit) {
			this->st_mem_path = Settings::SysDir + "\\StaticMemory";
			this->core = core;
			this->isInit = true;
			Debug::Log("Memory is initialized!", System); }
		else Debug::Log("Memory already initialized!", System);
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
 