#include "glog.h"

#include <string>
using namespace boost::log::trivial;
namespace GlobalLog
{
	boost::log::sources::severity_logger< boost::log::trivial::severity_level > gLog_;

	void initLogPath(const std::string &sPath, int nLevel){
		namespace logging = boost::log;
		namespace src = boost::log::sources;
		namespace sinks = boost::log::sinks;
		namespace keywords = boost::log::keywords;

		logging::add_common_attributes();
		logging::add_file_log
			(
			keywords::open_mode = (std::ios::out | std::ios::app),
			keywords::auto_flush = true,
			keywords::file_name = sPath + "-%Y%m%d_%5N.log",
			keywords::rotation_size = 10 * 1024 * 1024,
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0),
			keywords::format = "[%TimeStamp%]: %Message%"
			);

		logging::core::get()->set_filter
			(
			logging::trivial::severity >= nLevel
			);

	}
};
