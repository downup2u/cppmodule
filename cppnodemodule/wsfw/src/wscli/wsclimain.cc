#include "wsclimain.h"


wsclimain::wsclimain(){
}

void wsclimain::start(const std::string& uri, SessionCallBack callback){
	callback_ = callback;
	wscli_ = boost::make_shared<wscli>(io_);
	workPtr_ = boost::make_shared<boost::asio::io_service::work>(io_);
	wscli_->start(uri, callback);
	t_ = boost::make_shared<boost::thread>(boost::bind(&boost::asio::io_service::run, &io_));
}

void wsclimain::stop(){
	
	if (wscli_){
		wscli_->stop();
		io_.stop();
		t_->join();
	}
}

void wsclimain::send_messsage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq){
	if (!wscli_){
		boost::shared_ptr<comminternal::PkgMsg> pPkgMsgReply = codecutil::getClientMsg_InitStatus(comminternal::PkgMsg_EnMsgStatusCode_MSG_CONNECTED);
		pPkgMsgReply->set_issuc(false);
		pPkgMsgReply->set_errdevdesc("not start");
		callback_(pPkgMsgReply);

	}
	else{
		io_.dispatch(boost::bind(&wscli::send_messsage, wscli_,pMsgReq));
	}
}
