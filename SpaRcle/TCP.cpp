#include "pch.h"
#pragma comment ( lib, "ws2_32.lib" )
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <conio.h>
#include "TCP.h"
#include "Debug.h"
#include "CentralCore.h"

using namespace std;

namespace SpaRcle {
	const u_short port = 1111;
	const char* ip = "25.11.176.226";
	const int BUFFER_SIZE = 1024 * 4;

	void THREAD_SEND(CentralCore* core) {
		TCP* tcp = core->GetTCP();
		while (Settings::IsActive) {
			if (tcp->count_mess > 0) {
				send(tcp->newConnection, std::string(std::to_string(tcp->count_mess) + "~" + tcp->message).c_str(), 128, 0);
				//Debug::Log(std::string(std::to_string(tcp->count_mess) + "~" + tcp->message));

				tcp->count_mess = 0;
				tcp->message.clear();
			}
			else
				Sleep(100);

			//send(tcp->newConnection, (tcp->message).c_str(), 128, 0);
			//send(tcp->newConnection, mess.c_str(), sizeof(mess.c_str()), 0);
			//if (send(tcp->newConnection, mess.c_str(), sizeof(mess.c_str()), NULL) == SOCKET_ERROR) {
			
		}
	}
	void THREAD_RECV(CentralCore* core) {
		Debug::Log("TCPServer : starting TCP server...", System);
		TCP* tcp = core->GetTCP();

		SOCKADDR_IN addr;
		int sizeofaddr = sizeof(addr);
		addr.sin_addr.s_addr = inet_addr(ip);
		addr.sin_port = htons(port);
		addr.sin_family = AF_INET;

		SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
		bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
		listen(sListen, SOMAXCONN);

		char buffer[BUFFER_SIZE];
		memset(buffer, 0x00, BUFFER_SIZE);
		std::string message = "";
		std::vector<std::string> splits;

		while (Settings::IsActive) {
			tcp->newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
			if (tcp->newConnection == 4294967295) { Sleep(1000); continue; }

			if (tcp->newConnection == 0) { std::cout << "Error #2\n"; }
			else {
				std::cout << "TCPServer : Client Connected! (" << tcp->newConnection << ")\n";
				if (tcp->process_send.joinable()) tcp->process_send.detach();
				tcp->process_send = std::thread(THREAD_SEND, core);

				size_t index = 0; float Bad, Good;

				while (Settings::IsActive) { // is active connection
					//Debug::Log("TCPServer : receiving...");
					int receivedBytes = ::recv(tcp->newConnection, buffer, BUFFER_SIZE, 0);
					message = buffer;
					memset(buffer, 0x00, BUFFER_SIZE);
					if (message == "") break;

					//std::cout << "Received : (" << message << ")" << std::endl;

					if (message[message.size() - 1] == '~') message.resize(message.size() - 1);
				ret:
					splits.clear();   splits = Helper::Split(message, " ", "~", index);
					if (splits.size() > 0) {
						if (splits[0] == "event") {
							if (true) {
								bool self = false;
								if (splits.size() > 4)
									if (splits[4] == "self") { self = true; }

								Bad = -std::stod(splits[3]);
								Good = std::stod(splits[3]);

								if (ToAType(splits[1][0]) == AType::Speech) {
									Consequence conq = Consequence(Sound(splits[2], 10, 15));
									conq.self = self;
									conq.Bad = Bad;
									conq.Good = Good;
									core->_causality->NewEvent(conq, false);
									Sleep(20);
								}
								else if (ToAType(splits[1][0]) == AType::VisualData) {
									Consequence conq = Consequence(Sound(splits[2], 10, 15));
									conq.self = self;
									conq.Bad = Bad;
									conq.Good = Good;
									core->_causality->NewEvent(conq, false);
								}
								else if (ToAType(splits[1][0]) == AType::Move) {
									Consequence conq = Consequence(Motion(splits[2], 0));

									//Debug::Log("hp = "+splits[3]);
									conq.Bad = Bad;
									conq.Good = Good;

									conq.self = self;
									core->_causality->NewEvent(conq, false);
								}
								else Debug::Log("TCPServer : unknown type! (" + message + ")", Error);
							}
							else
							{
								Debug::Log("========================================================");
								for (size_t i = 0; i < Settings::Size_SCP * 2 + 5; i++)
									core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
							}
						}
						else if (splits[0] == "clear") {
							Debug::Log("======================================[CLEAR]======================================");
							for (size_t i = 0; i < Settings::Size_SCP * 2 + 5; i++)
								core->_causality->UncheckedEvents.push_back(Consequence(Settings::EmptyName));
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
		this->core->ConnectTCP(this);

		WSAData wsaData;
		WORD DLLVersion = MAKEWORD(2, 1);
		if (WSAStartup(DLLVersion, &wsaData) != 0) {
			Debug::Log("TCPServer : loading DLL is failure!", Error);
			Settings::IsActive = false; }
	}
	void TCP::Send(Action& act) {
		switch (act.type) {
		case AType::Move: {
			Motion* m = new Motion(act);
			message += ("action M " + m->part + " " + std::to_string(m->value) + "~");
			delete m;
			this->count_mess++; ///\^IIIIIIIIIIIIII
			break; }
		case AType::Speech: {
			Sound* s = new Sound(act);
			message += ("action S " + s->text + "~");
			delete s;
			this->count_mess++; ///\^IIIIIIIIIIIIII
			break; }
		default:
			Debug::Log("TCP::Send : unknown type!", Error);
			break;
		}
	}
	void TCP::Start() { process_recv = std::thread(THREAD_RECV, this->core); process_send = std::thread(); }
	void TCP::Close() { 
		if (this->process_recv.joinable()) { Debug::Log("TCPServer : stopping TCP server", System); this->process_recv.detach(); }
		if (this->process_send.joinable()) { Debug::Log("TCPClient : stopping TCP client", System); this->process_send.detach(); }
	}

	TCP::~TCP() { this->Close(); }
}
