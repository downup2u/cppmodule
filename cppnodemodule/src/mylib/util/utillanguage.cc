#include "utillanguage.h"
#include <boost/make_shared.hpp>
#include <glog.h>
UtilLanguage::UtilLanguage()
{
	sDefaultLanguage_ = "chinese";
}

UtilLanguage::~UtilLanguage()
{

}

void UtilLanguage::addLanguage(const std::string&sLanguage, const std::string& sXML, bool isDefault /*= false*/)
{
	if (isDefault)
		sDefaultLanguage_ = sLanguage;
	boost::shared_ptr<Cfg> pCfg = boost::make_shared<Cfg>();
	pCfg->LoadCfg(sXML);
	mapCfg_[sLanguage] = pCfg;
}

std::string UtilLanguage::getLanguageString(const std::string&sLanguage, const std::string&sDotPath)
{
	std::string sRet;
	boost::shared_ptr<Cfg> pCfg;
	if (sLanguage.empty())
	{
		pCfg = mapCfg_[sDefaultLanguage_];
	}
	else
	{
		pCfg = mapCfg_[sLanguage];
	}

	if (pCfg)
	{
		sRet = pCfg->getCfg(sDotPath, "");
	}

	return sRet;
}
