#pragma once
#include <iostream>
#include <string>

namespace SpaRcle {
	struct Vector3
	{
	public:
		float x; // x pos
		float y; // y pos
		float z; // size
		Vector3(float x, float y, float z);
		Vector3();
	};

	class Visual
	{
	public:
		std::string tag;
		Vector3 pos;
		Visual(std::string tag);
		Visual();
		~Visual();
	};
}
