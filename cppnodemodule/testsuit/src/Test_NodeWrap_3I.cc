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

#include "../../src/gcomm/thirdinterface.pb.h"
#include "../../src/DataWrap3I.h"
#include "../../src/mylib/util/utilstring.h"
#include "../../src/mylib/util/codec.h"
#include "../../src/mylib/util/utillanguage.h"

class Test_NodeWrap_3I : public testing::Test
{
public:
	virtual void SetUp()
	{
		int npoolsize = 4;
		bUseJson = false;
		std::string sXML = "E:\\jytask\\trunk\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap\\db\\dbinfo.xml";
		DataWrap3I::get_mutable_instance().initDB(sXML);
		pPkgSessionUser = boost::make_shared<session::PkgSessionUser>();
		pPkgSessionUser->set_userid(0);

		std::string hex = "08012207656e676c697368520431323334";

		boost::shared_ptr<google::protobuf::Message> pMsgSession = DataWrap3I::get_mutable_instance().getMsgFromHex("session.PkgSessionUser", hex);
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
std::string Test_NodeWrap_3I::packMsgData(boost::shared_ptr<google::protobuf::Message> pMsg)
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

std::string Test_NodeWrap_3I::getDataWithSession(const std::string& classname, const std::string& json, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sHexSessionData;
	pPkgSessionUser->AppendToString(&sHexSessionData);
	sHexSessionData = boost::algorithm::hex(sHexSessionData);
	std::string sOutTypeName;
	std::string sOutHexData;
	bool bSuc = DataWrap3I::get_mutable_instance().getDataMsg(classname, json, sHexSessionData, sOutTypeName, sOutHexData);
	return sOutTypeName + "|" + sOutHexData;
}

void Test_NodeWrap_3I::savetoFile(const std::string&testCase, boost::shared_ptr<google::protobuf::Message> pMsgReq, boost::shared_ptr<google::protobuf::Message> pMsgReply, const std::string&retData)
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


boost::shared_ptr<google::protobuf::Message> Test_NodeWrap_3I::getMsgFromData(const std::string&sData)
{
	boost::shared_ptr<google::protobuf::Message> pMsg;
	size_t n = sData.find("|");
	if (n != std::string::npos)
	{
		std::string sTypeName = sData.substr(0, n);
		std::string sJsonData = sData.substr(n+1, std::string::npos);
		if (bUseJson)
			pMsg = DataWrap3I::get_mutable_instance().getMsgFromJson(sTypeName, sJsonData);
		else
			pMsg = DataWrap3I::get_mutable_instance().getMsgFromHex(sTypeName, sJsonData);
	}
	return pMsg;
}

TEST_F(Test_NodeWrap_3I, test_json)
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
TEST_F(Test_NodeWrap_3I, initDBPool)
{
	int npoolsize = 4;
	std::string sXML = "E:\\jytask\\trunk\\bin\\nodeaddon\\sample\\node_modules\\iteasynodewrap\\db\\dbinfo.xml";
	DataWrap3I::get_mutable_instance().initDB(sXML);

}

TEST_F(Test_NodeWrap_3I, Test_getPasswordTable)
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

		savetoFile("Test_getPasswordTable", pReqMsg, pRetMsg, sretdata);
	}
}
	
TEST_F(Test_NodeWrap_3I, Test_setGetPasswordTableStatus)
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

		savetoFile("Test_setGetPasswordTableStatus", pReqMsg, pRetMsg, sretdata);

	}
}



TEST_F(Test_NodeWrap_3I, Test_syInfo)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	{
		boost::shared_ptr<thirdinterface::PkgSysInfoListGetReq> pPkgSysInfoGetReq = boost::make_shared<thirdinterface::PkgSysInfoListGetReq>();
		pPkgSysInfoGetReq->add_keystring("emailcontentregister");
		pPkgSysInfoGetReq->add_keystring("emailcontentinvition");
		stypename = pPkgSysInfoGetReq->GetTypeName();
		sjasondata = pb2json(*pPkgSysInfoGetReq);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgSysInfoGetReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<thirdinterface::PkgSysInfoListGetReply> pPkgSysInfoGetReply = boost::dynamic_pointer_cast<thirdinterface::PkgSysInfoListGetReply>(pRetMsg);
		EXPECT_EQ(pPkgSysInfoGetReply != NULL, true);

		savetoFile("Test_sysInfo", pReqMsg, pRetMsg, sretdata);

	}

	
}


TEST_F(Test_NodeWrap_3I, Test_getRegisterInvition)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;
	{
		boost::shared_ptr<thirdinterface::PkgRegisterInvitionReq> pPkgRegisterInvitionReq = boost::make_shared<thirdinterface::PkgRegisterInvitionReq>();
		pPkgRegisterInvitionReq->set_startid(0);
		pPkgRegisterInvitionReq->set_gettype(thirdinterface::PkgRegisterInvitionReq::GT_PHONE);
		stypename = pPkgRegisterInvitionReq->GetTypeName();
		sjasondata = pb2json(*pPkgRegisterInvitionReq);
		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgRegisterInvitionReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<thirdinterface::PkgRegisterInvitionReply> pPkgRegisterInvitionReply = boost::dynamic_pointer_cast<thirdinterface::PkgRegisterInvitionReply>(pRetMsg);
		EXPECT_EQ(pPkgRegisterInvitionReply != NULL, true);

		savetoFile("Test_getRegisterInvition", pReqMsg, pRetMsg, sretdata);
	}
}

TEST_F(Test_NodeWrap_3I, Test_setRegisterInvitionStatus)
{
	std::string stypename;
	std::string sjasondata;
	std::string sretdata;

	{
		boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReq> pPkgSetRegisterInvitionStatusReq = boost::make_shared<thirdinterface::PkgSetRegisterInvitionStatusReq>();
		thirdinterface::PkgRegisterInvitionStatus*p = pPkgSetRegisterInvitionStatusReq->add_statuslist();
		p->set_id(1);
		p->set_status(thirdinterface::PkgRegisterInvitionStatus::ES_SENDING);
		p = pPkgSetRegisterInvitionStatusReq->add_statuslist();
		p->set_id(2);
		p->set_status(thirdinterface::PkgRegisterInvitionStatus::ES_SENDOK);

		boost::shared_ptr<google::protobuf::Message> pReqMsg = pPkgSetRegisterInvitionStatusReq;

		stypename = pReqMsg->GetTypeName();
		sjasondata = packMsgData(pReqMsg);
		sretdata = getDataWithSession(stypename, sjasondata, pPkgSessionUser);

		boost::shared_ptr<google::protobuf::Message> pRetMsg = getMsgFromData(sretdata);
		boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReply> pPkgSetRegisterInvitionStatusReply = boost::dynamic_pointer_cast<thirdinterface::PkgSetRegisterInvitionStatusReply>(pRetMsg);
		EXPECT_EQ(pPkgSetRegisterInvitionStatusReply != NULL, true);

		savetoFile("Test_setRegisterInvitionStatus", pReqMsg, pRetMsg, sretdata);

	}
}
