#include "DataWrapComm.h"
#include <json2pb.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/hex.hpp>
#include "mylib/util/codec.h"
#include <glog.h>
#include "mylib/util/utillanguage.h"
#include <boost/algorithm/string.hpp>
#include "nodeexception.h"
DataWrapComm::DataWrapComm()
{

}

DataWrapComm::~DataWrapComm()
{

}

bool DataWrapComm::onOpenDB()
{
	bool bSuc = false;
	try
	{
		dbSession_ = boost::make_shared<DBSessionComm>();
		bSuc = dbSession_->initSession(poolsize_, connectstring_);
	}
	catch (...)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "cannot opendb,db error";
	}
	return bSuc;
}


boost::shared_ptr<google::protobuf::Message> DataWrapComm::onRecvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser)
{
	bool bSuc = false;
	boost::shared_ptr<google::protobuf::Message> pRetMsg;
	const std::string &name = pMsg->GetTypeName();
	try
	{
		if (name == "comm.PkgLogoutReq"){//ע��
			boost::shared_ptr<comm::PkgLogoutReply> pPkgConnectReply = boost::make_shared<comm::PkgLogoutReply>();
			pRetMsg = pPkgConnectReply;
			pPkgSessionUser->set_userid(0);
		}
		else if (name == "comm.PkgNotifyReq"){
			//�ȵ�¼
			dbSession_->isSessionUserVaildWithException(pPkgSessionUser);
			boost::shared_ptr<comm::PkgNotifyReq> pPkgNotifyReq = boost::dynamic_pointer_cast<comm::PkgNotifyReq>(pMsg);
			if (pPkgNotifyReq)
			{
				boost::shared_ptr<comm::PkgNotifyReply> pPkgNotifyReply = boost::make_shared<comm::PkgNotifyReply>();
				std::string sUpdateTime = pPkgNotifyReq->dataupatedtime();
				int nUpdateFlag = dbSession_->getUserUpdateFlag(pPkgSessionUser->userid(), sUpdateTime);
				int nUpdateForceFlag = pPkgNotifyReq->forceupdateflag();
				nUpdateFlag = nUpdateFlag | nUpdateForceFlag;
				pPkgNotifyReply->set_dataupatedtime(sUpdateTime);
				pPkgNotifyReply->set_notifyupdatedflag(nUpdateFlag);//ǿ�Ƹ���ʱҲ��Ҫflag.
				if (nUpdateFlag & comm::EnUpdatedFlag::UF_COMPANYINFO){
					//get company info
					comm::PkgCompanyInfo*pPkgCompanyInfo = dbSession_->getCompanyInfo(pPkgSessionUser);
					pPkgNotifyReply->set_allocated_companyinfo(pPkgCompanyInfo);
					dbSession_->clearUserUpdateFlag(pPkgSessionUser->userid(), comm::EnUpdatedFlag::UF_COMPANYINFO);
				}
				if ( nUpdateFlag & comm::EnUpdatedFlag::UF_MYTASKFINISHEDNUMBERS){
					//���������
					int taskFinished = 0;
					int taskNotFinished = 0;
					dbSession_->getTaskNumber(pPkgSessionUser, taskFinished, taskNotFinished);
					pPkgNotifyReply->set_tasknumberfinished(taskFinished);
					pPkgNotifyReply->set_tasknumbernotfinished(taskNotFinished);
					dbSession_->clearUserUpdateFlag(pPkgSessionUser->userid(), comm::EnUpdatedFlag::UF_MYTASKFINISHEDNUMBERS);
				}
				if (nUpdateFlag & comm::EnUpdatedFlag::UF_VERSION)
				{//���°汾
					std::string versionlastest;
					std::string versiondownloadurl;
					dbSession_->getLastestVersion(pPkgNotifyReq->platmform(), versionlastest, versiondownloadurl);
					pPkgNotifyReply->set_versionlastest(versionlastest);
					pPkgNotifyReply->set_versiondownloadurl(versiondownloadurl);
					dbSession_->clearUserUpdateFlag(pPkgSessionUser->userid(), comm::EnUpdatedFlag::UF_VERSION);
				}
				if (nUpdateFlag & comm::EnUpdatedFlag::UF_USERROLE)
				{
					boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = dbSession_->getRoles(pPkgSessionUser);
					if (pPkgRoleUser)
					{
						pPkgNotifyReply->set_permission(pPkgRoleUser->permission());
						pPkgNotifyReply->set_permissionroleid(pPkgRoleUser->id());
						pPkgNotifyReply->set_rolename(pPkgRoleUser->rolename());
					}
				}
				pRetMsg = pPkgNotifyReply;
			}
		}
		else if (name == "comm.PkgConnectReq")
		{//��һ������
			boost::shared_ptr<comm::PkgConnectReq> pPkgConnectReq = boost::dynamic_pointer_cast<comm::PkgConnectReq>(pMsg);
			if (pPkgConnectReq)
			{//Connect
				boost::shared_ptr<comm::PkgConnectReply> pPkgConnectReply = boost::make_shared<comm::PkgConnectReply>();

				pPkgSessionUser->set_apptype(pPkgConnectReq->apptype());
				pPkgSessionUser->set_appversion(pPkgConnectReq->appversion());
				pPkgSessionUser->set_applanguage(pPkgConnectReq->applanguage());
				pPkgSessionUser->set_ostype(pPkgConnectReq->ostype());
				pPkgSessionUser->set_osversion(pPkgConnectReq->osversion());
				pPkgSessionUser->set_osrr(pPkgConnectReq->osrr());
				pPkgSessionUser->set_clientsn(pPkgConnectReq->clientsn());
				//std::string hashedclientsn = EncryptWrap::get_mutable_instance().hashedSn(pPkgConnectReq->clientsn());
				//std::string encryptkey = EncryptWrap::get_mutable_instance().generateRandomSn();
				//std::string serversn = EncryptWrap::get_mutable_instance().generateRandomSn();
				//pPkgSessionUser->set_serversn(serversn);
				//pPkgSessionUser->set_encryptkey(encryptkey);
				//pPkgSessionUser->set_hashedserversn(EncryptWrap::get_mutable_instance().hashedSn(serversn));

				pPkgConnectReply->set_issuccess(true);
				//pPkgConnectReply->set_encryptkey(encryptkey);
				//pPkgConnectReply->set_hashedclientsn(hashedclientsn);
				//pPkgConnectReply->set_serversn(serversn);

				pRetMsg = pPkgConnectReply;
			}
		}
		else if (boost::starts_with(name, "comm.PkgUser"))
		{//�û����
			if (name == "comm.PkgUserLoginReq")
			{
				boost::shared_ptr<comm::PkgUserLoginReq> pPkgUserLoginReq = boost::dynamic_pointer_cast<comm::PkgUserLoginReq>(pMsg);
				if (pPkgUserLoginReq)
				{
					bSuc = true;
					boost::shared_ptr<comm::PkgUserLoginReply> pPkgUserLoginReply = boost::make_shared<comm::PkgUserLoginReply>();
					if (pPkgSessionUser->has_serversn())
					{//��֤�Ƿ�Ƿ��ͻ���
						if (pPkgSessionUser->hashedserversn() != pPkgUserLoginReq->hashedserversn())
						{
							bSuc = false;
							pPkgUserLoginReply->set_issuccess(false);
							pPkgUserLoginReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.illegalclient"));
						}
					}

					if (pPkgSessionUser->has_encryptkey())
					{//��������
						//std::string spassword = EncryptWrap::get_mutable_instance().decryptString(pPkgUserLoginReq->password(), pPkgSessionUser->encryptkey());
						//pPkgUserLoginReq->set_password(spassword);
					}

					if (bSuc)
					{
						bSuc = dbSession_->handleMessage_userLogin(pPkgUserLoginReq, pPkgUserLoginReply, pPkgSessionUser);
						if (pPkgUserLoginReply->issuccess())
						{//����session����ֶ�
							std::string sn = generateRandomSn();
							pPkgSessionUser->set_loginsn(sn);
							dbSession_->setUserLoginSn(pPkgSessionUser->userid(), sn);
						}
					}
					pRetMsg = pPkgUserLoginReply;
				}
			}
			else if (name == "comm.PkgUserCreateReq")
			{
				boost::shared_ptr<comm::PkgUserCreateReq> pPkgUserCreateReq = boost::dynamic_pointer_cast<comm::PkgUserCreateReq>(pMsg);
				if (pPkgUserCreateReq)
				{
					boost::shared_ptr<comm::PkgUserCreateReply> pPkgUserCreateReply = boost::make_shared<comm::PkgUserCreateReply>();
					bSuc = dbSession_->handleMessage_userCreateUser(pPkgUserCreateReq, pPkgUserCreateReply, pPkgSessionUser);
					pRetMsg = pPkgUserCreateReply;
				}
			}
			else if (name == "comm.PkgUserCheckReq")
			{
				boost::shared_ptr<comm::PkgUserCheckReq> pPkgUserCheckReq = boost::dynamic_pointer_cast<comm::PkgUserCheckReq>(pMsg);
				if (pPkgUserCheckReq)
				{
					boost::shared_ptr<comm::PkgUserCheckReply> pPkgUserCreateReply = boost::make_shared<comm::PkgUserCheckReply>();
					bSuc = dbSession_->handleMessage_userCheck(pPkgUserCheckReq, pPkgUserCreateReply, pPkgSessionUser);
					pRetMsg = pPkgUserCreateReply;
				}
			}
			else if (name == "comm.PkgUserGetPasswordReq")
			{
				boost::shared_ptr<comm::PkgUserGetPasswordReq> pPkgUserGetPasswordReq = boost::dynamic_pointer_cast<comm::PkgUserGetPasswordReq>(pMsg);
				if (pPkgUserGetPasswordReq)
				{
					boost::shared_ptr<comm::PkgUserGetPasswordReply> pPkgUserGetPasswordReply = boost::make_shared<comm::PkgUserGetPasswordReply>();
					bSuc = dbSession_->handleMessage_userGetPassword(pPkgUserGetPasswordReq, pPkgUserGetPasswordReply, pPkgSessionUser);
					pRetMsg = pPkgUserGetPasswordReply;
				}

			}
			else if (name == "comm.PkgUserResetPasswordReq")
			{
				boost::shared_ptr<comm::PkgUserResetPasswordReq> pPkgUserResetPasswordReq = boost::dynamic_pointer_cast<comm::PkgUserResetPasswordReq>(pMsg);
				if (pPkgUserResetPasswordReq)
				{
					boost::shared_ptr<comm::PkgUserResetPasswordReply> pPkgUserResetPasswordReply = boost::make_shared<comm::PkgUserResetPasswordReply>();
					bSuc = dbSession_->handleMessage_userResetPassword(pPkgUserResetPasswordReq, pPkgUserResetPasswordReply, pPkgSessionUser);
					pRetMsg = pPkgUserResetPasswordReply;
				}

			}
			else if (name == "comm.PkgUserSetReq")
			{
				boost::shared_ptr<comm::PkgUserSetReq> pPkgUserSetReq = boost::dynamic_pointer_cast<comm::PkgUserSetReq>(pMsg);
				if (pPkgUserSetReq)
				{
					boost::shared_ptr<comm::PkgUserSetReply> pPkgUserSetReply = boost::make_shared<comm::PkgUserSetReply>();
					bSuc = dbSession_->handleMessage_userSet(pPkgUserSetReq, pPkgUserSetReply, pPkgSessionUser);
					pRetMsg = pPkgUserSetReply;
				}
			}
			else if (name == "comm.PkgUserGetAuthReq")
			{
				boost::shared_ptr<comm::PkgUserGetAuthReq> pPkgUserGetAuthReq = boost::dynamic_pointer_cast<comm::PkgUserGetAuthReq>(pMsg);
				if (pPkgUserGetAuthReq)
				{
					boost::shared_ptr<comm::PkgUserGetAuthReply> pPkgUserGetAuthReply = boost::make_shared<comm::PkgUserGetAuthReply>();
					bSuc = dbSession_->handleMessage_userGetAuth(pPkgUserGetAuthReq, pPkgUserGetAuthReply, pPkgSessionUser);
					pRetMsg = pPkgUserGetAuthReply;
				}

			}
			else if (name == "comm.PkgUserInvitionUserReq")
			{//�����û�ע��
				//�ȵ�¼
				dbSession_->isSessionUserVaildWithException(pPkgSessionUser);
			
				boost::shared_ptr<comm::PkgUserInvitionUserReq> pPkgUserInvitionUserReq = boost::dynamic_pointer_cast<comm::PkgUserInvitionUserReq>(pMsg);
				if (pPkgUserInvitionUserReq)
				{
					boost::shared_ptr<comm::PkgUserInvitionUserReply> pPkgUserInvitionUserReply = boost::make_shared<comm::PkgUserInvitionUserReply>();
					bSuc = dbSession_->handleMessage_userInvitionUser(pPkgUserInvitionUserReq, pPkgUserInvitionUserReply, pPkgSessionUser);
					pRetMsg = pPkgUserInvitionUserReply;
				}
		
			}
			else if (name == "comm.PkgUserCheckInvitionCodeReq")
			{//��֤�������Ƿ�Ϸ�
				boost::shared_ptr<comm::PkgUserCheckInvitionCodeReq> pPkgUserCheckInvitionCodeReq = boost::dynamic_pointer_cast<comm::PkgUserCheckInvitionCodeReq>(pMsg);
				if (pPkgUserCheckInvitionCodeReq)
				{
					boost::shared_ptr<comm::PkgUserCheckInvitionCodeReply> pPkgUserCheckInvitionCodeReply = boost::make_shared<comm::PkgUserCheckInvitionCodeReply>();
					bSuc = dbSession_->handleMessage_userCheckInvitionCode(pPkgUserCheckInvitionCodeReq, pPkgUserCheckInvitionCodeReply, pPkgSessionUser);
					pRetMsg = pPkgUserCheckInvitionCodeReply;
				}
			}
			else if (name == "comm.PkgUserInvitionCheckReq")
			{//ͨ�������������֯
				//�ȵ�¼
				dbSession_->isSessionUserVaildWithException(pPkgSessionUser);


				boost::shared_ptr<comm::PkgUserInvitionCheckReq> pPkgUserInvitionCheckReq = boost::dynamic_pointer_cast<comm::PkgUserInvitionCheckReq>(pMsg);
				if (pPkgUserInvitionCheckReq)
				{
					boost::shared_ptr<comm::PkgUserInvitionCheckReply> pPkgUserInvitionCheckReply = boost::make_shared<comm::PkgUserInvitionCheckReply>();
					bSuc = dbSession_->handleMessage_userInvitionCheck(pPkgUserInvitionCheckReq, pPkgUserInvitionCheckReply, pPkgSessionUser);
					pRetMsg = pPkgUserInvitionCheckReply;
				}
			}
			else if (name == "comm.PkgUserCreateCompanyReq")
			{
				//������֯
				//�ȵ�¼
				dbSession_->isSessionUserVaildWithException(pPkgSessionUser);

				boost::shared_ptr<comm::PkgUserCreateCompanyReq> pPkgUserCreateCompanyReq = boost::dynamic_pointer_cast<comm::PkgUserCreateCompanyReq>(pMsg);
				if (pPkgUserCreateCompanyReq)
				{
					boost::shared_ptr<comm::PkgUserCreateCompanyReply> pPkgUserCreateCompanyReply = boost::make_shared<comm::PkgUserCreateCompanyReply>();
					bSuc = dbSession_->handleMessage_userCreateCompany(pPkgUserCreateCompanyReq, pPkgUserCreateCompanyReply, pPkgSessionUser);
					pRetMsg = pPkgUserCreateCompanyReply;
				}
			}
			else if (name == "comm.PkgUserReassignAdminReq")
			{
				//ת�ù���Ա
				//�ȵ�¼
				dbSession_->isSessionUserVaildWithException(pPkgSessionUser);

				boost::shared_ptr<comm::PkgUserReassignAdminReq> pPkgUserReassignAdminReq = boost::dynamic_pointer_cast<comm::PkgUserReassignAdminReq>(pMsg);
				if (pPkgUserReassignAdminReq)
				{
					boost::shared_ptr<comm::PkgUserReassignAdminReply> pPkgUserReassignAdminReply = boost::make_shared<comm::PkgUserReassignAdminReply>();
					bSuc = dbSession_->handleMessage_userReassignAdmin(pPkgUserReassignAdminReq, pPkgUserReassignAdminReply, pPkgSessionUser);
					pRetMsg = pPkgUserReassignAdminReply;
				}
			}
			else if (name == "comm.PkgUserDeleteFromCompanyReq")
			{//����֯��ɾ����Ա�����޹���Ա��
				//�ȵ�¼
				dbSession_->isSessionUserVaildWithException(pPkgSessionUser);

				boost::shared_ptr<comm::PkgUserDeleteFromCompanyReq> pPkgUserDeleteFromCompanyReq = boost::dynamic_pointer_cast<comm::PkgUserDeleteFromCompanyReq>(pMsg);
				if (pPkgUserDeleteFromCompanyReq)
				{
					boost::shared_ptr<comm::PkgUserDeleteFromCompanyReply> pPkgUserDeleteFromCompanyReply = boost::make_shared<comm::PkgUserDeleteFromCompanyReply>();
					bSuc = dbSession_->handleMessage_userDeleteFromCompany(pPkgUserDeleteFromCompanyReq, pPkgUserDeleteFromCompanyReply, pPkgSessionUser);
					pRetMsg = pPkgUserDeleteFromCompanyReply;
				}
			}

			else if (name == "comm.PkgUserCheckInvitionCodeReq")
			{//��֤�������Ƿ�Ϸ�
				boost::shared_ptr<comm::PkgUserCheckInvitionCodeReq> pPkgUserCheckInvitionCodeReq = boost::dynamic_pointer_cast<comm::PkgUserCheckInvitionCodeReq>(pMsg);
				if (pPkgUserCheckInvitionCodeReq)
				{
					boost::shared_ptr<comm::PkgUserCheckInvitionCodeReply> pPkgUserCheckInvitionCodeReply = boost::make_shared<comm::PkgUserCheckInvitionCodeReply>();
					bSuc = dbSession_->handleMessage_userCheckInvitionCode(pPkgUserCheckInvitionCodeReq, pPkgUserCheckInvitionCodeReply, pPkgSessionUser);
					pRetMsg = pPkgUserCheckInvitionCodeReply;
				}
			}
			else if (name == "comm.PkgUserExitCompanyReq")
			{//�˳���˾
				//�ȵ�¼
				dbSession_->isSessionUserVaildWithException(pPkgSessionUser);

				boost::shared_ptr<comm::PkgUserExitCompanyReq> pPkgUserExitCompanyReq = boost::dynamic_pointer_cast<comm::PkgUserExitCompanyReq>(pMsg);
				if (pPkgUserExitCompanyReq)
				{
					boost::shared_ptr<comm::PkgUserExitCompanyReply> pPkgUserExitCompanyReply = boost::make_shared<comm::PkgUserExitCompanyReply>();
					bSuc = dbSession_->handleMessage_userExitFromCompany(pPkgUserExitCompanyReq, pPkgUserExitCompanyReply, pPkgSessionUser);
					pRetMsg = pPkgUserExitCompanyReply;
				}
			}		

		}
		else if (boost::starts_with(name, "comm.PkgTask"))
		{//�������
			//�ȵ�¼
			dbSession_->isSessionUserVaildWithException(pPkgSessionUser);



			if (name == "comm.PkgTaskPageQueryReq")
			{
				boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReq>(pMsg);
				if (pPkgTaskPageQueryReq)
				{
					boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::make_shared<comm::PkgTaskPageQueryReply>();
					bSuc = dbSession_->handleMessage_taskPageQuery(pPkgTaskPageQueryReq, pPkgTaskPageQueryReply, pPkgSessionUser);
					pRetMsg = pPkgTaskPageQueryReply;
				}

			}
			else if (name == "comm.PkgTaskQueryReq")
			{
				boost::shared_ptr<comm::PkgTaskQueryReq> pPkgTaskQueryReq = boost::dynamic_pointer_cast<comm::PkgTaskQueryReq>(pMsg);
				if (pPkgTaskQueryReq)
				{
					boost::shared_ptr<comm::PkgTaskQueryReply> pPkgTaskQueryReply = boost::make_shared<comm::PkgTaskQueryReply>();
					bSuc = dbSession_->handleMessage_taskQuery(pPkgTaskQueryReq, pPkgTaskQueryReply, pPkgSessionUser);
					pRetMsg = pPkgTaskQueryReply;
				}

			}
			else if (name == "comm.PkgTaskSetSortFlagReq")
			{
				boost::shared_ptr<comm::PkgTaskSetSortFlagReq> pPkgTaskSetOrderReq = boost::dynamic_pointer_cast<comm::PkgTaskSetSortFlagReq>(pMsg);
				if (pPkgTaskSetOrderReq)
				{
					boost::shared_ptr<comm::PkgTaskSetSortFlagReply> pPkgTaskSetOrderReply = boost::make_shared<comm::PkgTaskSetSortFlagReply>();
					bSuc = dbSession_->handleMessage_taskSetSortFlag(pPkgTaskSetOrderReq, pPkgTaskSetOrderReply, pPkgSessionUser);
					pRetMsg = pPkgTaskSetOrderReply;
				}
			}
			else if (name == "comm.PkgTaskOperationReq")
			{
				boost::shared_ptr<comm::PkgTaskOperationReq> pPkgTaskOperationReq = boost::dynamic_pointer_cast<comm::PkgTaskOperationReq>(pMsg);
				if (pPkgTaskOperationReq)
				{
					boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::make_shared<comm::PkgTaskOperationReply>();
					bSuc = dbSession_->handleMessage_taskOperation(pPkgTaskOperationReq, pPkgTaskOperationReply, pPkgSessionUser);
					pRetMsg = pPkgTaskOperationReply;
				}
			}
		}
		else if (boost::starts_with(name, "comm.PkgCompany"))
		{//�û���
			//�ȵ�¼
			dbSession_->isSessionUserVaildWithException(pPkgSessionUser);

			if (pPkgSessionUser->user().companyid() == 0)
			{//���û��Ǹ��˰��û�
				//throw std::exception("nocompany");
				std::string serr = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.nocompany");
				throw nodeexception() << err_no(ERROR_LOGIC) << errtype_str("nocompany") << err_str(serr);

			}

			if (name == "comm.PkgCompanyGroupQueryReq")
			{
				boost::shared_ptr<comm::PkgCompanyGroupQueryReq> pPkgCompanyGroupQueryReq = boost::dynamic_pointer_cast<comm::PkgCompanyGroupQueryReq>(pMsg);
				if (pPkgCompanyGroupQueryReq)
				{
					boost::shared_ptr<comm::PkgCompanyGroupQueryReply> pPkgCompanyGroupQueryReply = boost::make_shared<comm::PkgCompanyGroupQueryReply>();
					bSuc = dbSession_->handleMessage_groupQuery(pPkgCompanyGroupQueryReq, pPkgCompanyGroupQueryReply, pPkgSessionUser);
					pRetMsg = pPkgCompanyGroupQueryReply;
				}
			}
		}
		else if (boost::starts_with(name, "comm.PkgAdv"))
		{//������
			if (name == "comm.PkgAdvQueryReq")
			{
				boost::shared_ptr<comm::PkgAdvQueryReq> pPkgAdvQueryReq = boost::dynamic_pointer_cast<comm::PkgAdvQueryReq>(pMsg);
				if (pPkgAdvQueryReq)
				{
					boost::shared_ptr<comm::PkgAdvQueryReply> pPkgAdvQueryReply = boost::make_shared<comm::PkgAdvQueryReply>();
					bSuc = dbSession_->handleMessage_advQuery(pPkgAdvQueryReq, pPkgAdvQueryReply, pPkgSessionUser);
					pRetMsg = pPkgAdvQueryReply;
				}

			}
		}
		else if (boost::starts_with(name, "comm.PkgQueryMyProfileReq"))
		{
			boost::shared_ptr<comm::PkgQueryMyProfileReq> pPkgQueryMyProfileReq = boost::dynamic_pointer_cast<comm::PkgQueryMyProfileReq>(pMsg);
			if (pPkgQueryMyProfileReq)
			{
				boost::shared_ptr<comm::PkgQueryMyProfileReply> pPkgQueryMyProfileReply = boost::make_shared<comm::PkgQueryMyProfileReply>();
				bSuc = dbSession_->handleMessage_sysQueryMyProfile(pPkgQueryMyProfileReq, pPkgQueryMyProfileReply, pPkgSessionUser);
				pRetMsg = pPkgQueryMyProfileReply;
			}
		}

	}
	catch (soci::soci_error &err)
	{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "(Error),type:" << name << ",errorinfo:" << err.what();
		throw nodeexception() << err_no(ERROR_DB) << errtype_str("dberror") << err_str(err.what());
		//throw std::exception("dberror");
	}
	return pRetMsg;
}




