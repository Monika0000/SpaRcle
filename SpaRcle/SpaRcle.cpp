// SpaRcle.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Visual Studio\\SFML-2.5.1\\lib\\sfml-graphics.lib")
#include "pch.h"
#include <iostream>
#include "Display.h"
#include "CentralCore.h"
#include "CausalityCore.h"
#include "LogicalCore.h"
#include "RealityCore.h"
#include "EmotionCore.h"
#include "Debug.h"
#include "Helper.h"
#include "Settings.h"
#include <string>
#include <thread>
#include <stdexcept>
using namespace SpaRcle;
//using namespace boost::tuples;

#pragma region Propetries;
CentralCore* _core;
CausalityCore* _causality;
LogicalCore* _logic;
RealityCore* _reality;
EmotionCore* _emotion;
void Destruct() {
	//_emotion->~EmotionCore();
	//_reality->~RealityCore();
	//_logic->~LogicalCore();
	//_causality->~CausalityCore();
	//_core->~CentralCore();
	delete _emotion;
	delete _reality;
	delete _logic;
	delete _causality;
	delete _core;
}
#pragma endregion
 
/*
class B;
class C;

class A {
public:
	A();
	int type;
	virtual B* Get(const B& b);
	virtual C* Get(const C& c);
};

class B : public A
{
public:
	B();
	B* Get(const B& b) override;
};
class C : public A
{
public:
	C();
	C* Get(const C& c) override;
};


A::A() {
	std::cout << "A construct" << std::endl;
}
B* A::Get(const B& b) { return new B(); }
C* A::Get(const C& c) { return new C(); }


B::B() : A() {
	std::cout << "B construct" << std::endl;}	
B* B::Get(const B& b) {
	std::cout << "B Get" << std::endl;
	return this; }

C::C() : A() {
	std::cout << "C construct" << std::endl;}
C* C::Get(const C& c) {
	std::cout << "C Get" << std::endl;
	return this; }
*/

/*
class B {

};
class A {
public:
	//A(const B& b) {
	//}
	operator B() {
		return B();
	}
};
*/

/*
class Object {
public :
	template <typename T> Object(T value) {

	}
	 
	template <typename T> T Get() {

	}

private :
	template <typename T> class Data
	{
	public:

	};

	Data data;
};
*/

/*
void Demo(int(*_printf) (const char *, ...))
{
	_printf("Hello, OSIX!n");
	return;
}
FILE *f;
char buff[1000];
int(*_printf) (const char *, ...);
int(*_main) (int, char **);
void(*_Demo) (int(*) (const char *, ...));

_Demo = Demo;
_main = main;
_printf = printf;
int func_len = (unsigned int)_main + (unsigned int)_Demo;
func_len = func_len * 10;

f = fopen("Demo32.bin", "wb");
for (int a = 0; a < func_len; a++)
	fputc(((int)buff[a]) ^ 0x77, f);
fclose(f);
return 0;
*/

// Task : Сделай кнопки,чтобы быстро кликать на события (идет, бежит, прыгает, летит,больно)

int main(int argc, char **argv)
{
	//std::cout << ((1.22322e-4)* 100) / 100 << std::endl;
	//std::cout << ((1.3676e-294)* 10) / 100 << std::endl;
	//std::cout << Round(1.3676e-294) << std::endl;
	system("color 70");
	setlocale(LC_ALL, "rus");
	ShowWindow(GetConsoleWindow(), SW_NORMAL);
	//return Settings::count_sens - 1;
	//std::string s = "12345";
	//std::cout << s.substr(0, 1) << std::endl;return 1;
	//std::cout << Helper::GetSensivityOfName("1234567") << std::endl;
	//std::cout << Helper::GetSensivityOfName("1") << std::endl;
	//std::cout << Helper::SimilarityPercentage("012000", "013000") << std::endl;




	#pragma region [======== Pre-Init ========]
	Debug::StartDebugger();
	Sleep(100);
	Debug::Log("Pre-initializing", DType::System);

	Settings::CoreDebug = false; // Disable loging cores
	//////////////////////////////
	TCHAR cwd[100];
	GetCurrentDirectory(100, cwd);
	std::wstring ws(&cwd[0]);
	Settings::SetPaths(std::string(ws.begin(), ws.end()),
		"D:\\SpaRcle\\SpaRcleData\\System",
		"D:\\SpaRcle\\SpaRcle\\Resources",
		"D:\\SpaRcle\\SpaRcleData\\Memory",
		"D:\\SpaRcle\\SpaRcleData\\Logic");
	Debug::Log("~ DIR \"" + Settings::PathPj + "\"");


	//std::string all; Consequence conq("four", AType::Speech);
	//for (auto& a : LogicalCore::DecomposeConsequence(conq)) all += "\n\t" + a;
	//Debug::Log("Decompose \"4\" : " + all); Sleep(1111111);

	if (!Settings::Diagnostic())
		Debug::Log("Diagnostic... System is unstable!", true, DType::System, SpaRcle::Red);
	else
	{
		boost::tuple<int> r;
		Debug::Log("Diagnostic... System is stable!", DType::System, SpaRcle::Green);

		//std::cout << Helper::SimilarityPercentage("12", "123") << std::endl;
		//Sleep(225552);

		Settings::Status = 1;
		//////////////////////////////
		_core = new CentralCore(5);
		_causality = new CausalityCore(5);
		_logic = new LogicalCore(5);
		_reality = new RealityCore(100);
		_emotion = new EmotionCore(1500);
		#pragma endregion

		#pragma region [======== Init ========]
		Debug::Log("Initializing", DType::System);
		Display::StartDisplay(_core); // Graph-mode

		_core->ConnectLogic(_logic);
		_core->ConnectCausality(_causality);
		_core->ConnectReality(_reality);
		_core->ConnectEmotion(_emotion);

		Sleep(100);
		_reality->Start();

		Sleep(100);
		_logic->Start();

		Sleep(100);
		_causality->Start();
		
		Sleep(100);
		_emotion->Start();
		Sleep(100);

		_core->Start();
		Sleep(100);
		#pragma endregion

		while (true)
		{
			Sleep(100);
			Settings::Layout = Helper::GetLayout();
			if (Settings::Status < 0) {
				break;
			}

			if (Display::Status == 2)
				break;
			else if (Display::Status == 0)
			{
				//std::cout << "Для продолжения нажмите Enter\n";
				Debug::Log("Starting are successful. For exit press the Enter...", DType::System);
				std::cin.get();
				break;
			}
		}

		Debug::StopDebugger();
		std::cout << "================================== STOP ==================================" << std::endl;
		Sleep(20000);

		Destruct();

		Display::Deactive();
		std::cout << std::endl;
		

		return 0;
	}

	Sleep(200);

	Debug::StopDebugger();
	return -1;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
