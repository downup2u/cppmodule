#include <gtest/gtest.h>   
#include "../gcomm/comm.pb.h"
#include "../gcomm/admin.pb.h"
#include "../gcomm/thirdinterface.pb.h"
#include "../../nodecpp/include/DataWrap.h"
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
#include "../../mylib/include/util/UtilString.h"
#include "../../mylib\include\util\Codec.h"
#include "../../mylib/include/util/utillanguage.h"

class Test_NodeWrap : public testing::Test
{
public:
	virtual void SetUp()
	{
		int npoolsize = 4;
		bUseJson = false;
		std::string sconnectionstring = "db=task host=192.168.1.199 user=root password=itadmin charset=utf8";
		bool bSuc = DataWrap::get_mutable_instance().initDBPool(npoolsize, sconnectionstring);
		pPkgSessionUser = boost::make_shared<session::PkgSessionUser>();
		pPkgSessionUser->set_userid(0);

		std::string hex = "08012207656e676c697368520431323334";

		boost::shared_ptr<google::protobuf::Message> pMsgSession = DataWrap::get_mutable_instance().getMsgFromHex("session.PkgSessionUser", hex);
		boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser2;
		if (pMsgSession)
		{
			pPkgSessionUser2 = boost::dynamic_pointer_cast<session::PkgSessionUser>(pMsgSession);
		}

		if (pPkgSessionUser2)
		{
			std::cout << "test" << std::endl;
		}
		//boost::shared_ptr<google::protobuf::Message> pMsg = CodecUtil::createMsgFromTypeName("session.PkgSessionUser");
		//pMsg->AppendToString(&hex);
		//08012207656e676c697368520431323334
	}
	virtual void TearDown()
	{
	}
protected:
	void savetoFile(const std::string&testCase, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply,const std::string&retData);
	boost::shared_ptr<google::protobuf::Message> getMsgFromData(const std::string&sData);
	std::string packMsgData(boost::shared_ptr<google::protobuf::Message> pMsg);
	std::string getDataWithSession(const std::string& classname, const std::string& json, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool bUseJson;
	boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser;

};
std::string Test_NodeWrap::packMsgData(boost::shared_ptr<google::protobuf::Message> pMsg)
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

std::string Test_NodeWrap::getDataWithSession(const std::string& classname, const std::string& json, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sHexSessionData;
	pPkgSessionUser->AppendToString(&sHexSessionData);
	sHexSessionData = boost::algorithm::hex(sHexSessionData);
	std::string sOutTypeName;
	std::string sOutHexData;
	bool bSuc = DataWrap::get_mutable_instance().getDataMsg(classname, json, sHexSessionData, sOutTypeName, sOutHexData);
	return sOutTypeName + "|" + sOutHexData;
}

void Test_NodeWrap::savetoFile(const std::string&testCase, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply, const std::string&retData)
{
	std::string sPath = "E:\\jytask\\trunk\\bin\\testdata\\" + testCase;
	std::ofstream out(sPath);
	if (out.is_open())
	{
		out << "测试用例名:" << testCase << std::endl;
		out << "----------------------------" << std::endl;
		if (pMsgReq)
		{
			out << "请求类型:" << std::endl;
			out << pMsgReq->GetTypeName() << std::endl;
			std::string sHexData,sJsonData;
			pMsgReq->AppendToString(&sHexData);
			sHexData = boost::algorithm::hex(sHexData);
			out << "请求数据(二进制):" << std::endl;
			out << sHexData << std::endl;
			sJsonData = pb2json(*pMsgReq);
			out << "请求数据(JSON):" << std::endl;
			out << sJsonData << std::endl;
		}

		out << "----------------------------" << std::endl;
		if (pMsgReply)
		{
			out << "回复类型:" << std::endl;
			out << pMsgReply->GetTypeName() << std::endl;
			std::string sHexData, sJsonData;
			pMsgReply->AppendToString(&sHexData);
			sHexData = boost::algorithm::hex(sHexData);
			out << "回复数据(二进制):" << std::endl;
			out << sHexData << std::endl;
			sJsonData = pb2json(*pMsgReply);
			out << "回复数据(JSON):" << std::endl;
			out << sJsonData << std::endl;
		}
		else
		{
			out << "无法解析为回复格式,数据为:" << std::endl;
			out << retData;
		}
		out.close();
	}
}


boost::shared_ptr<google::protobuf::Message> Test_NodeWrap::getMsgFromData(const std::string&sData)
{
	boost::shared_ptr<google::protobuf::Message> pMsg;
	size_t n = sData.find("|");
	if (n != std::string::npos)
	{
		std::string sTypeName = sData.substr(0, n);
		std::string sJsonData = sData.substr(n+1, std::string::npos);
		if (bUseJson)
			pMsg = DataWrap::get_mutable_instance().getMsgFromJson(sTypeName, sJsonData);
		else
			pMsg = DataWrap::get_mutable_instance().getMsgFromHex(sTypeName, sJsonData);
	}
	return pMsg;
}

TEST_F(Test_NodeWrap, test_json)
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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);
	}

}
TEST_F(Test_NodeWrap, initDBPool)
{
	int npoolsize = 4;
	std::string sconnectionstring = "db=task host=192.168.1.199 user=root password=itadmin  charset=utf8";
	bool bSuc = DataWrap::get_mutable_instance().initDBPool(npoolsize, sconnectionstring);
	EXPECT_EQ(bSuc, true);

}

TEST_F(Test_NodeWrap, Test_userLogin)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserLoginReq> pPkgUserLoginReq = boost::make_shared<comm::PkgUserLoginReq>();
	pPkgUserLoginReq->set_gettype(comm::GT_PHONE);
	pPkgUserLoginReq->set_phonenumber("18118003691");
	pPkgUserLoginReq->set_password("1234");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserLoginReq;

	stypename = pReqMsg->GetTypeName();
	sjasondata = packMsgData(pReqMsg);
	sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

	boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
	boost::shared_ptr<comm::PkgUserLoginReply> pPkgUserLoginReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
	EXPECT_EQ(pPkgUserLoginReply!=NULL, true);

	savetoFile("Test_userLogin", pReqMsg, pRetMsg, sretdata);
}

TEST_F(Test_NodeWrap, Test_userGetAuth)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserGetAuthReq> pPkgUserGetAuthReq = boost::make_shared<comm::PkgUserGetAuthReq>();
	pPkgUserGetAuthReq->set_gettype(comm::GT_PHONE);
	pPkgUserGetAuthReq->set_phonenumber("18118003690");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserGetAuthReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserGetAuthReply> pPkgUserGetAuthReply = boost::make_shared<comm::PkgUserGetAuthReply>();
		EXPECT_EQ(pPkgUserGetAuthReply != NULL, true);

		savetoFile("Test_userGetAuth(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap, Test_userCheck)
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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCheckReply> pPkgUserCheckReply = boost::dynamic_pointer_cast<comm::PkgUserCheckReply>(pRetMsg);
		EXPECT_EQ(pPkgUserCheckReply != NULL, true);

		savetoFile("Test_userCheck(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap, Test_userCreate)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserCreateReq> pPkgUserCreateReq = boost::make_shared<comm::PkgUserCreateReq>();
	pPkgUserCreateReq->set_gettype(comm::GT_PHONE);
	pPkgUserCreateReq->set_phonenumber("18118003690");
	pPkgUserCreateReq->set_authcode("123456");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserCreateReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCreateReply> pPkgUserCreateReply = boost::dynamic_pointer_cast<comm::PkgUserCreateReply>(pRetMsg);
		EXPECT_EQ(pPkgUserCreateReply != NULL, true);

		savetoFile("Test_userCreate(phone)", pReqMsg, pRetMsg, sretdata);

	}

	pPkgUserCreateReq->set_gettype(comm::GT_EMAIL);
	pPkgUserCreateReq->set_emailaddress("3299@qq.com");
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserCreateReply> pPkgUserCreateReply = boost::dynamic_pointer_cast<comm::PkgUserCreateReply>(pRetMsg);
		EXPECT_EQ(pPkgUserCreateReply != NULL, true);

		savetoFile("Test_userCreate(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap, Test_userGetPassword)
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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserGetPasswordReply> pPkgUserGetPasswordReply = boost::dynamic_pointer_cast<comm::PkgUserGetPasswordReply>(pRetMsg);
		EXPECT_EQ(pPkgUserGetPasswordReply != NULL, true);

		savetoFile("Test_userGetPassword(email)", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap, Test_userResetPassword)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserResetPasswordReq> pPkgUserResetPasswordReq = boost::make_shared<comm::PkgUserResetPasswordReq>();
	pPkgUserResetPasswordReq->set_gettype(comm::GT_PHONE);
	pPkgUserResetPasswordReq->set_phonenumber("18118003691");
	pPkgUserResetPasswordReq->set_authcode("124578");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserResetPasswordReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserResetPasswordReply> pPkgUserResetPasswordReply = boost::dynamic_pointer_cast<comm::PkgUserResetPasswordReply>(pRetMsg);
		EXPECT_EQ(pPkgUserResetPasswordReply != NULL, true);

		savetoFile("Test_userResetPassword(email)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap, Test_userSet)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserSetReq> pPkgUserSetReq = boost::make_shared<comm::PkgUserSetReq>();
	pPkgUserSetReq->set_settype(comm::PkgUserSetReq::ST_NEWPASSWORD);
	pPkgUserSetReq->set_userid(3);
	pPkgUserSetReq->set_password("123456");
	pPkgUserSetReq->set_newpassword("654321");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserSetReq;
	{
		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgUserSetReply> pPkgUserSetReply = boost::dynamic_pointer_cast<comm::PkgUserSetReply>(pRetMsg);
		EXPECT_EQ(pPkgUserSetReply != NULL, true);

		savetoFile("Test_userSet", pReqMsg, pRetMsg, sretdata);

	}

}

TEST_F(Test_NodeWrap, Test_taskOperation)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	
	int nUserId = 1;

	bool bSuc = false;
	boost::shared_ptr<comm::PkgTaskOperationReq> pPkgTaskOperationReq = boost::make_shared<comm::PkgTaskOperationReq>();
	{
		//插入一个普通任务		
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(0);
		pkgTaskInfo->set_name("Test_NodeWrap normal task name");
		pkgTaskInfo->set_content("Test_NodeWrap normal task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortorder("002");
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(0);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);
	
		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(insert normal task)", pReqMsg, pRetMsg, sretdata);

	}	

	{
		//插入一个普通任务,附带子任务		
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(0);
		pkgTaskInfo->set_name("Test_NodeWrap sub task name");
		pkgTaskInfo->set_content("Test_NodeWrap sub task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortorder("002");
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(0);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(insert normal and sub task)", pReqMsg, pRetMsg, sretdata);

	}

	{
		//插入一个普通任务,附带分配人		
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(0);
		pkgTaskInfo->set_name("Test_NodeWrap normal task name");
		pkgTaskInfo->set_content("Test_NodeWrap normal task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortorder("002");
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_INSERT_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(0);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(insert normal and sub task)", pReqMsg, pRetMsg, sretdata);

	}

	{	//更新任务信息(不管是普通还是协作任务)
		//	普通或协作任务，taskinfo有效，仅设置普通任务信息（字段包括：任务名、任务内容、时间、排序、紧急度、备注）
		comm::PkgTaskInfo* pkgTaskInfo = new comm::PkgTaskInfo;
		pkgTaskInfo->set_id(10);
		pkgTaskInfo->set_name("Test_NodeWrap update task name");
		pkgTaskInfo->set_content("Test_NodeWrap update task content");
		pkgTaskInfo->set_tasklevel(comm::PkgTaskInfo::TL_IMPORTANCE_ARGENCY);
		pkgTaskInfo->set_status(comm::PkgTaskInfo::TS_NEW);
		pkgTaskInfo->set_sortorder("002");
		pkgTaskInfo->set_taskdate(UtilTime::getCurrentDate());
		pPkgTaskOperationReq->set_taskoperation(comm::PkgTaskOperationReq::TO_UPDATE_INFO);
		pPkgTaskOperationReq->set_allocated_taskinfo(pkgTaskInfo);
		pPkgTaskOperationReq->set_taskid(10);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply = boost::dynamic_pointer_cast<comm::PkgTaskOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskOperationReply != NULL, true);

		savetoFile("Test_taskOperation(delete task)", pReqMsg, pRetMsg, sretdata);

	}
}

TEST_F(Test_NodeWrap, Test_getDataWithUserId_getGetPasswordTable)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	{
		boost::shared_ptr<thirdinterface::PkgGetPasswordTableReq> pPkgGetPasswordTableReq = boost::make_shared<thirdinterface::PkgGetPasswordTableReq>();
		pPkgGetPasswordTableReq->set_startid(0);
		pPkgGetPasswordTableReq->set_gettype(thirdinterface::PkgGetPasswordTableReq::GT_PHONE);
		stypename = pPkgGetPasswordTableReq->GetTypeName();
		sjasondata = pb2json(*pPkgGetPasswordTableReq);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgGetPasswordTableReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<thirdinterface::PkgGetPasswordTableReply> pPkgGetPasswordTableReply = boost::dynamic_pointer_cast<thirdinterface::PkgGetPasswordTableReply>(pRetMsg);
		EXPECT_EQ(pPkgGetPasswordTableReply != NULL, true);

		//savetoFile(stypename, sjasondata);
		//std::string sreturntype = pRetMsg ? pRetMsg->GetTypeName(): stypename + "(return)";
		//savetoFile(sreturntype, sretdata);
	}
}
	
TEST_F(Test_NodeWrap, Test_getDataWithUserId_setGetPasswordTableStatus)
{	
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	{
		boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReq> pPkgSetGetPasswordTableStatusReq = boost::make_shared<thirdinterface::PkgSetGetPasswordTableStatusReq>();
		thirdinterface::PkgGetPasswordStatus*p = pPkgSetGetPasswordTableStatusReq->add_statuslist();
		p->set_id(1);
		p->set_status(thirdinterface::PkgGetPasswordStatus::ES_SENDING);
		p = pPkgSetGetPasswordTableStatusReq->add_statuslist();
		p->set_id(2);
		p->set_status(thirdinterface::PkgGetPasswordStatus::ES_SENDOK);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgSetGetPasswordTableStatusReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReply> pPkgSetGetPasswordTableStatusReply = boost::dynamic_pointer_cast<thirdinterface::PkgSetGetPasswordTableStatusReply>(pRetMsg);
		EXPECT_EQ(pPkgSetGetPasswordTableStatusReply != NULL, true);

		//savetoFile(stypename, sjasondata);
		//std::string sreturntype = pRetMsg ? pRetMsg->GetTypeName() : stypename + "(return)";
		//savetoFile(sreturntype, sretdata);
	}
}


TEST_F(Test_NodeWrap, Test_getDataWithUserId_taskSetOrder)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;

	for (int j = 0; j < 2;j++)
	{//排序
		boost::shared_ptr<comm::PkgTaskSetOrderReq> pPkgTaskSetOrderReq = boost::make_shared<comm::PkgTaskSetOrderReq>();
		for (int i = 1; i < 10; i++)
		{
			comm::PkgTaskOrder *pPkgTaskOrder = pPkgTaskSetOrderReq->add_taskorderlist();
			pPkgTaskOrder->set_taskid(i);
			std::string sOrder = "000" + boost::lexical_cast<std::string>(i * 10);
			pPkgTaskOrder->set_sortorder(sOrder);
			//pPkgTaskOrder->set_iscowork(j>0?true:false);

		}

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskSetOrderReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskSetOrderReply> pPkgTaskSetOrderReply = boost::dynamic_pointer_cast<comm::PkgTaskSetOrderReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskSetOrderReply != NULL, true);

		//savetoFile(stypename, sjasondata);
		//std::string sreturntype = pRetMsg ? pRetMsg->GetTypeName() : stypename + "(return)";
		//savetoFile(sreturntype, sretdata);
	}
}

TEST_F(Test_NodeWrap, Test_taskQuery)
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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskQueryReply> pPkgTaskQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskQueryReply != NULL, true);

		savetoFile("Test_taskQuery(todaymytask)", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap, Test_taskPageQuery)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	int nUserId = 1;
	{//查询==>今日任务列表
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskdatestart(UtilTime::getCurrentDate());
		pPkgTaskQueryCondition->set_taskdateend(UtilTime::getCurrentDate());
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(mytask)", pReqMsg, pRetMsg, sretdata);

	}

	{//查询==>已完成任务列表
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskstatus(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(allfinishedmywork)", pReqMsg, pRetMsg, sretdata);
	}

	{//查询==>已完成任务列表+过滤条件(任务名字)
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_taskstatus(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskQueryCondition->set_keyforname(UtilString::convertWstringToUTF8(L"任务"));//UTF-8格式
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS | comm::PkgTaskPageQueryReq::TQC_KEYFORNAME);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(allfinishedmyworkwithname)", pReqMsg, pRetMsg, sretdata);
	}

	{//查询==>选择任务：所有自己的任务 + 状态非 TS_FINISHED 的任务
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->set_tasknotstatus(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKNOTSTATUS);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgTaskPageQueryReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::dynamic_pointer_cast<comm::PkgTaskPageQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgTaskPageQueryReply != NULL, true);

		savetoFile("Test_taskPageQuery(allnotfinishedmywork)", pReqMsg, pRetMsg, sretdata);

	}
}


TEST_F(Test_NodeWrap, Test_getDataWithUserId_groupQuery)
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
	sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

	boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
	boost::shared_ptr<comm::PkgCompanyGroupQueryReply> pPkgCompanyGroupQueryReply = boost::dynamic_pointer_cast<comm::PkgCompanyGroupQueryReply>(pRetMsg);
	EXPECT_EQ(pPkgCompanyGroupQueryReply != NULL, true);

	//savetoFile(stypename, sjasondata);
	//std::string sreturntype = pRetMsg ? pRetMsg->GetTypeName(): stypename + "(return)";
	//savetoFile(sreturntype, sretdata);
}


TEST_F(Test_NodeWrap, Test_advQuery)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgAdvQueryReq> pPkgAdvQueryReq = boost::make_shared<comm::PkgAdvQueryReq>();
	pPkgAdvQueryReq->set_advid(1);
	pPkgAdvQueryReq->set_numreq(3);
	std::map<std::string, int> mapQuery;
	mapQuery["QueryAll"] = comm::PkgAdvQueryReq::QT_ALL;
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
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgAdvQueryReply> pPkgAdvQueryReply = boost::dynamic_pointer_cast<comm::PkgAdvQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvQueryReply != NULL, true);

		savetoFile("Test_advQuery(" + sKey + ")", pReqMsg, pRetMsg, sretdata);
	}

}

TEST_F(Test_NodeWrap, Test_advOperation)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	int advid = 0;
	boost::shared_ptr<comm::PkgAdvOperationReq> pPkgAdvOperationReq = boost::make_shared<comm::PkgAdvOperationReq>();
	{
		comm::PkgAdvInfo*pPkgAdvInfo = new comm::PkgAdvInfo;
		pPkgAdvInfo->set_name("advname");
		pPkgAdvInfo->set_advtxt("advtxt");
		pPkgAdvInfo->set_advdesc("advdesc");
		pPkgAdvInfo->set_expiretime("2014-10-30 12:55:22");
		pPkgAdvInfo->set_urlpic("http://localhost:3000/advquery?id=1");

		pPkgAdvOperationReq->set_advoperation(comm::PkgAdvOperationReq::TO_INSERT_INFO);
		pPkgAdvOperationReq->set_allocated_advinfo(pPkgAdvInfo);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgAdvOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgAdvOperationReply> pPkgAdvOperationReply = boost::dynamic_pointer_cast<comm::PkgAdvOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvOperationReply != NULL, true);
		advid = pPkgAdvOperationReply->advid();
		savetoFile("Test_advOperation(insertadv)", pReqMsg, pRetMsg, sretdata);
	}

	{
		pPkgAdvOperationReq->set_advid(advid);
		
		comm::PkgAdvInfo*pPkgAdvInfo = new comm::PkgAdvInfo;
		pPkgAdvInfo->set_id(advid);
		pPkgAdvInfo->set_name("advnameupdate");
		pPkgAdvInfo->set_advtxt("advtxtupdate");
		pPkgAdvInfo->set_advdesc("advdescupdate");
		pPkgAdvInfo->set_expiretime("2014-10-30 12:55:22");
		pPkgAdvInfo->set_urlpic("http://localhost:3000/advquery?id=1");

		pPkgAdvOperationReq->set_advoperation(comm::PkgAdvOperationReq::TO_UPDATE_INFO);
		pPkgAdvOperationReq->set_allocated_advinfo(pPkgAdvInfo);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgAdvOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgAdvOperationReply> pPkgAdvOperationReply = boost::dynamic_pointer_cast<comm::PkgAdvOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvOperationReply != NULL, true);

		savetoFile("Test_advOperation(updateadv)", pReqMsg, pRetMsg, sretdata);

	}

	{
		pPkgAdvOperationReq->set_advoperation(comm::PkgAdvOperationReq::TO_DELETE_INFO);
		pPkgAdvOperationReq->set_advid(advid);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgAdvOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<comm::PkgAdvOperationReply> pPkgAdvOperationReply = boost::dynamic_pointer_cast<comm::PkgAdvOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvOperationReply != NULL, true);

		savetoFile("Test_advOperation(deleteadv)", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap, Test_language)
{
	{
		UtilLanguage::get_mutable_instance().addLanguage("chinese", "E:\\jytask\\trunk\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap\\language\\chinese.xml");
		UtilLanguage::get_mutable_instance().addLanguage("english", "E:\\jytask\\trunk\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap\\language\\english.xml");
		pPkgSessionUser->set_applanguage("chinese");
	}
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<comm::PkgUserLoginReq> pPkgUserLoginReq = boost::make_shared<comm::PkgUserLoginReq>();
	pPkgUserLoginReq->set_gettype(comm::GT_PHONE);
	pPkgUserLoginReq->set_phonenumber("18118003691");
	pPkgUserLoginReq->set_password("123");
	boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserLoginReq;

	stypename = pReqMsg->GetTypeName();
	sjasondata = packMsgData(pReqMsg);
	sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

	boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
	boost::shared_ptr<comm::PkgUserLoginReply> pPkgUserLoginReply = boost::dynamic_pointer_cast<comm::PkgUserLoginReply>(pRetMsg);
	EXPECT_EQ(pPkgUserLoginReply != NULL, true);

	savetoFile("Test_userLogin", pReqMsg, pRetMsg, sretdata);
}


TEST_F(Test_NodeWrap, Test_adminUserLogin)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<admin::PkgUserLoginReq> pPkgUserLoginReq = boost::make_shared<admin::PkgUserLoginReq>();
	pPkgUserLoginReq->set_username("admin");
	pPkgUserLoginReq->set_password("1234");

	{

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserLoginReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgUserLoginReply> pPkgUserLoginReply = boost::dynamic_pointer_cast<admin::PkgUserLoginReply>(pRetMsg);
		EXPECT_EQ(pPkgUserLoginReply != NULL, true);

		savetoFile("Test_adminLogin", pReqMsg, pRetMsg, sretdata);
	}

}

TEST_F(Test_NodeWrap, Test_adminUserOperation)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	int userid = 0;
	boost::shared_ptr<admin::PkgUserOperationReq> pPkgUserOperationReq = boost::make_shared<admin::PkgUserOperationReq>();
	{
		admin::PkgUserInfo*pPkgUserInfo = new admin::PkgUserInfo;
		pPkgUserInfo->set_username("admin2");
		pPkgUserInfo->set_password("advtxt");
	

		pPkgUserOperationReq->set_useroperation(admin::PkgUserOperationReq::TO_INSERT_INFO);
		pPkgUserOperationReq->set_allocated_userinfo(pPkgUserInfo);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgUserOperationReply> pPkgUserOperationReply = boost::dynamic_pointer_cast<admin::PkgUserOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgUserOperationReply != NULL, true);
		userid = pPkgUserOperationReply->userid();
		savetoFile("Test_adminUserOperation(insertuser)", pReqMsg, pRetMsg, sretdata);
	}

	{
		pPkgUserOperationReq->set_userid(userid);

		admin::PkgUserInfo*pPkgUserInfo = new admin::PkgUserInfo;
		pPkgUserInfo->set_username("admin2");
		pPkgUserInfo->set_password("advtxt");

		pPkgUserOperationReq->set_useroperation(admin::PkgUserOperationReq::TO_UPDATE_INFO);
		pPkgUserOperationReq->set_allocated_userinfo(pPkgUserInfo);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgUserOperationReply> pPkgUserOperationReply = boost::dynamic_pointer_cast<admin::PkgUserOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgUserOperationReply != NULL, true);

		savetoFile("Test_adminUserOperation(updateuser)", pReqMsg, pRetMsg, sretdata);

	}

	{
		pPkgUserOperationReq->set_useroperation(admin::PkgUserOperationReq::TO_DELETE_INFO);
		pPkgUserOperationReq->set_userid(userid);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgUserOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgUserOperationReply> pPkgUserOperationReply = boost::dynamic_pointer_cast<admin::PkgUserOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgUserOperationReply != NULL, true);

		savetoFile("Test_adminUserOperation(deleteuser)", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap, Test_thirdMsg_msgInfo)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	{
		boost::shared_ptr<thirdinterface::PkgMsgInfoReq> pPkgMsgInfoReq = boost::make_shared<thirdinterface::PkgMsgInfoReq>();
		pPkgMsgInfoReq->set_isget(false);
		pPkgMsgInfoReq->set_emailtitle(UtilString::convertWstringToUTF8(L"简软任务注册提醒"));
		pPkgMsgInfoReq->set_emailcontent(UtilString::convertWstringToUTF8(L"尊敬的用户 您好：<br> 我们已收到了您的注册信息，您的验证码为[AuthCode]，请及时登录系统修改，如有其他疑问请咨询网站客服或致电0519-XXXX.简软任务平台网址：www.xxx.com<br>简软任务运营团队<br>0519 - xxx"));
		pPkgMsgInfoReq->set_smscontent(UtilString::convertWstringToUTF8(L"[简软任务]简软任务注册验证码[AuthCode],请尽快完成注册"));

		stypename = pPkgMsgInfoReq->GetTypeName();
		sjasondata = pb2json(*pPkgMsgInfoReq);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgMsgInfoReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<thirdinterface::PkgMsgInfoReply> pPkgMsgInfoReply = boost::dynamic_pointer_cast<thirdinterface::PkgMsgInfoReply>(pRetMsg);
		EXPECT_EQ(pPkgMsgInfoReply != NULL, true);

		savetoFile("Test_thirdMsg_msgInfo(set)", pReqMsg, pRetMsg, sretdata);

	}

	{
		boost::shared_ptr<thirdinterface::PkgMsgInfoReq> pPkgMsgInfoReq = boost::make_shared<thirdinterface::PkgMsgInfoReq>();
		pPkgMsgInfoReq->set_isget(true);
		stypename = pPkgMsgInfoReq->GetTypeName();
		sjasondata = pb2json(*pPkgMsgInfoReq);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgMsgInfoReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<thirdinterface::PkgMsgInfoReply> pPkgMsgInfoReply = boost::dynamic_pointer_cast<thirdinterface::PkgMsgInfoReply>(pRetMsg);
		EXPECT_EQ(pPkgMsgInfoReply != NULL, true);

		savetoFile("Test_thirdMsg_msgInfo(get)", pReqMsg, pRetMsg, sretdata); 
	}
}