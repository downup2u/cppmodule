#include "DataWrapAdmin.h"
#include <json2pb.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/hex.hpp>
#include "mylib/util/codec.h"
#include <glog.h>
#include "mylib/util/utillanguage.h"
#include <boost/algorithm/string.hpp>
#include "nodeexception.h"

DataWrapAdmin::DataWrapAdmin()
{

}

DataWrapAdmin::~DataWrapAdmin()
{

}

bool DataWrapAdmin::onOpenDB()
{
	bool bSuc = false;
	try
	{
		dbSession_ = boost::make_shared<DBSessionAdmin>();
		bSuc = dbSession_->initSession(poolsize_, connectstring_);
	}
	catch (...)
	{
	}
	return bSuc;
}


boost::shared_ptr<google::protobuf::Message> DataWrapAdmin::onRecvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser)
{
	boost::shared_ptr<google::protobuf::Message> pRetMsg;
	const std::string &name = pMsg->GetTypeName();
	try
	{
		bool bSuc = false;
		if (boost::starts_with(name, "admin.PkgUser"))
		{//后台用户相关
			if (name == "admin.PkgUserLoginReq")
			{
				boost::shared_ptr<admin::PkgUserLoginReq> pPkgUserLoginReq = boost::dynamic_pointer_cast<admin::PkgUserLoginReq>(pMsg);
				if (pPkgUserLoginReq)
				{
					boost::shared_ptr<admin::PkgUserLoginReply> pPkgUserLoginReply = boost::make_shared<admin::PkgUserLoginReply>();
					bSuc = dbSession_->handleAdminMsg_userLogin(pPkgUserLoginReq, pPkgUserLoginReply, pPkgSessionUser);
					if (pPkgUserLoginReply->issuccess())
					{//设置session相关字段
						pPkgSessionUser->set_userid(pPkgUserLoginReply->userid());
						//pPkgSessionUser->set_permission(pPkgUserLoginReply->permission());
					}
					pRetMsg = pPkgUserLoginReply;
				}

			}
			else if (name == "admin.PkgUserOperationReq")
			{
				boost::shared_ptr<admin::PkgUserOperationReq> pPkgUserOperationReq = boost::dynamic_pointer_cast<admin::PkgUserOperationReq>(pMsg);
				if (pPkgUserOperationReq)
				{
					boost::shared_ptr<admin::PkgUserOperationReply> pPkgUserOperationReply = boost::make_shared<admin::PkgUserOperationReply>();
					bSuc = dbSession_->handleAdminMsg_userOperation(pPkgUserOperationReq, pPkgUserOperationReply, pPkgSessionUser);
					pRetMsg = pPkgUserOperationReply;
				}

			}
		}
		else if (boost::starts_with(name, "admin.PkgAdv"))
		{
			if (name == "admin.PkgAdvQueryReq")
			{
				boost::shared_ptr<admin::PkgAdvQueryReq> pPkgAdvQueryReq = boost::dynamic_pointer_cast<admin::PkgAdvQueryReq>(pMsg);
				if (pPkgAdvQueryReq)
				{
					boost::shared_ptr<admin::PkgAdvQueryReply> pPkgAdvQueryReply = boost::make_shared<admin::PkgAdvQueryReply>();
					bSuc = dbSession_->handleMessage_advQuery(pPkgAdvQueryReq, pPkgAdvQueryReply, pPkgSessionUser);
					pRetMsg = pPkgAdvQueryReply;
				}

			}
			else if (name == "admin.PkgAdvOperationReq")
			{
				boost::shared_ptr<admin::PkgAdvOperationReq> pPkgAdvOperationReq = boost::dynamic_pointer_cast<admin::PkgAdvOperationReq>(pMsg);
				if (pPkgAdvOperationReq)
				{
					boost::shared_ptr<admin::PkgAdvOperationReply> pPkgAdvOperationReply = boost::make_shared<admin::PkgAdvOperationReply>();
					bSuc = dbSession_->handleMessage_advOperation(pPkgAdvOperationReq, pPkgAdvOperationReply, pPkgSessionUser);
					pRetMsg = pPkgAdvOperationReply;
				}

			}

		}
		else if (name == "admin.PkgSysInfoSetReq")
		{
			boost::shared_ptr<admin::PkgSysInfoSetReq> pPkgSysInfoReq = boost::dynamic_pointer_cast<admin::PkgSysInfoSetReq>(pMsg);
			if (pPkgSysInfoReq)
			{
				boost::shared_ptr<admin::PkgSysInfoSetReply> pPkgSysInfoReply = boost::make_shared<admin::PkgSysInfoSetReply>();
				bSuc = dbSession_->handleThirdMsg_setSysInfo(pPkgSysInfoReq, pPkgSysInfoReply, pPkgSessionUser);
				pRetMsg = pPkgSysInfoReply;
			}
		}
		else if (name == "admin.PkgSysInfoGetReq")
		{
			boost::shared_ptr<admin::PkgSysInfoGetReq> pPkgGetSysInfoReq = boost::dynamic_pointer_cast<admin::PkgSysInfoGetReq>(pMsg);
			if (pPkgGetSysInfoReq)
			{
				boost::shared_ptr<admin::PkgSysInfoGetReply> pPkgGetSysInfoReply = boost::make_shared<admin::PkgSysInfoGetReply>();
				bSuc = dbSession_->handleThirdMsg_getSysInfo(pPkgGetSysInfoReq, pPkgGetSysInfoReply, pPkgSessionUser);
				pRetMsg = pPkgGetSysInfoReply;
			}
		}
	}
	catch (soci::soci_error &err)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::info) << "(Error),Type:" << name << ",ErrorInfo:" << err.what();
		throw nodeexception() << err_no(ERROR_DB) << errtype_str("dberror") << err_str(err.what());
		//throw std::exception("dberror");
	}
	return pRetMsg;
}



