#pragma once
#include <iostream>
#include <string>

namespace SpaRcle {
	struct Motion  {
	public:
		std::string part;
		float value;

		Motion();
		Motion(std::string part, float value);
		~Motion();
	};
}

