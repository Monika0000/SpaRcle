#pragma once
#include <string>

namespace SpaRcle {
	struct Sound
	{

	public:
		Sound();
		Sound(std::string text);
		Sound(std::string text, double tone, double volime);
		~Sound();

		std::string text;

		double tone;
		double volime;
	private:
	};
}

