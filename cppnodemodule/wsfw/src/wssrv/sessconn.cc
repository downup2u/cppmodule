#include "sessconn.h"

sessconn::sessconn(){
	pPkgSessionUser_ = boost::make_shared<comminternal::PkgSessionUser>();
	psessdbdispatcher_ = boost::make_shared<sessdbdispatcher>();
	msgdispatcher_ = boost::make_shared<ProtobufDispatcher>();
	psessdbdispatcher_->registerCallback(msgdispatcher_);
}

sessconn::~sessconn()
{

}

void sessconn::onMessage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq, SessionCallBack callback){
	if (pMsgReq){
		boost::shared_ptr<google::protobuf::Message> pMsg = codecutil::getMsgFromHex(pMsgReq->reqmsgtype(), pMsgReq->reqmsgdata());
		msgdispatcher_->onMessage(pMsgReq, pMsg, pPkgSessionUser_, callback);
	}
}