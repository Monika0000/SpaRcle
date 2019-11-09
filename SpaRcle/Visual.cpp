#include "pch.h"
#include "Visual.h"

namespace SpaRcle {
	Vector3::Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z; }
	Vector3::Vector3() {
		Vector3(0, 0, 0); }

	Visual::Visual(std::string tag) {
		this->tag = tag;
		this->pos = Vector3();
	}
	Visual::Visual() {
		Visual(""); }
	Visual::~Visual()
	{
		this->tag.~basic_string();
	}
}