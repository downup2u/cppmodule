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

#include "../../src/gcomm/admin.pb.h"
#include "../../src/DataWrapAdmin.h"
#include "../../src/mylib/util/utilstring.h"
#include "../../src/mylib/util/codec.h"
#include "../../src/mylib/util/utillanguage.h"

class Test_NodeWrap_Admin : public testing::Test
{
public:
	virtual void SetUp()
	{
		int npoolsize = 4;
		bUseJson = false;
		std::string sXML = "E:\\jytask\\trunk\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap\\db\\dbinfo.xml";
		bool bSuc = DataWrapAdmin::get_mutable_instance().initDB(sXML);

		pPkgSessionUser = boost::make_shared<session::PkgSessionUser>();
		pPkgSessionUser->set_userid(2);
		pPkgSessionUser->set_applanguage("chinese");

		//std::string hex = "08012207656e676c697368520431323334";

		//boost::shared_ptr<google::protobuf::Message> pMsgSession = DataWrapAdmin::get_mutable_instance().getMsgFromHex("session.PkgSessionUser", hex);
		//boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser2;
		//if (pMsgSession)
		//{
		//	pPkgSessionUser2 = boost::dynamic_pointer_cast<session::PkgSessionUser>(pMsgSession);
		//}

		//if (pPkgSessionUser2)
		//{
		//	std::cout << "test" << std::endl;
		//}
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
std::string Test_NodeWrap_Admin::packMsgData(boost::shared_ptr<google::protobuf::Message> pMsg)
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

std::string Test_NodeWrap_Admin::getDataWithSession(const std::string& classname, const std::string& json, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sHexSessionData;
	pPkgSessionUser->AppendToString(&sHexSessionData);
	sHexSessionData = boost::algorithm::hex(sHexSessionData);
	std::string sOutTypeName;
	std::string sOutHexData;
	bool bSuc = DataWrapAdmin::get_mutable_instance().getDataMsg(classname, json, sHexSessionData, sOutTypeName, sOutHexData);
	return sOutTypeName + "|" + sOutHexData;
}

void Test_NodeWrap_Admin::savetoFile(const std::string&testCase, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply, const std::string&retData)
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
			out << retData;
		}
		out.close();
	}
}


boost::shared_ptr<google::protobuf::Message> Test_NodeWrap_Admin::getMsgFromData(const std::string&sData)
{
	boost::shared_ptr<google::protobuf::Message> pMsg;
	size_t n = sData.find("|");
	if (n != std::string::npos)
	{
		std::string sTypeName = sData.substr(0, n);
		std::string sJsonData = sData.substr(n+1, std::string::npos);
		if (bUseJson)
			pMsg = DataWrapAdmin::get_mutable_instance().getMsgFromJson(sTypeName, sJsonData);
		else
			pMsg = DataWrapAdmin::get_mutable_instance().getMsgFromHex(sTypeName, sJsonData);
	}
	return pMsg;
}

TEST_F(Test_NodeWrap_Admin, test_json)
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
TEST_F(Test_NodeWrap_Admin, initDBPool)
{
	std::string sXML = "E:\\jytask\\trunk\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap\\db\\dbinfo.xml";
	bool bSuc = DataWrapAdmin::get_mutable_instance().initDB(sXML);
	EXPECT_EQ(bSuc, true);

}

TEST_F(Test_NodeWrap_Admin, Test_advQuery)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	boost::shared_ptr<admin::PkgAdvQueryReq> pPkgAdvQueryReq = boost::make_shared<admin::PkgAdvQueryReq>();
	pPkgAdvQueryReq->set_advid(1);
	pPkgAdvQueryReq->set_keyname("2");
	std::map<std::string, int> mapQuery;
	mapQuery["QueryAll"] = admin::PkgAdvQueryReq::QT_ALL;
	mapQuery["QueryKeyName"] = admin::PkgAdvQueryReq::QT_KEYNAME;
	mapQuery["QueryId"] = admin::PkgAdvQueryReq::QT_ADVID;

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
		boost::shared_ptr<admin::PkgAdvQueryReply> pPkgAdvQueryReply = boost::dynamic_pointer_cast<admin::PkgAdvQueryReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvQueryReply != NULL, true);

		savetoFile("Test_advQuery(" + sKey + ")", pReqMsg, pRetMsg, sretdata);
	}

}

TEST_F(Test_NodeWrap_Admin, Test_advOperation)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	int advid = 0;
	boost::shared_ptr<admin::PkgAdvOperationReq> pPkgAdvOperationReq = boost::make_shared<admin::PkgAdvOperationReq>();
	{
		admin::PkgAdvInfo*pPkgAdvInfo = new admin::PkgAdvInfo;
		pPkgAdvInfo->set_name("advname");
		pPkgAdvInfo->set_advtxt("advtxt");
		pPkgAdvInfo->set_advdesc("advdesc");
		pPkgAdvInfo->set_picpath("/upload/aaa.jpg");
		pPkgAdvInfo->set_expiretime("2014-10-30 12:55:22");
		pPkgAdvInfo->set_urlpic("http://localhost:3000/advquery?id=1");

		pPkgAdvOperationReq->set_advoperation(admin::PkgAdvOperationReq::TO_INSERT_INFO);
		pPkgAdvOperationReq->set_allocated_advinfo(pPkgAdvInfo);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgAdvOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgAdvOperationReply> pPkgAdvOperationReply = boost::dynamic_pointer_cast<admin::PkgAdvOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvOperationReply != NULL, true);
		advid = pPkgAdvOperationReply->advid();
		savetoFile("Test_advOperation(insertadv)", pReqMsg, pRetMsg, sretdata);
	}

	{
		pPkgAdvOperationReq->set_advid(advid);
		
		admin::PkgAdvInfo*pPkgAdvInfo = new admin::PkgAdvInfo;
		pPkgAdvInfo->set_id(advid);
		pPkgAdvInfo->set_name("advnameupdate");
		pPkgAdvInfo->set_advtxt("advtxtupdate");
		pPkgAdvInfo->set_advdesc("advdescupdate");
		pPkgAdvInfo->set_expiretime("2014-10-30 12:55:22");
		pPkgAdvInfo->set_urlpic("http://localhost:3000/advquery?id=1");

		pPkgAdvOperationReq->set_advoperation(admin::PkgAdvOperationReq::TO_UPDATE_INFO);
		pPkgAdvOperationReq->set_allocated_advinfo(pPkgAdvInfo);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgAdvOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgAdvOperationReply> pPkgAdvOperationReply = boost::dynamic_pointer_cast<admin::PkgAdvOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvOperationReply != NULL, true);

		savetoFile("Test_advOperation(updateadv)", pReqMsg, pRetMsg, sretdata);

	}

	{
		pPkgAdvOperationReq->set_advoperation(admin::PkgAdvOperationReq::TO_DELETE_INFO);
		pPkgAdvOperationReq->set_advid(advid);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgAdvOperationReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgAdvOperationReply> pPkgAdvOperationReply = boost::dynamic_pointer_cast<admin::PkgAdvOperationReply>(pRetMsg);
		EXPECT_EQ(pPkgAdvOperationReply != NULL, true);

		savetoFile("Test_advOperation(deleteadv)", pReqMsg, pRetMsg, sretdata);
	}
}


TEST_F(Test_NodeWrap_Admin, Test_adminUserLogin)
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

TEST_F(Test_NodeWrap_Admin, Test_adminUserOperation)
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

TEST_F(Test_NodeWrap_Admin, Test_setMsgInfo)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	{
		boost::shared_ptr<admin::PkgSysInfoSetReq> pPkgMsgInfoReq = boost::make_shared<admin::PkgSysInfoSetReq>();
		pPkgMsgInfoReq->set_keystring("emailtitleregister");
		//pPkgMsgInfoReq->set_valuestring(UtilString::convertWstringToUTF8(L"简软任务注册提醒Aa"));
		//pPkgMsgInfoReq->set_emailcontent(UtilString::convertWstringToUTF8(L"尊敬的用户 您好:<br> 我们已收到了您的注册信息，您的验证码为[AuthCode]，请及时登录系统修改，如有其他疑问请咨询网站客服或致电0519-XXXX.简软任务平台网址:www.xxx.com<br>简软任务运营团队<br>0519 - xxx"));
		//pPkgMsgInfoReq->set_smscontent(UtilString::convertWstringToUTF8(L"[简软任务]简软任务注册验证码[AuthCode],请尽快完成注册"));
		//pPkgMsgInfoReq->set_advurlbase("http://192.168.1.199/advquery?id=");
		//pPkgMsgInfoReq->set_advpathbase("e:/web/upload");
		stypename = pPkgMsgInfoReq->GetTypeName();
		sjasondata = pb2json(*pPkgMsgInfoReq);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgMsgInfoReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgSysInfoSetReply> pPkgMsgInfoReply = boost::dynamic_pointer_cast<admin::PkgSysInfoSetReply>(pRetMsg);
		EXPECT_EQ(pPkgMsgInfoReply != NULL, true);

		savetoFile("Test_msgInfo", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap_Admin, Test_getMsgInfo)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	{
		boost::shared_ptr<admin::PkgSysInfoGetReq> pPkgMsgInfoReq = boost::make_shared<admin::PkgSysInfoGetReq>();
		pPkgMsgInfoReq->set_keystring("emailtitleregister");
		stypename = pPkgMsgInfoReq->GetTypeName();
		sjasondata = pb2json(*pPkgMsgInfoReq);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgMsgInfoReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<admin::PkgSysInfoGetReply> pPkgGetMsgInfoReply = boost::dynamic_pointer_cast<admin::PkgSysInfoGetReply>(pRetMsg);
		EXPECT_EQ(pPkgGetMsgInfoReply != NULL, true);

		savetoFile("Test_getMsgInfo", pReqMsg, pRetMsg, sretdata);

	}


}