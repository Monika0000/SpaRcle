#pragma once
#include <ctime>

namespace SpaRcle {
	class DataTime
	{
	public:
		DataTime();
		~DataTime();

		int year;
		int day;
		int hour;
		int min;
		int sec;

		bool operator>(DataTime right) {
			if (this->year > right.year) return true;
			else if (this->year < right.year) return false;

			if (this->day > right.day) return true;
			else if (this->day < right.day) return false;

			if (this->hour > right.hour) return true;
			else if (this->hour < right.hour) return false;

			if (this->min > right.min) return true;
			else if (this->min < right.min) return false;

			if (this->sec > right.sec) return true;
			else if (this->sec < right.sec) return false;

			return false;
		}

		bool operator<(DataTime right) {
			if (this->year < right.year) return true;
			else if (this->year > right.year) return false;

			if (this->day < right.day) return true;
			else if (this->day > right.day) return false;

			if (this->hour < right.hour) return true;
			else if (this->hour > right.hour) return false;

			if (this->min < right.min) return true;
			else if (this->min > right.min) return false;

			if (this->sec < right.sec) return true;
			else if (this->sec > right.sec) return false;

			return false;
		}

		bool operator==(DataTime right) {
			if (this->year == right.year)
				if (this->day == right.day)
					if (this->hour == right.hour)
						if (this->min == right.min)
							if (this->sec == right.sec)
								return true;
			return false;
		}
	};
}