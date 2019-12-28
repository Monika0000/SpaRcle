#include "pch.h"
#include "TCP.h"
#include "Debug.h"
#pragma comment ( lib, "ws2_32.lib" )
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <iostream>
#include <conio.h>
#include "Settings.h"
#include "CentralCore.h"

using namespace std;

namespace SpaRcle {
	

	SOCKET Connections[100];
	const int BUFFER_SIZE = 1024 * 4;
	int Counter = 0;

	void THREAD(CentralCore* core) {
		Debug::Log("TCPServer : starting TCP server...", System);

		SOCKADDR_IN addr;
		int sizeofaddr = sizeof(addr);
		addr.sin_addr.s_addr = inet_addr("25.11.176.226");
		addr.sin_port = htons(1111);
		addr.sin_family = AF_INET;

		SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
		bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
		listen(sListen, SOMAXCONN);

		//std::byte Buffer[BUFFER_SIZE];

		char buffer[BUFFER_SIZE];
		memset(buffer, 0x00, BUFFER_SIZE);
		std::string message = "";
		std::vector<std::string> splits;
		SOCKET newConnection;

		while (true) {
			newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
			if (newConnection == 4294967295) { Sleep(1000); continue; }

			if (newConnection == 0) { std::cout << "Error #2\n"; }
			else {
				std::cout << "TCPServer : Client Connected! (" << newConnection << ")\n";
				//std::string message = "Hello NIKITA!";
				//message.c_str();
				//char msg[256];

				size_t index = 0;

				while (true) { // is active connection
					//Debug::Log("TCPServer : receiving...");
					int receivedBytes = ::recv(newConnection, buffer, BUFFER_SIZE, 0);
					message = buffer;
					memset(buffer, 0x00, BUFFER_SIZE);
					if (message == "") break;

					//std::cout << "Received : (" << message << ")" << std::endl;

					if (message[message.size() - 1] == '~') message.resize(message.size() - 1);
				ret:
					splits.clear();   splits = Helper::Split(message, " ", "~", index);
					if (splits.size() > 0) {
						if (splits[0] == "event") {
							if (true)
								if (ToAType(splits[1][0]) == AType::Speech) {
									Consequence conq = Consequence(Sound(splits[2], 10, 15));
									core->_causality->NewEvent(conq, false);
								}
								else if (ToAType(splits[1][0]) == AType::VisualData) {
									Consequence conq = Consequence(Sound(splits[2], 10, 15));
									core->_causality->NewEvent(conq, false);
								}
								else if (ToAType(splits[1][0]) == AType::Move) {
									Consequence conq = Consequence(Sound(splits[2], 10, 15));
									core->_causality->NewEvent(conq, false);
								}
								else Debug::Log("TCPServer : unknown type! (" + message + ")", Error);
							else
							{
								Debug::Log("========================================================");
								//core->_causality->NewEvent(Consequence(Sound("hello", 10, 15)), false);
								//core->_causality->NewEvent(Consequence(Sound("monika", 10, 15)), false);
								//core->_causality->NewEvent(Consequence(Sound("desu", 10, 15)), false);
								///core->_causality->NewEvent(Consequence(Sound("baka", 10, 15)), false);
								for (size_t i = 0; i < Settings::Size_SCP * 2 + 5; i++)
									core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
							}
						}
						else if (splits[0] == "clear") {
							for (size_t i = 0; i < Settings::Size_SCP * 2 + 5; i++)
								core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
							Debug::Log("======================================[CLEAR]======================================");
						}
						else
							Debug::Log("TCPServer : unknown command \""+ splits[0]+ "\"");
					}

					if (index != size_t_max) {
						message = message.substr(index);
						index = 0; goto ret;
					}
					else index = 0;
				}
				Debug::Log("TCPServer : client has been disconncted!", Info);
				//send(newConnection, message.c_str(), sizeof(message.c_str()), NULL);
				//Connections[i] = newConnection;
				//Counter++;
				//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
			}
			Sleep(500);
		}
	}

	TCP::TCP(CentralCore *core) {
		Debug::Log("TCPServer : creating TCP server...", System);
		this->core = core;

		//SOCKET Connections[100];
		WSAData wsaData;
		WORD DLLVersion = MAKEWORD(2, 1);
		if (WSAStartup(DLLVersion, &wsaData) != 0) {
			Debug::Log("TCPServer : loading DLL is failure!", Error);
			Settings::IsActive = false; }
	}
	void TCP::Start() { process = std::thread(THREAD, this->core); }
	void TCP::Close() { if (this->process.joinable()) { Debug::Log("TCPServer : stopping TCP server", System); this->process.detach(); } }

	TCP::~TCP() { this->Close(); }
}
