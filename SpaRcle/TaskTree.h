#pragma once
#include <iostream>
#include <vector>
#include "Consequence.h"

namespace SpaRcle {
	class Task
	{
	public:
		Task();
		Task(Consequence result);
		~Task();

		Consequence result;
	};

	class Branch
	{
	public:
		Branch();
		~Branch();

		std::vector<Task> Tasks;
	};

	class TaskTree
	{
	public:
		TaskTree();
		~TaskTree();

		std::vector<Branch> Branches;
	};
}

