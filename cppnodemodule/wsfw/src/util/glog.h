#ifndef GLOG_HPP_INCLUDE
#define GLOG_HPP_INCLUDE

#include <logging.h>  

#include <string>

namespace GlobalLog
{
	extern void initLogPath(const std::string &sPath, int nLevel);
};

using namespace GlobalLog;


#endif