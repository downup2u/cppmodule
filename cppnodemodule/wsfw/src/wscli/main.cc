#include <string>
#include <boost/make_shared.hpp>
#include "wsclimain.h"
#include "xkcrm.pb.h"

void onclient_msg(boost::shared_ptr<comminternal::PkgMsg> pMsgReply){
	if (pMsgReply){

	}
}


int main(int argc, char* argv[]) {

	std::string uri = "ws://localhost:9002";
	wsclimain::get_mutable_instance().start(uri,boost::bind(onclient_msg,_1));
 
	char ch = getchar();
	while (ch != 'q'){
		if (ch == 'l'){			
			boost::shared_ptr<xkcrm::PkgUserLoginReq> pReq = boost::make_shared<xkcrm::PkgUserLoginReq>();
			pReq->set_phonenumber("18118003691");
			pReq->set_password("123456");
			pReq->set_gettype(xkcrm::EnGetType::GT_PHONE);
			boost::shared_ptr<comminternal::PkgMsg> pPkgMsg = codecutil::getClientMsg_InitWithMsg(pReq);
			wsclimain::get_mutable_instance().send_messsage(pPkgMsg);
		}
		
		ch = getchar();

	}

	wsclimain::get_mutable_instance().stop();
   
}