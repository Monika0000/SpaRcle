#pragma once
#include <thread>

namespace SpaRcle {
	class Core
	{
	protected:
		std::thread Process;
	public:
		int DelayCPU = 1500;
	};
}

