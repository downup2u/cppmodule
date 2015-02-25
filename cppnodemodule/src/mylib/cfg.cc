#include "cfg.h"
#include <glog.h>

Cfg::Cfg()
{
}


Cfg::~Cfg()
{
}

std::string Cfg::getCfg(const std::string& sName, const std::string& sDefaultValue)
{
	std::string sValue;
	mutex_cfg_.lock();
	try
	{
		sValue = ptParse_.get<std::string>(sName, sDefaultValue);
	}
	catch (std::exception const&  ex)
	{
		std::string err = ex.what();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "get cfg error,name:" << sName << ",sDefaultValue:" << sDefaultValue << ",reason:[" << err << "]";
	}
	catch (...)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "get cfg error,name:" << sName << ",sDefaultValue:" << sDefaultValue << ",unknow reason";
	}
	mutex_cfg_.unlock();
	return sValue;
}

void Cfg::setCfg(const std::string& sName, const std::string& sValue)
{
	mutex_cfg_.lock();
	try
	{
		ptParse_.put(sName, sValue);
	}
	catch (std::exception const&  ex)
	{
		std::string err = ex.what();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "set cfg error,name:" << sName << ",Value:" << sValue << ",reason:[" << err << "]";
	}
	catch (...)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "set cfg error,name:" << sName << ",Value:" << sValue << ",unknow reason";
	}
	mutex_cfg_.unlock();
}

bool Cfg::LoadCfg(const std::string& sFilePath)
{
	sFilePath_ = sFilePath;
	bool bSuc = false;
	mutex_cfg_.lock();
	try
	{
		boost::property_tree::read_xml(sFilePath_, ptParse_, boost::property_tree::xml_parser::trim_whitespace);
		bSuc = true;
	}
	catch (std::exception const&  ex)
	{
		std::string err = ex.what();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "read xml error,reason:[" << err << "]";
	}
	catch (...)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "read xml error,unknow reason";
	}
	mutex_cfg_.unlock();
	return bSuc;
}

bool Cfg::SaveCfg()
{
	bool bSuc = false;
	mutex_cfg_.lock();
	try
	{
		boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
		boost::property_tree::write_xml(sFilePath_, ptParse_, std::locale(), settings);
		bSuc = true;
	}
	catch (std::exception const&  ex)
	{
		std::string err = ex.what();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "write xml error,reason:[" << err << "]";
	}
	catch (...)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "write xml error,know reason";
	}
	mutex_cfg_.unlock();
	return bSuc;
}
