#pragma once
#include "Core.h"
#include <vector>
//#include <boost/tuple/tuple.hpp>
#include "Consequence.h"
#include "TaskTree.h"

namespace SpaRcle {
	class CentralCore;

	class Helper;

	class LogicalCore : public Core	{
	private:

		std::vector<std::string> mono_nam;
		std::vector<std::string> mono_sit;

		size_t temp_hash; float temp_percent;

		void LogicalSolution();

		//!LoverPrioritySynapse

		size_t lower_size; 
		std::vector<std::string> lower_names;
		std::vector<size_t> lower_id_bad;
		std::vector<size_t> lower_id_good;
	public:
		void DoLowerPriopity(Consequence & conq);

		LogicalCore(int cpuSpeed);
		~LogicalCore();

		void LoverPrioritySynapse(std::string event_name, size_t bad_syn_index, size_t good_syn_index);

		static std::vector<std::string> DecomposeConsequence(Consequence& conseq);

		short CoreLoad;

		CentralCore* core;
		int maxTasks = 8;
		std::vector<Task> Tasks; // size 8

		void Start();
		void DoIt(Task task);

		/* Sensivitys передавать не по ссылке!!!! */
		void EditCauses(std::vector<std::string>& Causes, std::vector<int>& Meets, std::vector<std::string> Sensivitys, Consequence& conq);
	
		/*
			1 - Новые причины следствия
			2 - Количество встреч причины под тем же индексом
			3 - Чувствительность нейрона (краткие данные о его причинах)
			3 - Общая полезность следствия, которому принадлежат причины
			5 - Имя следствия вместе с типом, пример : T/Name	
		*/
		std::vector<std::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, std::string>> Causes; // Private (Don't use)
		size_t size_causes;

		/*
			Изменить репутацию(полезность) некоторого следствия, в соответствии с другими следствиями.
		*/
		bool CauseReputation(std::tuple<std::vector<std::string>, std::vector<int>, std::vector<std::string>, double, std::string>& Cause, const bool Diagnostic = false);

		/*
			Получить логическую противоположность заданному следствию
		*/
		static bool GetOpposite(Consequence& opposite, Consequence& event, bool Diagnostic = false);

		/* 
			Данная функция имеет собственный буфер, который изменяется при каждом вызове.
		*/
		float IsMonotone(std::string& name, std::string& situation);
	};
}

