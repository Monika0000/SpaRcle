#pragma once
#include <iostream>
#include <thread>
#include "Settings.h"

namespace SpaRcle {
	class CentralCore;
	class Action;

	class TCP {
	private:
	public:
		int count_mess = 0;
		std::string message;

		std::thread process_recv;
		std::thread process_send;

		SOCKET newConnection;

		CentralCore* core = NULL;

		TCP(CentralCore *core);
		~TCP();

		void Send(Action& act);
		void Start();
		void Close();
	};
}

