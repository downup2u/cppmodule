#ifndef _UTIL_LANGUAGE
#define _UTIL_LANGUAGE
#include <string>
#include <cfg.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/singleton.hpp> 
class UtilLanguage :public boost::serialization::singleton<UtilLanguage>
{
public:
	UtilLanguage();
	~UtilLanguage();
public:
	void addLanguage(const std::string&sLanguage, const std::string& sXML, bool isDefault = false);
	std::string getLanguageString(const std::string&sLanguage, const std::string&sDotPath);
private:
	std::map<const std::string, boost::shared_ptr<Cfg> > mapCfg_;
	std::string sDefaultLanguage_;
};


#endif