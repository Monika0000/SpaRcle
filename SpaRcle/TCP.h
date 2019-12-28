#pragma once
#include <iostream>
#include <thread>

namespace SpaRcle {
	class CentralCore;

	class TCP
	{
	public:
		int Counter = 0;

		std::thread process;

		CentralCore* core = NULL;

		TCP(CentralCore *core);
		~TCP();

		void Start();
		void Close();
		//void ClientHandler(int index, SOCKET* Connections);
	};
}

