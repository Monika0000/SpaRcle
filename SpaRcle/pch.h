// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
#define NOMINMAX // Нужно для того, чтобы SFML не выдавала ошибок

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#ifndef PCH_H
#define PCH_H

// TODO: add headers that you want to pre-compile here
#include <string>

#define SWITCH(str)  switch(SpaRcle::str_hash_for_switch(str))
#define CASE(str)    static_assert(SpaRcle::str_is_correct(str) && (SpaRcle::str_len(str) <= SpaRcle::MAX_LEN),\
"CASE string contains wrong characters, or its length is greater than 9");\
case SpaRcle::str_hash(str, SpaRcle::str_len(str))
#define DEFAULT  default

namespace SpaRcle
{
	typedef unsigned char uchar;
	typedef unsigned long long ullong;

	const uchar MAX_LEN = 9;
	const ullong N_HASH = static_cast<ullong>(-1);

	constexpr ullong raise_128_to(const uchar power)
	{
		return 1ULL << 7 * power;
	}

	constexpr bool str_is_correct(const char* const str)
	{
		return (static_cast<signed char>(*str) > 0) ? str_is_correct(str + 1) : (*str ? false : true);
	}

	constexpr uchar str_len(const char* const str)
	{
		return *str ? (1 + str_len(str + 1)) : 0;
	}

	constexpr ullong str_hash(const char* const str, const uchar current_len)
	{
		return *str ? (raise_128_to(current_len - 1) * static_cast<uchar>(*str) + str_hash(str + 1, current_len - 1)) : 0;
	}

	inline ullong str_hash_for_switch(const char* const str)
	{
		return (str_is_correct(str) && (str_len(str) <= MAX_LEN)) ? str_hash(str, str_len(str)) : N_HASH;
	}

	inline ullong str_hash_for_switch(const std::string& str)
	{
		return (str_is_correct(str.c_str()) && (str.length() <= MAX_LEN)) ? str_hash(str.c_str(), (uchar)str.length()) : N_HASH;
	}
}
#endif //PCH_H

//#ifndef STR_SWITCH_H
//#define STR_SWITCH_H


//#endif  // STR_SWITCH_H