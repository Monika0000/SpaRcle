#pragma once
#include "Helper.h"
#include "Consequence.h"

namespace SpaRcle {
	class CentralCore;

	class Memory
	{
	protected:
		std::string st_mem_path;
		bool isInit;
		bool isCreate;
		CentralCore* core;
	public:
		Memory();
		~Memory();

		void LoadStaticMemory();
		void InitMemory(CentralCore* core);
		static Memory* GetMemory();
	};
}


