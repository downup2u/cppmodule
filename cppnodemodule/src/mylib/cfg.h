#ifndef _CFG_INCLUDE_H
#define _CFG_INCLUDE_H

#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread/mutex.hpp>
class Cfg
{
public:
	Cfg();
	~Cfg();
public:
	std::string getCfg(const std::string& sName, const std::string& sDefaultValue);
	void setCfg(const std::string& sName, const std::string& sValue);
public:
	bool LoadCfg(const std::string& sFilePath);
	bool SaveCfg();
private:
	std::string sFilePath_;
	boost::property_tree::ptree ptParse_;
	boost::mutex mutex_cfg_;
};

#endif