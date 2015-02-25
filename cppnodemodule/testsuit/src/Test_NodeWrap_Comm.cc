#include <gtest/gtest.h>   

#include <boost/algorithm/hex.hpp>
#include <boost/uuid/uuid.hpp>  
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <json2pb.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <fstream> 
#include <iostream>


#include "../../src/mylib/util/codec.h"
#include "../../src/mylib/util/utillanguage.h"
#include "../../src/gcomm/comm.pb.h"
#include "../../src/DataWrapComm.h"

class Test_NodeWrap_Comm : public testing::Test
{
public:
	virtual void SetUp()
	{
		bUseJson = false;
		std::string sXML = "F:\\jytask\\trunk\\src\\web\\node_modules\\iteasynodewrap\\db\\dbinfo.xml";
		bool bSuc = DataWrapComm::get_mutable_instance().initDB(sXML);
		pPkgSessionUser_ = boost::make_shared<session::PkgSessionUser>();
		pPkgSessionUser_->set_userid(0);
		pPkgSessionUser_->set_applanguage("chinese");
	}
	virtual void TearDown()
	{
	}
protected:
	void savetoFile(const std::string&testCase, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply, const std::string&retData);
	void appendtoFile(const std::string&testCase,const std::string&testMsg, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply, const std::string&retData);
	boost::shared_ptr<google::protobuf::Message> getMsgFromData(const std::string&sData);
	std::string packMsgData(boost::shared_ptr<google::protobuf::Message> pMsg);
	std::string getDataWithSession(const std::string& classname, const std::string& json, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser_);
	bool bUseJson;
	boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser_;

};
std::string Test_NodeWrap_Comm::packMsgData(boost::shared_ptr<google::protobuf::Message> pMsg)
{
	if (bUseJson)
		return pb2json(*pMsg);
	else
	{
		std::string sData;
		pMsg->AppendToString(&sData);
		return boost::algorithm::hex(sData);
	}
}

std::string Test_NodeWrap_Comm::getDataWithSession(const std::string& classname, const std::string& json, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sHexSessionData;
	pPkgSessionUser->AppendToString(&sHexSessionData);
	sHexSessionData = boost::algorithm::hex(sHexSessionData);
	std::string sOutTypeName;
	std::string sOutHexData;
	bool bSuc = DataWrapComm::get_mutable_instance().getDataMsg(classname, json, sHexSessionData, sOutTypeName, sOutHexData);
	boost::shared_ptr<google::protobuf::Message> pUser = DataWrapComm::get_mutable_instance().getMsgFromHex("session.PkgSessionUser", sHexSessionData);
	
	sHexSessionData.clear();
	pUser->AppendToString(&sHexSessionData);
	pPkgSessionUser->ParseFromString(sHexSessionData);

	return sOutTypeName + "|" + sOutHexData;
}
void Test_NodeWrap_Comm::appendtoFile(const std::string&testCase, const std::string&testMsg, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply, const std::string&retData)
{
	std::string sPath = "E:\\jytask\\trunk\\bin\\testdata\\comm\\" + testCase;
	std::ofstream out(sPath,std::ios::app);
	if (out.is_open())
	{
		out << "test case name:" << testMsg << std::endl;
		out << "----------------------------" << std::endl;
		if (pMsgReq)
		{
			out << "req:" << std::endl;
			out << pMsgReq->GetTypeName() << std::endl;
			std::string sHexData, sJsonData;
			pMsgReq->AppendToString(&sHexData);
			sHexData = boost::algorithm::hex(sHexData);
			out << "req(bin):" << std::endl;
			out << sHexData << std::endl;
			sJsonData = pb2json(*pMsgReq);
			out << "req(JSON):" << std::endl;
			out << sJsonData << std::endl;
		}

		out << "----------------------------" << std::endl;
		if (pMsgReply)
		{
			out << "res:" << std::endl;
			out << pMsgReply->GetTypeName() << std::endl;
			std::string sHexData, sJsonData;
			pMsgReply->AppendToString(&sHexData);
			sHexData = boost::algorithm::hex(sHexData);
			out << "res(bin):" << std::endl;
			out << sHexData << std::endl;
			sJsonData = pb2json(*pMsgReply);
			out << "res(JSON):" << std::endl;
			out << sJsonData << std::endl;
		}
		else
		{
			out << "error,data:" << std::endl;
			out << retData << std::endl;
		}
		out << "=====================================" << std::endl;
		out << std::endl;
		out.close();
	}
}

void Test_NodeWrap_Comm::savetoFile(const std::string&testCase, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply, const std::string&retData)
{
	std::string sPath = "E:\\jytask\\trunk\\bin\\testdata\\comm\\" + testCase;
	std::ofstream out(sPath);
	if (out.is_open())
	{
		out << "test case name:" << testCase << std::endl;
		out << "----------------------------" << std::endl;
		if (pMsgReq)
		{
			out << "reqtype:" << std::endl;
			out << pMsgReq->GetTypeName() << std::endl;
			std::string sHexData,sJsonData;
			pMsgReq->AppendToString(&sHexData);
			sHexData = boost::algorithm::hex(sHexData);
			out << "req(bin):" << std::endl;
			out << sHexData << std::endl;
			sJsonData = pb2json(*pMsgReq);
			out << "req(JSON):" << std::endl;
			out << sJsonData << std::endl;
		}

		out << "----------------------------" << std::endl;
		if (pMsgReply)
		{
			out << "res:" << std::endl;
			out << pMsgReply->GetTypeName() << std::endl;
			std::string sHexData, sJsonData;
			pMsgReply->AppendToString(&sHexData);
			sHexData = boost::algorithm::hex(sHexData);
			out << "res(bin):" << std::endl;
			out << sHexData << std::endl;
			sJsonData = pb2json(*pMsgReply);
			out << "res(JSON):" << std::endl;
			out << sJsonData << std::endl;
		}
		else
		{
			out << "error,data:" << std::endl;
			out << retData;
		}
		out.close();
	}
}


boost::shared_ptr<google::protobuf::Message> Test_NodeWrap_Comm::getMsgFromData(const std::string&sData)
{
	boost::shared_ptr<google::protobuf::Message> pMsg;
	size_t n = sData.find("|");
	if (n != std::string::npos)
	{
		std::string sTypeName = sData.substr(0, n);
		std::string sJsonData = sData.substr(n+1, std::string::npos);
		if (bUseJson)
			pMsg = DataWrapComm::get_mutable_instance().getMsgFromJson(sTypeName, sJsonData);
		else
			pMsg = DataWrapComm::get_mutable_instance().getMsgFromHex(sTypeName, sJsonData);
	}
	return pMsg;
}

TEST_F(Test_NodeWrap_Comm, test_json)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	sretdata = "thirdinterface.PkgGetPasswordTableReq|{\"starttime\":null,\"status\":null,\"gettype\":null,\"startid\":0}";
	boost::shared_ptr<google::protobuf::Message> pReqMsg = getMsgFromData(sretdata);
	if (pReqMsg)
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);
	}

}
TEST_F(Test_NodeWrap_Comm, initDBPool)
{
	//int npoolsize = 4;
	//std::string sXML = "E:\\jytask\\trunk\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap\\db\\dbinfo.xml";
	//bool bSuc = DataWrapComm::get_mutable_instance().initDB(sXML);
	//EXPECT_EQ(bSuc, true);

}

TEST_F(Test_NodeWrap_Comm, Test_userLogin)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserLoginReq> pPkgUserLoginReq = boost::make_shared<comm::PkgUserLoginReq>();
	//pPkgUserLoginReq->set_gettype(comm::GT_PHONE);
	//pPkgUserLoginReq->set_phonenumber("18118003691");
	//pPkgUserLoginReq->set_password("1234");
	pPkgUserLoginReq->set_gettype(comm::GT_USERNAME);
	pPkgUserLoginReq->set_username("jwh");
	pPkgUserLoginReq->set_password("111111");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserLoginReq;

	stypename = pReqMsg->GetTypeName();
	sjasondata = packMsgData(pReqMsg);
	sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

	boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
	boost::shared_ptr<comm::PkgUserLoginReply> pPkgUserLoginReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
	EXPECT_EQ(pPkgUserLoginReply!=NULL, true);

	savetoFile("Test_userLogin", pReqMsg, pRetMsg, sretdata);
}

TEST_F(Test_NodeWrap_Comm, Test_userGetAuth)
{//验证账号是否可用,向某手机发送一个验证码
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserGetAuthReq> pPkgUserGetAuthReq = boost::make_shared<comm::PkgUserGetAuthReq>();
	pPkgUserGetAuthReq->set_gettype(comm::GT_PHONE);
	pPkgUserGetAuthReq->set_phonenumber("18011111112");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserGetAuthReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserGetAuthReply> pPkgUserGetAuthReply = boost::make_shared<comm::PkgUserGetAuthReply>();
		EXPECT_EQ(pPkgUserGetAuthReply != NULL, true);

		savetoFile("Test_userGetAuth(phone)", pReqMsg, pRetMsg, sretdata);

	}
	pPkgUserGetAuthReq->set_gettype(comm::GT_EMAIL);
	pPkgUserGetAuthReq->set_emailaddress("25199344@qq.com");
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserGetAuthReply> pPkgUserGetAuthReply = boost::make_shared<comm::PkgUserGetAuthReply>();
		EXPECT_EQ(pPkgUserGetAuthReply != NULL, true);

		savetoFile("Test_userGetAuth(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap_Comm, Test_userCheckInvitionCode)
{//注册时验证邀请码
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserCheckInvitionCodeReq> pPkgUserCheckInvitionCodeReq = boost::make_shared<comm::PkgUserCheckInvitionCodeReq>();
	pPkgUserCheckInvitionCodeReq->set_gettype(comm::GT_PHONE);
	pPkgUserCheckInvitionCodeReq->set_invitioncode("123456");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserCheckInvitionCodeReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCheckInvitionCodeReply> pPkgUserCheckInvitionCodeReply = boost::make_shared<comm::PkgUserCheckInvitionCodeReply>();
		EXPECT_EQ(pPkgUserCheckInvitionCodeReply != NULL, true);

		savetoFile("Test_userCheckInvitionCode(phone)", pReqMsg, pRetMsg, sretdata);

	}
	
}

TEST_F(Test_NodeWrap_Comm, Test_userCheck)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserCheckReq> pPkgUserCheckReq = boost::make_shared<comm::PkgUserCheckReq>();
	pPkgUserCheckReq->set_gettype(comm::GT_PHONE);
	pPkgUserCheckReq->set_phonenumber("18118003690");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserCheckReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCheckReply> pPkgUserCheckReply = boost::dynamic_pointer_cast<comm::PkgUserCheckReply>(pRetMsg);
		EXPECT_EQ(pPkgUserCheckReply != NULL, true);

		savetoFile("Test_userCheck(phone)", pReqMsg, pRetMsg, sretdata);
	}

	pPkgUserCheckReq->set_gettype(comm::GT_EMAIL);
	pPkgUserCheckReq->set_emailaddress("25199344@qq.com");
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCheckReply> pPkgUserCheckReply = boost::dynamic_pointer_cast<comm::PkgUserCheckReply>(pRetMsg);
		EXPECT_EQ(pPkgUserCheckReply != NULL, true);

		savetoFile("Test_userCheck(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap_Comm, Test_userCreate)
{//生成一个用户（该消息会向数据库表中插入一条User记录，密码为验证码或邀请码）
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserCreateReq> pPkgUserCreateReq = boost::make_shared<comm::PkgUserCreateReq>();
	pPkgUserCreateReq->set_gettype(comm::GT_PHONE);
	pPkgUserCreateReq->set_phonenumber("13861214711");
	pPkgUserCreateReq->set_authcode("710968");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserCreateReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCreateReply> pPkgUserCreateReply = boost::dynamic_pointer_cast<comm::PkgUserCreateReply>(pRetMsg);
		EXPECT_EQ(pPkgUserCreateReply != NULL, true);

		savetoFile("Test_userCreate(phone)", pReqMsg, pRetMsg, sretdata);

	}

	pPkgUserCreateReq->set_gettype(comm::GT_EMAIL);
	pPkgUserCreateReq->set_emailaddress("3299@qq.com");
	pPkgUserCreateReq->set_invitioncode("1234567");
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCreateReply> pPkgUserCreateReply = boost::dynamic_pointer_cast<comm::PkgUserCreateReply>(pRetMsg);
		EXPECT_EQ(pPkgUserCreateReply != NULL, true);

		savetoFile("Test_userCreate(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap_Comm, Test_userGetPassword)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserGetPasswordReq> pPkgUserGetPasswordReq = boost::make_shared<comm::PkgUserGetPasswordReq>();
	pPkgUserGetPasswordReq->set_gettype(comm::GT_PHONE);
	pPkgUserGetPasswordReq->set_phonenumber("18118003691");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserGetPasswordReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserGetPasswordReply> pPkgUserGetPasswordReply = boost::dynamic_pointer_cast<comm::PkgUserGetPasswordReply>(pRetMsg);
		EXPECT_EQ(pPkgUserGetPasswordReply!=NULL, true);

		savetoFile("Test_userGetPassword(phone)", pReqMsg, pRetMsg, sretdata);
	}
	pPkgUserGetPasswordReq->set_gettype(comm::GT_EMAIL);
	pPkgUserGetPasswordReq->set_emailaddress("3299@qq.com");
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserGetPasswordReply> pPkgUserGetPasswordReply = boost::dynamic_pointer_cast<comm::PkgUserGetPasswordReply>(pRetMsg);
		EXPECT_EQ(pPkgUserGetPasswordReply != NULL, true);

		savetoFile("Test_userGetPassword(email)", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap_Comm, Test_userResetPassword)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserResetPasswordReq> pPkgUserResetPasswordReq = boost::make_shared<comm::PkgUserResetPasswordReq>();
	pPkgUserResetPasswordReq->set_gettype(comm::GT_PHONE);
	pPkgUserResetPasswordReq->set_phonenumber("18019688849");
	pPkgUserResetPasswordReq->set_authcode("403222");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserResetPasswordReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserResetPasswordReply> pPkgUserResetPasswordReply = boost::dynamic_pointer_cast<comm::PkgUserResetPasswordReply>(pRetMsg);
		EXPECT_EQ(pPkgUserResetPasswordReply != NULL, true);

		savetoFile("Test_userResetPassword(phone)", pReqMsg, pRetMsg, sretdata);

	}

	pPkgUserResetPasswordReq->set_gettype(comm::GT_EMAIL);
	pPkgUserResetPasswordReq->set_phonenumber("aa@qq.com");
	pPkgUserResetPasswordReq->set_authcode("124578");
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserResetPasswordReply> pPkgUserResetPasswordReply = boost::dynamic_pointer_cast<comm::PkgUserResetPasswordReply>(pRetMsg);
		EXPECT_EQ(pPkgUserResetPasswordReply != NULL, true);

		savetoFile("Test_userResetPassword(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap_Comm, Test_userSet)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserSetReq> pPkgUserSetReq = boost::make_shared<comm::PkgUserSetReq>();
	pPkgUserSetReq->set_gettype(comm::GT_PHONE);
	pPkgUserSetReq->set_settype(comm::PkgUserSetReq::ST_NEWPASSWORD | comm::PkgUserSetReq::ST_REALNAME);
	//pPkgUserSetReq->set_userid(3);
	pPkgUserSetReq->set_phonenumber("13861214711");
	pPkgUserSetReq->set_password("710968");
	pPkgUserSetReq->set_newpassword("111111");
	pPkgUserSetReq->set_realname("pPkgUserSetReq");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserSetReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserSetReply> pPkgUserSetReply = boost::dynamic_pointer_cast<comm::PkgUserSetReply>(pRetMsg);
		EXPECT_EQ(pPkgUserSetReply != NULL, true);

		savetoFile("Test_userSet", pReqMsg, pRetMsg, sretdata);

	}

}

TEST_F(Test_NodeWrap_Comm, Test_taskOperation)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	
	int nUserId = 1;

	bool bSuc = false;
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pPkgUserLoginReq = boost::make_shared<comm::PkgUserLoginReq>();
		pPkgUserLoginReq->set_gettype(comm::GT_PHONE);
		pPkgUserLoginReq->set_phonenumber("18118003691");
		pPkgUserLoginReq->set_password("410128");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserLoginReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserLoginReply> pPkgUserLoginReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
		EXPECT_EQ(pPkgUserLoginReply != NULL, true);
	}
	boost::shared_ptr<comm::PkgTaskOperationReq> pPkgTaskOperationReq = boost::make_shared<comm::PkgTaskOperationReq>();
	{
		//插入一个普通任务		
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(0);
		pkgTaskInfo->set_name("s = \"ff:\'yucca!)(;:$&/-[?*\\|\";\"");
		pkgTaskInfo->set_content("Test_NodeWrap_Comm normal task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortflag(1);
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(0);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);
	
		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(insert normal task)", pReqMsg, pRetMsg, sretdata);

	}	

	{
		//插入一个普通任务,附带子任务		
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(0);
		pkgTaskInfo->set_name("Test_NodeWrap_Comm sub task name");
		pkgTaskInfo->set_content("Test_NodeWrap_Comm sub task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortflag(1);
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(0);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(insert normal and sub task)", pReqMsg, pRetMsg, sretdata);

	}

	{
		//插入一个普通任务,附带分配人		
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(0);
		pkgTaskInfo->set_name("Test_NodeWrap_Comm normal task name");
		pkgTaskInfo->set_content("Test_NodeWrap_Comm normal task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortflag(1);
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(0);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(insert normal and sub task)", pReqMsg, pRetMsg, sretdata);

	}

	{	//更新任务信息(不管是普通还是协作任务)
		//	普通或协作任务，taskinfo有效，仅设置普通任务信息（字段包括:任务名、任务内容、时间、排序、紧急度、备注）
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(10);
		pkgTaskInfo->set_name("Test_NodeWrap_Comm update task name");
		pkgTaskInfo->set_content("Test_NodeWrap_Comm update task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortflag(1);
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_UPDATE_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(10);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(insert normal and sub task and assgin)", pReqMsg, pRetMsg, sretdata);
	}

	{
		//更新任务状态
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(1);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_UPDATE_STATUS);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(update status)", pReqMsg, pRetMsg, sretdata);

	}

	{
		//删除任务
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_DELETE_INFO);
		pPkgTaskOperationReq->set_taskid(1);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(delete task)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap_Comm, Test_cppModule){
	std::string sMsgReq = "088D011000";
	boost::shared_ptr<google::protobuf::Message> pReq = DataWrapComm::get_mutable_instance().getMsgFromHex("comm.PkgPhoneCppWrapReq", sMsgReq);
	if (pReq){
		std::string sJsonData = pb2json(*pReq);
		std::cout << sJsonData << std::endl;
	}
}


TEST_F(Test_NodeWrap_Comm, Test_taskSetOrder)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;

	for (int j = 0; j < 2;j++)
	{//排序
		boost::shared_ptr<comm::PkgTaskSetSortFlagReq> pPkgTaskSetOrderReq = boost::make_shared<comm::PkgTaskSetSortFlagReq>();
		for (int i = 1; i < 10; i++)
		{
			comm::PkgTaskSortFlag *pPkgTaskOrder = pPkgTaskSetOrderReq->add_tasksortflaglist();
			pPkgTaskOrder->set_taskid(i);
		
			pPkgTaskOrder->set_sortflag(i%6);

		}

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskSetOrderReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskSetSortFlagReply> pPkgTaskSetOrderReply = boost::dynamic_pointer_cast<comm::PkgTaskSetSortFlagReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskSetOrderReply != NULL, true);

		savetoFile("Test_taskSetOrder", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap_Comm, Test_taskQuery)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;
	int nTaskId = 10;
	{//查询==>根据id查询信息
		boost::shared_ptr<comm::PkgTaskQueryReq> pPkgTaskQueryReq = boost::make_shared<comm::PkgTaskQueryReq>();
		pPkgTaskQueryReq->set_queryflag(comm::PkgTaskQueryReq::QF_TASKINFO);
		pPkgTaskQueryReq->set_taskid(nTaskId);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskQueryReply> pPkgTaskQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskQueryReply != NULL, true);

		savetoFile("Test_taskQuery(todaymytask)", pReqMsg, pRetMsg, sretdata);
	}
}
TEST_F(Test_NodeWrap_Comm, Test_taskUpdateStatus)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;
	std::string sTestPhone = "18118003630";
	//1.	王一登录
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(sTestPhone);
		pReq->set_password("123456");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("Test_taskUpdateStatus", "(user1login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0, true);
		}
	}
	//9.	王一将任务1状态设置为已完成
	{
		//王一将任务1状态设置为已完成
		boost::shared_ptr<comm::PkgTaskOperationReq> pReq = boost::make_shared<comm::PkgTaskOperationReq>();
		pReq->set_taskid(138);
		pReq->set_taskoperation(comm::PkgTaskOperationReq::TO_UPDATE_STATUS);
		pReq->set_updatestatus(comm::PkgTaskInfo::TS_NEW);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		appendtoFile("Test_taskUpdateStatus", "(update task finished)", pReqMsg, pRetMsg, sretdata);
	}

}

TEST_F(Test_NodeWrap_Comm, Test_taskQueryCowork)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;
	std::string sTestPhone = "15961125167";
	std::string sTestEMailAddress = "523947655@qq.com";
	//1.	王一登录
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		//pReq->set_gettype(comm::GT_EMAIL);
		//pReq->set_emailaddress(sTestEMailAddress);
		//pReq->set_password("111111");
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(sTestPhone);
		pReq->set_password("123456");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("Test_taskUpdateStatus", "(user1login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0, true);
		}
	}
	{//查询==>协作任务列表
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskdatestart(UtilTime::getCurrentDate());
		pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_COWORKTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::EnTaskQueryCondition::PkgTaskPageQueryReq_EnTaskQueryCondition_TQC_ONLYRECV);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskQueryCowork(onlyrecv)", pReqMsg, pRetMsg, sretdata);

	}
	{//查询==>协作任务列表
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskdatestart(UtilTime::getCurrentDate());
		pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_COWORKTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::EnTaskQueryCondition::PkgTaskPageQueryReq_EnTaskQueryCondition_TQC_ONLYSEND);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskQueryCowork(onlysend)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap_Comm, Test_sysQueryMyProfile)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	std::string sTestPhone = "18118003630";
	//1.	张二登录
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(sTestPhone);
		pReq->set_password("123456");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("Test_sysQueryMyProfile", "(user2login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0, true);
		}
	}

	//2.	查询自己信息
	{
		boost::shared_ptr<comm::PkgQueryMyProfileReq> pReq = boost::make_shared<comm::PkgQueryMyProfileReq>();
		pReq->set_curversion("1.0.0");
		pReq->set_platmform("ios");	
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgQueryMyProfileReply> pReply = boost::dynamic_pointer_cast<comm::PkgQueryMyProfileReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("Test_sysQueryMyProfile", "(QueryMyProfile)", pReqMsg, pRetMsg, sretdata);

		}
	}
}


TEST_F(Test_NodeWrap_Comm, Test_taskPageQuery)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;
	std::string sTestPhone = "15961125167";
	//1.	王一登录
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(sTestPhone);
		pReq->set_password("123456");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcasetask1", "(user1login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0, true);
		}
	}

	{//查询==>今日任务列表
		std::string strQuery = "{\"pageflag\": 1, \"taskflag\": 1, \"enconditon\": 400, \"pkgtaskquerycondition\": {\"taskstatuslist\": [0], \"enreadedflag\": 3}}";
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		json2pb(*pPkgTaskPageQueryReq, strQuery.data(), strQuery.size());
		//std::string sDate = "2014-10-23";//UtilTime::getCurrentDate();
		//boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		//comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		//pPkgTaskQueryCondition->set_taskdatestart(sDate);
		//pPkgTaskQueryCondition->set_taskdateend(sDate);
		//pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		//pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		//pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		//pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS);
		//pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_NEW);
		//pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_GOING);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(todaymytask)", pReqMsg, pRetMsg, sretdata);

	}



	{//查询==>今日任务列表
		std::string sDate = "2014-10-23";//UtilTime::getCurrentDate();
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskdatestart(sDate);
		pPkgTaskQueryCondition->set_taskdateend(sDate);
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS);
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_NEW);
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_GOING);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);
		
		savetoFile("Test_taskPageQuery(todaymytask)", pReqMsg, pRetMsg, sretdata);

	}

	{//查询==>协作任务列表
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskdatestart(UtilTime::getCurrentDate());
		pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_COWORKTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(todaycowork)", pReqMsg, pRetMsg, sretdata);

	}

	{//查询==>所有任务列表
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(mytask)", pReqMsg, pRetMsg, sretdata);

	}

	{//查询==>已完成任务列表
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(allfinishedmywork)", pReqMsg, pRetMsg, sretdata);
	}

	{//查询==>已完成任务列表+过滤条件(任务名字)
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskQueryCondition->set_keyforname("task");//UTF-8格式
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS | comm::PkgTaskPageQueryReq::TQC_KEYFORNAME);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(allfinishedmyworkwithname)", pReqMsg, pRetMsg, sretdata);
	}

	{//查询==>选择任务:所有自己的任务 + 状态非 TS_FINISHED 的任务
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKNOTSTATUS);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(allnotfinishedmywork)", pReqMsg, pRetMsg, sretdata);

	}
}


TEST_F(Test_NodeWrap_Comm, Test_groupQuery)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;
	boost::shared_ptr<comm::PkgCompanyGroupQueryReq> pPkgCompanyGroupQueryReq = boost::make_shared<comm::PkgCompanyGroupQueryReq>();
	pPkgCompanyGroupQueryReq->set_querytype(comm::PkgCompanyGroupQueryReq::QT_ALL);
	
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgCompanyGroupQueryReq;

	stypename = pReqMsg->GetTypeName();
	sjasondata = packMsgData(pReqMsg);
	sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

	boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
	boost::shared_ptr<comm::PkgCompanyGroupQueryReply> pPkgCompanyGroupQueryReply = boost::dynamic_pointer_cast<comm::PkgCompanyGroupQueryReply>(pRetMsg);
	EXPECT_EQ(pPkgCompanyGroupQueryReply != NULL, true);

	savetoFile("Test_groupQuery", pReqMsg, pRetMsg, sretdata);
}


TEST_F(Test_NodeWrap_Comm, Test_advQuery)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgAdvQueryReq> pPkgAdvQueryReq = boost::make_shared<comm::PkgAdvQueryReq>();
	pPkgAdvQueryReq->set_advid(1);
	pPkgAdvQueryReq->set_numreq(3);
	std::map<std::string, int> mapQuery;
	mapQuery["QueryNumValidTime"] = comm::PkgAdvQueryReq::QT_SPNUM | comm::PkgAdvQueryReq::QT_VAILDTIME;
	mapQuery["QueryId"] = comm::PkgAdvQueryReq::QT_ADVID;

	for (std::map<std::string, int>::iterator iterMap = mapQuery.begin(); iterMap != mapQuery.end(); iterMap++)
	{
		const std::string&sKey = (*iterMap).first;
		int nQueryFlag = (*iterMap).second;

		pPkgAdvQueryReq->set_queryflag(nQueryFlag);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgAdvQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgAdvQueryReply> pPkgAdvQueryReply = boost::dynamic_pointer_cast<comm::PkgAdvQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvQueryReply != NULL, true);

		savetoFile("Test_advQuery(" + sKey + ")", pReqMsg, pRetMsg, sretdata);
	}

}

TEST_F(Test_NodeWrap_Comm, testcase1)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	std::string sTestPhone = "18118003630";
	std::string authcode;
	int userid = 0;
//1.	某人(18118003630) 注册一个用户（用手机）
	{
		{//判断18118003630是否存在
			boost::shared_ptr<comm::PkgUserGetAuthReq> pReq = boost::make_shared<comm::PkgUserGetAuthReq>();
			pReq->set_gettype(comm::GT_PHONE);
			pReq->set_phonenumber(sTestPhone);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserGetAuthReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserGetAuthReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase1","(isphonenumberexist)", pReqMsg, pRetMsg, sretdata);
			}
		}

		{//向该手机18118003630发送一个验证码
			boost::shared_ptr<comm::PkgUserGetAuthReq> pReq = boost::make_shared<comm::PkgUserGetAuthReq>();
			pReq->set_gettype(comm::GT_PHONE);
			pReq->set_phonenumber(sTestPhone);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserGetAuthReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserGetAuthReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase1", "(sendauthcodetophone)", pReqMsg, pRetMsg, sretdata);
			}
		}

		std::cout << "input code:" << std::endl;
		std::cin >> authcode;
		{//发送注册请求，在数据库中新增一条记录
			boost::shared_ptr<comm::PkgUserCreateReq> pReq = boost::make_shared<comm::PkgUserCreateReq>();
			pReq->set_gettype(comm::GT_PHONE);
			pReq->set_phonenumber(sTestPhone);
			pReq->set_authcode(authcode);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserCreateReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserCreateReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase1", "(createuserwithauthcode)", pReqMsg, pRetMsg, sretdata);

				userid = pReply->userid();
			}
		}

	}

	//2.	将名字和密码修改为王一和123456
	{
		{
			boost::shared_ptr<comm::PkgUserSetReq> pReq = boost::make_shared<comm::PkgUserSetReq>();
			pReq->set_gettype(comm::GT_USERID);
			pReq->set_userid(userid);
			pReq->set_settype(comm::PkgUserSetReq::ST_NEWPASSWORD | comm::PkgUserSetReq::ST_REALNAME);
			pReq->set_realname("w1");
			pReq->set_password(authcode);
			pReq->set_newpassword("123456");
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserSetReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserSetReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase1", "(setnameandpassword)", pReqMsg, pRetMsg, sretdata);
			}
		}
	}
	//3.	用账号18118003630和密码123456登录
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		pReq->set_gettype(comm::GT_USERID);
		pReq->set_userid(userid);
		pReq->set_password("123456");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase1", "(login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0,true);
		}

	}
	//4.	王一(18118003630) 新建一个公司名为:简易网络
	{
		boost::shared_ptr<comm::PkgUserCreateCompanyReq> pReq = boost::make_shared<comm::PkgUserCreateCompanyReq>();
		pReq->set_companyname("jy");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserCreateCompanyReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserCreateCompanyReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase1", "(createcompany)", pReqMsg, pRetMsg, sretdata);
		}
	}
	//5.	王一邀请张二(15922241100)加入
	std::string testPhone2 = "15922241100";
	{
		boost::shared_ptr<comm::PkgUserInvitionUserReq> pReq = boost::make_shared<comm::PkgUserInvitionUserReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(testPhone2);
		pReq->set_realname("z2");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserInvitionUserReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserInvitionUserReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase1", "(invitefriendin)", pReqMsg, pRetMsg, sretdata);
		}
	}
	//5.	张二手机获得邀请码
	//6.	张二通过邀请码加入公司:简易网络
	std::string sInvitionCode;
	{
		std::cout << "input code:" << std::endl;
		std::cin >> sInvitionCode;
		{//发送注册请求，在数据库中新增一条记录
			boost::shared_ptr<comm::PkgUserCreateReq> pReq = boost::make_shared<comm::PkgUserCreateReq>();
			pReq->set_gettype(comm::GT_PHONE);
			pReq->set_phonenumber(testPhone2);
			pReq->set_invitioncode(sInvitionCode);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserCreateReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserCreateReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase1", "(createuserwithinvitioncode)", pReqMsg, pRetMsg, sretdata);

				userid = pReply->userid();
			}
		}

	}

	//7.	张二修改密码为222222
	{
		boost::shared_ptr<comm::PkgUserSetReq> pReq = boost::make_shared<comm::PkgUserSetReq>();
		pReq->set_gettype(comm::GT_USERID);
		pReq->set_userid(userid);
		pReq->set_settype(comm::PkgUserSetReq::ST_NEWPASSWORD | comm::PkgUserSetReq::ST_REALNAME);
		pReq->set_realname("z2");
		pReq->set_password(sInvitionCode);
		pReq->set_newpassword("222222");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserSetReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserSetReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase1", "(setnameandpassword)", pReqMsg, pRetMsg, sretdata);
		}
	}
	//8.	张二忘记密码，通过密码找回功能找回
	std::string authcode2;
	{
		boost::shared_ptr<comm::PkgUserGetPasswordReq> pReq = boost::make_shared<comm::PkgUserGetPasswordReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(testPhone2);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserGetPasswordReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserGetPasswordReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase1", "(findpasswordback)", pReqMsg, pRetMsg, sretdata);

			
		}
		
	}
//9.	张二通过验证码重置密码为111111
	std::cout << "input code:" << std::endl;
	std::cin >> authcode2;
	std::string oldpassword;
	{
		boost::shared_ptr<comm::PkgUserResetPasswordReq> pReq = boost::make_shared<comm::PkgUserResetPasswordReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(testPhone2);
		pReq->set_authcode(authcode2);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserResetPasswordReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserResetPasswordReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase1", "(getoldpassword)", pReqMsg, pRetMsg, sretdata);

			oldpassword = pReply->oldpassword();
		}
	}

	//10.	张二通过修改密码为111111
	{
		boost::shared_ptr<comm::PkgUserSetReq> pReq = boost::make_shared<comm::PkgUserSetReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(testPhone2);
		pReq->set_settype(comm::PkgUserSetReq::ST_NEWPASSWORD);
		pReq->set_password(oldpassword);
		pReq->set_newpassword("111111");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserSetReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserSetReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase1", "(setnameandpassword)", pReqMsg, pRetMsg, sretdata);
		}
	}

}

TEST_F(Test_NodeWrap_Comm, testcase2)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

//1.	某人wanger@aa.com注册一个用户(用Email)
	std::string sTestEmail = "wanger@aa.com";
	{
		{//判断wanger@aa.com是否存在
			boost::shared_ptr<comm::PkgUserGetAuthReq> pReq = boost::make_shared<comm::PkgUserGetAuthReq>();
			pReq->set_gettype(comm::GT_EMAIL);
			pReq->set_emailaddress(sTestEmail);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserGetAuthReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserGetAuthReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase2", "(isemailaddressexist)", pReqMsg, pRetMsg, sretdata);
			}
		}

		{//向该Email:wanger@aa.com发送一个超链接(超链接中含有验证码,例:http://www.iteasysoft.com/confirm?authcode=224472&email=wanger@aa.com
			boost::shared_ptr<comm::PkgUserGetAuthReq> pReq = boost::make_shared<comm::PkgUserGetAuthReq>();
			pReq->set_gettype(comm::GT_EMAIL);
			pReq->set_emailaddress(sTestEmail);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserGetAuthReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserGetAuthReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase2", "(sendauthcodetoemail)", pReqMsg, pRetMsg, sretdata);
			}
		}
	}
	std::string authcode;
	int userid = 0;
	{//用户打开邮箱点击超链接以后
		//注:后台需处理confirm页面，获取验证码和EMail地址
		//后台处理逻辑，先获得验证码
		std::cout << "input code:" << std::endl;
		std::cin >> authcode;
		{//发送注册请求，在数据库中新增一条记录
			boost::shared_ptr<comm::PkgUserCreateReq> pReq = boost::make_shared<comm::PkgUserCreateReq>();
			pReq->set_gettype(comm::GT_EMAIL);
			pReq->set_emailaddress(sTestEmail);
			pReq->set_authcode(authcode);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserCreateReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserCreateReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase2", "(createuserwithauthcode)", pReqMsg, pRetMsg, sretdata);

				userid = pReply->userid();
			}
		}
	}
	//2.	将名字修改为王二和123456
	{
		{
			boost::shared_ptr<comm::PkgUserSetReq> pReq = boost::make_shared<comm::PkgUserSetReq>();
			pReq->set_gettype(comm::GT_USERID);
			pReq->set_userid(userid);
			pReq->set_settype(comm::PkgUserSetReq::ST_NEWPASSWORD | comm::PkgUserSetReq::ST_REALNAME);
			pReq->set_realname("w2");
			pReq->set_password(authcode);
			pReq->set_newpassword("123456");
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserSetReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserSetReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase2", "(setnameandpassword)", pReqMsg, pRetMsg, sretdata);
			}
		}
	}
	//3.	王一邀请王二加入
	{
		//王一登录
		boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser1 = boost::make_shared<session::PkgSessionUser>();
		{
			boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
			pReq->set_gettype(comm::GT_PHONE);
			pReq->set_phonenumber("18118003630");
			pReq->set_password("123456");
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser1);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase2", "(user1login)", pReqMsg, pRetMsg, sretdata);

				EXPECT_EQ(pPkgSessionUser1->userid() > 0, true);
			}
		}
		{
			//王一发邀请
			boost::shared_ptr<comm::PkgUserInvitionUserReq> pReq = boost::make_shared<comm::PkgUserInvitionUserReq>();
			pReq->set_gettype(comm::GT_EMAIL);
			pReq->set_emailaddress(sTestEmail);
			pReq->set_realname("w23");
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
			{
				stypename = pReqMsg->GetTypeName();
				sjasondata = packMsgData(pReqMsg);
				sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser1);

				boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
				boost::shared_ptr<comm::PkgUserInvitionUserReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserInvitionUserReply>(pRetMsg);
				EXPECT_EQ(pReply != NULL, true);
				appendtoFile("testcase2", "(user1invitemein)", pReqMsg, pRetMsg, sretdata);
			}
		}

	}

	{
		//王二登录
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		pReq->set_gettype(comm::GT_EMAIL);
		pReq->set_emailaddress(sTestEmail);
		pReq->set_password("123456");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase2", "(user2login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0, true);
		}

	}
//4.	王二EMail获得邀请码
	std::string sInvitionCode;
	{
		std::cout << "input code:" << std::endl;
		std::cin >> sInvitionCode;
	}
	//5.	王二通过邀请码加入公司:简易网络
	{//已登录用户应在"我”界面中输入邀请码
		boost::shared_ptr<comm::PkgUserInvitionCheckReq> pReq = boost::make_shared<comm::PkgUserInvitionCheckReq>();
		pReq->set_invitioncode(sInvitionCode);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserInvitionCheckReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserInvitionCheckReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase2", "(addcompanywithinvitecode)", pReqMsg, pRetMsg, sretdata);
		}
	}
	//6.	王二退出公司简易网络
	{
		boost::shared_ptr<comm::PkgUserExitCompanyReq> pReq = boost::make_shared<comm::PkgUserExitCompanyReq>();
		pReq->set_encmd(comm::PkgUserExitCompanyReq::CMD_EXIT);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserExitCompanyReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserExitCompanyReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase2", "(exitcompany)", pReqMsg, pRetMsg, sretdata);
		}

	}
	//7.	王二创建公司名为:复杂网络
	{
		boost::shared_ptr<comm::PkgUserCreateCompanyReq> pReq = boost::make_shared<comm::PkgUserCreateCompanyReq>();
		pReq->set_companyname("cnet");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserCreateCompanyReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserCreateCompanyReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase2", "(createcompany)", pReqMsg, pRetMsg, sretdata);
		}
	}
//8.	王二修改密码为654321
	{
		boost::shared_ptr<comm::PkgUserSetReq> pReq = boost::make_shared<comm::PkgUserSetReq>();
		pReq->set_gettype(comm::GT_EMAIL);
		pReq->set_emailaddress(sTestEmail);
		pReq->set_settype(comm::PkgUserSetReq::ST_NEWPASSWORD);
		pReq->set_password("123456");
		pReq->set_newpassword("654321");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserSetReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserSetReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcase2", "(setnameandpassword)", pReqMsg, pRetMsg, sretdata);
		}
	}

	getchar();
}

TEST_F(Test_NodeWrap_Comm, testcasetask1)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	std::string sTestPhone = "18118003630";
	//1.	王一登录
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(sTestPhone);
		pReq->set_password("123456");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcasetask1", "(user1login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0, true);
		}
	}


	//2.	王一新建一个任务1命名为”今天出差”
	int taskid1 = 0;
	{
		boost::shared_ptr<comm::PkgTaskOperationReq> pReq = boost::make_shared<comm::PkgTaskOperationReq>();
		{
			//插入一个普通任务		
			comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
			pkgTaskInfo->set_id(0);
			pkgTaskInfo->set_name("test");
			pkgTaskInfo->set_content("testttxxx");
			pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
			pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
			pkgTaskInfo->set_sortflag(1);
			pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
			pReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
			pReq->set_allocated_taskinfo(pkgTaskInfo);
			pReq->set_taskid(0);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgTaskOperationReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);

			appendtoFile("testcasetask1","(insert task)", pReqMsg, pRetMsg, sretdata);

			taskid1 = pReply->taskid();
		}
	}

	//3.	王一将任务1重命名为”下午13点去客户1”
	{
		//更新任务信息(不管是普通还是协作任务)
		boost::shared_ptr<comm::PkgTaskOperationReq> pReq = boost::make_shared<comm::PkgTaskOperationReq>();
		//	普通或协作任务，taskinfo有效，仅设置普通任务信息（字段包括:任务名、任务内容、时间、排序、紧急度、备注）
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(taskid1);
		pkgTaskInfo->set_name("xxxxxxx");
		pkgTaskInfo->set_content("bbbbbbbbb");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortflag(1);
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pReq->set_taskoperation(comm::PkgTaskOperationReq::TO_UPDATE_INFO);
		pReq->set_allocated_taskinfo(pkgTaskInfo);
		pReq->set_taskid(taskid1);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		appendtoFile("testcasetask1","(update task info)", pReqMsg, pRetMsg, sretdata);

	}
	//4.	王一新建一个任务2命名为”今天预约客户2”
	int taskid2 = 0;
	{
		boost::shared_ptr<comm::PkgTaskOperationReq> pReq = boost::make_shared<comm::PkgTaskOperationReq>();
		{
			//插入一个普通任务		
			comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
			pkgTaskInfo->set_id(0);
			pkgTaskInfo->set_name("aaa");
			pkgTaskInfo->set_content("bbbbbbbbbbbb");
			pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
			pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
			pkgTaskInfo->set_sortflag(1);
			pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
			pReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
			pReq->set_allocated_taskinfo(pkgTaskInfo);
			pReq->set_taskid(0);
			boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgTaskOperationReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);

			appendtoFile("testcasetask1","(insert task)", pReqMsg, pRetMsg, sretdata);

			taskid2 = pReply->taskid();
		}
	}

	//5.	王一将任务2顺序调到最前
	{
		boost::shared_ptr<comm::PkgTaskSetSortFlagReq> pReq = boost::make_shared<comm::PkgTaskSetSortFlagReq>();
		{
			comm::PkgTaskSortFlag *pPkgTaskOrder1 = pReq->add_tasksortflaglist();
			pPkgTaskOrder1->set_taskid(taskid1);
			std::string sOrder = "2";
			pPkgTaskOrder1->set_sortflag(2);

			comm::PkgTaskSortFlag *pPkgTaskOrder2 = pReq->add_tasksortflaglist();
			pPkgTaskOrder2->set_taskid(taskid2);
			sOrder = "1";
			pPkgTaskOrder2->set_sortflag(1);
		}
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskSetSortFlagReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskSetSortFlagReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		appendtoFile("testcasetask1","(taskSetSortFlag)", pReqMsg, pRetMsg, sretdata);
	}
	//6.	王一获取今日任务列表
	{
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskdatestart(UtilTime::getCurrentDate());
		pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
		pReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		appendtoFile("testcasetask1", "(querytodaymytask)", pReqMsg, pRetMsg, sretdata);

	}
	//7.	王一点击分配任务获得人员列表
	int userid = 0;
	std::string realname;
	{
		boost::shared_ptr<comm::PkgCompanyGroupQueryReq> pReq = boost::make_shared<comm::PkgCompanyGroupQueryReq>();
		pReq->set_querytype(comm::PkgCompanyGroupQueryReq::QT_ALL);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgCompanyGroupQueryReply> pReply = boost::dynamic_pointer_cast<comm::PkgCompanyGroupQueryReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		for (int i = 0; i < pReply->groupuserlist().size(); i++)
		{
			const comm::PkgGroupUser&user = pReply->groupuserlist().Get(i);
			if (user.userid() != pPkgSessionUser_->userid())
			{
				userid = user.userid();
				realname = user.realname();
			}
		}
		appendtoFile("testcasetask1", "(queryusers)", pReqMsg, pRetMsg, sretdata);

	}
	//8.	王一发任务2给人员中的张二
	{
		//分配普通任务给其他人
		boost::shared_ptr<comm::PkgTaskOperationReq> pReq = boost::make_shared<comm::PkgTaskOperationReq>();
		pReq->set_taskid(taskid2);
		pReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO_COWORK);
		pReq->add_accepteduseridlist(userid);

		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(taskid2);
		pkgTaskInfo->set_name("dddd");
		pkgTaskInfo->set_content("xxxxxxxxxxxxxx");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortflag(2);
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pReq->set_allocated_taskinfo(pkgTaskInfo);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		appendtoFile("testcasetask1","(assgin task)", pReqMsg, pRetMsg, sretdata);
	}

	//9.	王一将任务1状态设置为已完成
	{
		//王一将任务1状态设置为已完成
		boost::shared_ptr<comm::PkgTaskOperationReq> pReq = boost::make_shared<comm::PkgTaskOperationReq>();
		pReq->set_taskid(taskid1);
		pReq->set_taskoperation(comm::PkgTaskOperationReq::TO_UPDATE_STATUS);
		pReq->set_updatestatus(comm::PkgTaskInfo::TS_FINISHED);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		appendtoFile("testcasetask1", "(update task finished)", pReqMsg, pRetMsg, sretdata);
	}

}

TEST_F(Test_NodeWrap_Comm, testcasetask2)
{
//1.	张二登录
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	std::string sTestPhone = "18118003630";
	//1.	张二登录
	{
		boost::shared_ptr<comm::PkgUserLoginReq> pReq = boost::make_shared<comm::PkgUserLoginReq>();
		pReq->set_gettype(comm::GT_PHONE);
		pReq->set_phonenumber(sTestPhone);
		pReq->set_password("654321");
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;
		{
			stypename = pReqMsg->GetTypeName();
			sjasondata = packMsgData(pReqMsg);
			sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

			boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
			boost::shared_ptr<comm::PkgUserLoginReply> pReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
			EXPECT_EQ(pReply != NULL, true);
			appendtoFile("testcasetask2", "(user2login)", pReqMsg, pRetMsg, sretdata);

			EXPECT_EQ(pPkgSessionUser_->userid() > 0, true);
		}
	}

	//2.	张二获得今日任务列表
	{
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskdatestart("2014-09-12");// UtilTime::getCurrentDate());
		pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
		pReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pReply != NULL, true);

		appendtoFile("testcasetask1", "(querytodaymytask)", pReqMsg, pRetMsg, sretdata);

	}
	//3.	张二上翻一页获得今日任务列表
	{

	}
	//4.	张二下翻一页获得今日任务列表
	{

	}
	//5.	王一获得协作任务列表
	/*{
	boost::shared_ptr<comm::PkgTaskPageQueryReq> pReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
	comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
	//pPkgTaskQueryCondition->set_taskdatestart(UtilTime::getCurrentDate());
	//pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
	pReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
	pReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
	pReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_COWORKTASK);
	//pReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND);
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

	stypename = pReqMsg->GetTypeName();
	sjasondata = packMsgData(pReqMsg);
	sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

	boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
	boost::shared_ptr<comm::PkgTaskPageQueryReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
	EXPECT_EQ(pReply != NULL, true);

	appendtoFile("testcasetask1", "(querycoworktask)", pReqMsg, pRetMsg, sretdata);

	}
	//6.	王一获得所有任务列表
	{
	boost::shared_ptr<comm::PkgTaskPageQueryReq> pReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
	comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
	//pPkgTaskQueryCondition->set_taskdatestart(UtilTime::getCurrentDate());
	//pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
	pReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
	pReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
	pReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
	//pReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND);
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pReq;

	stypename = pReqMsg->GetTypeName();
	sjasondata = packMsgData(pReqMsg);
	sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser_);

	boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
	boost::shared_ptr<comm::PkgTaskPageQueryReply> pReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
	EXPECT_EQ(pReply != NULL, true);

	appendtoFile("testcasetask1", "(queryalltask)", pReqMsg, pRetMsg, sretdata);

	}*/
//7.	张二点击协作任务的”今日预约客户2”，显示详情
//8.	张二点击接受
//9.	张二切换到今日任务列表(显示今日预约客户2)

}