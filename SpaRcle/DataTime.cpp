#include "pch.h"
#include "DataTime.h"

namespace SpaRcle {
	DataTime::DataTime()
	{
		time_t tim = time(NULL);
		tm *t = localtime(&tim);

		year = t->tm_year + 1900;
		day = t->tm_yday;
		hour = t->tm_hour;
		min = t->tm_min;
		sec = t->tm_sec;
	}
	DataTime::~DataTime() {  }
}
