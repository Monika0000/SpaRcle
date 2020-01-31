#include "pch.h"
#include "Sound.h"
#include "Debug.h"

namespace SpaRcle {
	Sound::Sound(std::string text, double tone, double volime) {
		this->text = text;
		this->tone = tone;
		this->volime = volime; 
		//Debug::Log()
	}

	Sound::Sound(std::string text) {
		this->text = text;
		this->tone = 0;
		this->volime = 0; }

	Sound::Sound() {
		this->text = "";
		this->tone = 0;
		this->volime = 0; }

	Sound::~Sound(){  } //this->text.~basic_string();
}
