#include "DataWrap.h"
#include <json2pb.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/hex.hpp>
#include "mylib/util/codec.h"
#include <glog.h>
#include "mylib/util/utillanguage.h"
#include <boost/algorithm/string.hpp>
#include "mylib/cfg.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "nodeexception.h"
#include <encryptstring.h>

DataWrap::DataWrap()
{

}

DataWrap::~DataWrap()
{

}

bool DataWrap::getConnectDBString(const std::string&sXML, std::string&sConnectString, int&nNum)
{
	Cfg cfg;
	if (cfg.LoadCfg(sXML))
	{
		std::string spool = cfg.getCfg("root.db.poolnum", "4");
		std::string host = cfg.getCfg("root.db.host","localhost");
		std::string dbname = cfg.getCfg("root.db.dbname", "task");
		std::string user = cfg.getCfg("root.db.user", "");
		std::string password = cfg.getCfg("root.db.password", "");
		std::string charset = cfg.getCfg("root.db.charset", "utf-8");
		std::string key = cfg.getCfg("root.db.key", "");
		std::string password_decode;
		decodeString(password, password_decode);
		nNum = boost::lexical_cast<int>(spool);
		boost::format fmt("db=%s host=%s user=%s password=%s charset=%s");
		fmt%dbname%host%user%password_decode%charset;
		sConnectString = fmt.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::trace) << "sConnectString:" << sConnectString;

		return true;
	}
	return false;
}

bool DataWrap::initDB(const std::string&sXML)
{
	std::string sConnectString;
	int nNum = 4;
	getConnectDBString(sXML, sConnectString,nNum);
	return initDBPool(nNum, sConnectString);
}

boost::shared_ptr<google::protobuf::Message> DataWrap::getMsgFromHex(const std::string& classname, const std::string& hex)
{
	boost::shared_ptr<google::protobuf::Message> pMsg = CodecUtil::createMsgFromTypeName(classname);
	if (pMsg)
	{
		try
		{
			std::string sRawData = boost::algorithm::unhex(hex);
			pMsg->ParseFromString(sRawData);			
		}
		catch (const std::exception&e)
		{
			std::string err = e.what();
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str(err);
		}
		catch (...)
		{
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str("unknowerror");
		}
	}
	return pMsg;
}

boost::shared_ptr<google::protobuf::Message> DataWrap::getMsgFromJson(const std::string& classname, const std::string& json)
{
	boost::shared_ptr<google::protobuf::Message> pMsg = CodecUtil::createMsgFromTypeName(classname);
	if (pMsg)
	{
		try
		{
			json2pb(*pMsg, json.c_str(), json.length());
		}	
		catch (const std::exception&e)
		{
			std::string err = e.what();
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str(err);
		}
		catch (...)
		{
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str("unknowerror");
		}
	}
	return pMsg;
}

bool DataWrap::initDBPool(int poolsize, const std::string& connectString)
{
	poolsize_ = poolsize;
	connectstring_ = connectString;	
	return onOpenDB();
}



bool DataWrap::getDataMsg(const std::string& sInTypeName, const std::string& sInHexData, std::string& sSessionHexData, std::string& sOutTypeName, std::string&sOutHexData, bool isHex/* = true*/)
{
	bool bSuc = false;
	boost::shared_ptr<google::protobuf::Message> pMsg;
	boost::shared_ptr<google::protobuf::Message> pMsgRet;
	boost::shared_ptr<google::protobuf::Message> pMsgSession = getMsgFromHex("session.PkgSessionUser", sSessionHexData);
	boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser;
	if (pMsgSession)
	{
		pPkgSessionUser = boost::dynamic_pointer_cast<session::PkgSessionUser>(pMsgSession);
	}
	else
	{
		pPkgSessionUser = boost::make_shared<session::PkgSessionUser>();
		pPkgSessionUser->set_userid(0);
	}

	try
	{
		
		if (!pPkgSessionUser)
		{
			sOutTypeName = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.sessiondataerr");
			sOutHexData = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.sessiondataerr");
			//break;
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("nosessionuser") << err_str(sInTypeName);
		}
		if (isHex)
		{
			pMsg = getMsgFromHex(sInTypeName, sInHexData);
		}
		else
		{
			pMsg = getMsgFromJson(sInTypeName, sInHexData);
		}

		if (!pMsg)
		{
			sOutTypeName = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errtype.invalidparam");
			sOutHexData = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.typenameinvalid") + sInTypeName;
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("no message") << err_str(sInTypeName);
		}
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << "=============================";
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << "Recvmsg Type:" << pMsg->GetTypeName();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << "Recvmsg Content:" << pb2json(*pMsg);

		pMsgRet = recvMsgWithSession(pMsg, pPkgSessionUser);
		if (!pMsgRet)
		{
			sOutTypeName = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errtype.invalidparam");
			sOutHexData = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.typenamenotsupport") + sInTypeName;
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("errormessage") << err_str(sInTypeName);
		}
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << "SendMsgType:" << pMsgRet->GetTypeName();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << "SendMsgContent:" << pb2json(*pMsgRet);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << std::endl;

		sOutTypeName = pMsgRet->GetTypeName();

		if (isHex)
		{
			pMsgRet->AppendToString(&sOutHexData);
			sOutHexData = boost::algorithm::hex(sOutHexData);
		}
		else
		{
			sOutHexData = pb2json(*pMsgRet);
		}
		bSuc = true;
		
	}
	catch (const std::exception &e)
	{
		int err = *boost::get_error_info<err_no>(e);
		sOutTypeName = *boost::get_error_info<errtype_str>(e);
		sOutHexData = *boost::get_error_info<err_str>(e);
	
		if (err == ERROR_DB)
		{
			onOpenDB();
		}
	
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << "ErrorCode:" << err << ",Type:" << sOutTypeName << ",Detail:" << sOutHexData;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << std::endl;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "ExceptionInfo:" << boost::diagnostic_information(e);
	}
	catch (...)
	{
		sOutTypeName = "unknowerror";
		sOutHexData = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errtype.unknowerror");
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "UnknowException";
	}

	std::string sData;
	pPkgSessionUser->AppendToString(&sData);
	sSessionHexData = boost::algorithm::hex(sData);

	return bSuc;
}

boost::shared_ptr<google::protobuf::Message> DataWrap::recvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	boost::shared_ptr<google::protobuf::Message> pMsgRet = onRecvMsgWithSession(pMsg, pPkgSessionUser);
	if (pMsgRet)
		return pMsgRet;

	throw nodeexception() << err_no(ERROR_MSG) << errtype_str("typenamenotsupport") << err_str(pMsg->GetTypeName());
	//throw std::exception("typenamenotsupport");
	return NULL;
}



