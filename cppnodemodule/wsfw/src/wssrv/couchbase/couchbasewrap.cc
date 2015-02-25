#include "couchbasewrap.h"
#include "codec.h"

couchbasewrap::couchbasewrap()
{
}


void couchbasewrap::startSrv(boost::asio::io_service&io, const std::string&conn, const std::string&bucketpassword/* = ""*/){
	pSrv_ = boost::make_shared<couchbaseSrv>(io);
	pSrv_->startRun(conn, bucketpassword);
}

void couchbasewrap::processmessage(boost::shared_ptr<couchbasesrv::DBOperationReq> pMsg, const msgCallback&callback){
	pSrv_->processmessage(pMsg, callback);
}

void couchbasewrap::stopSrv(){
	pSrv_->notifyexit();
}
//
//boost::shared_ptr<comm2::PkgUserLoginReply> nosqlsrv::onmessage_userlogin(boost::shared_ptr<comm2::PkgUserLoginReq> pReq, boost::shared_ptr<comminternal::PkgSessionUser> pSessionUsr){
//	boost::shared_ptr<comm2::PkgUserLoginReply> pPkgUserLoginReply = boost::make_shared<comm2::PkgUserLoginReply>();
//
//	return pPkgUserLoginReply;
//}
//
//void nosqlsrv::test(){
//	std::string useruuid = CodecUtil::generateDocumentuuid("jytask_user_");
//	boost::shared_ptr<comm2::DBDocumentUserInfo> pDBDocumentUserInfo = boost::make_shared<comm2::DBDocumentUserInfo>();
//	pDBDocumentUserInfo->set_uuiduserid(useruuid);
//	pDBDocumentUserInfo->set_realname("wangxiaoqing");
//	pDBDocumentUserInfo->set_phonenumber("15961125167");
//
//	std::string docJson = CodecUtil::getJsonFromMsg(pDBDocumentUserInfo);
//
//	boost::shared_ptr<couchbasesrv::DBOperationReq> pDBOperationReq = boost::make_shared<couchbasesrv::DBOperationReq>();
//	pDBOperationReq->set_enlcboperation(couchbasesrv::lcb_store);
//	pDBOperationReq->set_dockey(useruuid);
//	pDBOperationReq->set_doctxt(docJson);
//	pDBOperationReq->set_enlcbstorecmd(couchbasesrv::DBOperationReq::LCB_SET);
//
//	pSrv_->processmessage(pDBOperationReq, [=](boost::shared_ptr<couchbasesrv::DBOperationReply> pDBOperationReply){
//		std::string sJsonResult = CodecUtil::getJsonFromMsg(pDBOperationReply);
//		std::cout << sJsonResult << std::endl;
//	});
//}