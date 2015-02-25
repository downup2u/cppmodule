#ifndef _UTIL_TIME
#define _UTIL_TIME
#include <string>

namespace UtilTime
{
	//获取当前时间
	std::string getCurrentTime();
	std::string getCurrentDate();
	std::string convertToTimeString(const tm&t1);
	bool convertStringToTime(const std::string&ts, tm&t1);
};

#endif