#include "utiltime.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>  

namespace UtilTime
{
	std::string getCurrentDate()
	{
		std::string strDate;
		std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());  	// 这时候strTime里存放时间的格式是YYYYMMDDTHHMMSS，日期和时间用大写字母T隔开了  
		//20140121T170529
		strTime.replace(4, 0, std::string("-"));
		strTime.replace(7, 0, std::string("-"));
		size_t pos = strTime.find('T');
		if (pos != std::string::npos){
			strDate = strTime.substr(0, pos);
		}
		return strDate;
	}

	std::string getCurrentTime()
	{
		std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());  	// 这时候strTime里存放时间的格式是YYYYMMDDTHHMMSS，日期和时间用大写字母T隔开了  
		//20140121T170529
		strTime.replace(4, 0, std::string("-"));
		strTime.replace(7, 0, std::string("-"));
		size_t pos = strTime.find('T');
		if (pos != std::string::npos){
			strTime.replace(pos, 1, std::string(" "));
			strTime.replace(pos + 3, 0, std::string(":"));
			strTime.replace(pos + 6, 0, std::string(":"));
		}
		return strTime;
	}

	std::string convertToTimeString(const tm&t1)
	{
		boost::format fmt1("%04d-%02d-%02d %02d:%02d:%02d");
		fmt1 % (t1.tm_year + 1900) % (t1.tm_mon+1)%t1.tm_mday%t1.tm_hour%t1.tm_min%t1.tm_sec;
		return fmt1.str();
	}
	bool convertStringToTime(const std::string&ts, tm&t1)
	{
		try
		{
			boost::posix_time::ptime t(boost::posix_time::time_from_string(ts));
			t1 = boost::posix_time::to_tm(t);
		}
		catch (...)
		{
			return false;
		}
		return true;
	}
};

