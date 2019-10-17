#include "pch.h"
#include "TaskTree.h"

namespace SpaRcle {
	Task::Task() { }

	Task::Task(Consequence result) {
		this->result = result;
	}

	Task::~Task() {

	}

	Branch::Branch()
	{

	}

	Branch::~Branch() {

	}

	TaskTree::TaskTree() {

	}

	TaskTree::~TaskTree() {

	}
}