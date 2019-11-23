#include "pch.h"
#include <windows.h>
#include <iostream>
#include <wchar.h>
#include "Memory.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace SpaRcle {
	Memory::Memory() {
		this->isCreate = false;
		this->core = NULL;
		this->isInit = false; }
	Memory::~Memory() { }
	void Memory::LoadStaticMemory(){
		if (this->isInit) {
			Debug::Log("Loading static memory...", System);
			for (auto& p : fs::directory_iterator(this->st_mem_path)) {
				const wchar_t* txt = p.path().c_str();
				std::wstring ws(txt);
				Debug::Log(std::string(ws.begin(), ws.end()), Log);

				ws.~basic_string();
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
 