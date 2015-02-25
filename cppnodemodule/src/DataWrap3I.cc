#include "DataWrap3I.h"
#include <json2pb.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/hex.hpp>
#include "mylib/util/codec.h"
#include "EncryptWrap.h"
#include <glog.h>
#include "mylib/util/utillanguage.h"
#include <boost/algorithm/string.hpp>
#include "nodeexception.h"

DataWrap3I::DataWrap3I()
{

}

DataWrap3I::~DataWrap3I()
{

}



bool DataWrap3I::onOpenDB()
{
	bool bSuc = false;
	try
	{
		dbSession_ = boost::make_shared<DBSession3I>();
		bSuc = dbSession_->initSession(poolsize_, connectstring_);
	}
	catch (...)
	{
	}
	return bSuc;
}

boost::shared_ptr<google::protobuf::Message> DataWrap3I::onRecvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser)
{
	bool bSuc = false;
	boost::shared_ptr<google::protobuf::Message> pRetMsg;
	const std::string &name = pMsg->GetTypeName();
	try
	{
		if (boost::starts_with(name, "thirdinterface."))
		{//第三方接口/短信/邮件相关
			if (name == "thirdinterface.PkgGetPasswordTableReq")
			{
				boost::shared_ptr<thirdinterface::PkgGetPasswordTableReq> pPkgGetPasswordTableReq = boost::dynamic_pointer_cast<thirdinterface::PkgGetPasswordTableReq>(pMsg);
				if (pPkgGetPasswordTableReq)
				{
					boost::shared_ptr<thirdinterface::PkgGetPasswordTableReply> pPkgGetPasswordTableReply = boost::make_shared<thirdinterface::PkgGetPasswordTableReply>();
					bSuc = dbSession_->handleThirdMsg_getGetPasswordTable(pPkgGetPasswordTableReq, pPkgGetPasswordTableReply, pPkgSessionUser);
					pRetMsg = pPkgGetPasswordTableReply;
				}
			}
			else if (name == "thirdinterface.PkgSetGetPasswordTableStatusReq")
			{
				boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReq> pPkgSetGetPasswordTableStatusReq = boost::dynamic_pointer_cast<thirdinterface::PkgSetGetPasswordTableStatusReq>(pMsg);
				if (pPkgSetGetPasswordTableStatusReq)
				{
					boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReply> pPkgSetGetPasswordTableStatusReply = boost::make_shared<thirdinterface::PkgSetGetPasswordTableStatusReply>();
					bSuc = dbSession_->handleThirdMsg_setGetPasswordTableStatus(pPkgSetGetPasswordTableStatusReq, pPkgSetGetPasswordTableStatusReply, pPkgSessionUser);
					pRetMsg = pPkgSetGetPasswordTableStatusReply;
				}
			}
			else if (name == "thirdinterface.PkgSysInfoListGetReq")
			{
				boost::shared_ptr<thirdinterface::PkgSysInfoListGetReq> pPkgSysInfoReq = boost::dynamic_pointer_cast<thirdinterface::PkgSysInfoListGetReq>(pMsg);
				if (pPkgSysInfoReq)
				{
					boost::shared_ptr<thirdinterface::PkgSysInfoListGetReply> pPkgSysInfoReply = boost::make_shared<thirdinterface::PkgSysInfoListGetReply>();
					bSuc = dbSession_->handleThirdMsg_getSysInfoList(pPkgSysInfoReq, pPkgSysInfoReply, pPkgSessionUser);
					pRetMsg = pPkgSysInfoReply;
				}
			}
			else if (name == "thirdinterface.PkgRegisterInvitionReq")
			{
				boost::shared_ptr<thirdinterface::PkgRegisterInvitionReq> pPkgRegisterInvitionReq = boost::dynamic_pointer_cast<thirdinterface::PkgRegisterInvitionReq>(pMsg);
				if (pPkgRegisterInvitionReq)
				{
					boost::shared_ptr<thirdinterface::PkgRegisterInvitionReply> pPkgRegisterInvitionReply = boost::make_shared<thirdinterface::PkgRegisterInvitionReply>();
					bSuc = dbSession_->handleThirdMsg_getRegisterInvitionReq(pPkgRegisterInvitionReq, pPkgRegisterInvitionReply, pPkgSessionUser);
					pRetMsg = pPkgRegisterInvitionReply;
				}

			}
			else if (name == "thirdinterface.PkgSetRegisterInvitionStatusReq")
			{
				boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReq> pPkgSetRegisterInvitionStatusReq = boost::dynamic_pointer_cast<thirdinterface::PkgSetRegisterInvitionStatusReq>(pMsg);
				if (pPkgSetRegisterInvitionStatusReq)
				{
					boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReply> pPkgSetRegisterInvitionStatusReply = boost::make_shared<thirdinterface::PkgSetRegisterInvitionStatusReply>();
					bSuc = dbSession_->handleThirdMsg_setRegisterInvitionStatus(pPkgSetRegisterInvitionStatusReq, pPkgSetRegisterInvitionStatusReply, pPkgSessionUser);
					pRetMsg = pPkgSetRegisterInvitionStatusReply;
				}

			}

		}
	}
	catch (soci::soci_error &err)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "(Error),Type:" << name << ",ErrorInfo:" << err.what();
		//throw std::exception("dberror");
		throw nodeexception() << err_no(ERROR_DB) << errtype_str("dberror") << err_str(err.what());

	}
	return pRetMsg;
}




