#include "pch.h"
#include "Motion.h"

namespace SpaRcle {
	SpaRcle::Motion::Motion() {
		this->part = "";
		this->value = 0; }

	Motion::Motion(std::string part, float value) {
		this->part = part;
		this->value = value;
	}

	SpaRcle::Motion::~Motion() {
		this->part.clear();
	}
}