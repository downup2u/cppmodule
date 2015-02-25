#ifndef GLOG_HPP_INCLUDE
#define GLOG_HPP_INCLUDE

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <string>
using namespace boost::log::trivial;
namespace GlobalLog
{
extern boost::log::sources::severity_logger< boost::log::trivial::severity_level > gLog_;
extern void initLogPath(const std::string &sPath,int nLevel);
};

using namespace GlobalLog;
using namespace boost::log::trivial;

#ifdef _DEBUG 
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#else
#define AT ""
#endif

#endif