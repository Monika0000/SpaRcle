#pragma once
#include "Helper.h"
#include "Consequence.h"
#include <map>

namespace SpaRcle {
	class CentralCore;

	class Memory {
	private:
		std::map<std::string, Consequence*> conq_speech;
		std::map<std::string, Consequence*> conq_visual;
		std::map<std::string, Consequence*>::iterator iterator;
	protected:
		std::string st_mem_path;
		bool isInit;
		bool isCreate;
		CentralCore* core;
	public:
		std::vector<std::string> GetAllFiles(std::string path);
		Memory();
		~Memory();
		//int CopyFile(char const* source, char const* dest, bool overwrite);

		Consequence* GetFragment(std::string name, AType type); 
		void AddFragment(Consequence * con); 
		void LoadStaticMemory();
		void InitMemory(CentralCore* core);
		static Memory* GetMemory();
	};
}


