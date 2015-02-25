#include "codecinmsg.h"
#include <sstream>
#include <boost/smart_ptr.hpp>

namespace codecutil
{
	boost::shared_ptr<comminternal::PkgMsg> getInMsgFromHex(const std::string& hex){
		return boost::dynamic_pointer_cast<comminternal::PkgMsg>(codecutil::getMsgFromHex("comminternal.PkgMsg", hex));
		
	}
	std::string printInMsg(const boost::shared_ptr<comminternal::PkgMsg> pMsg){
		std::string sJsonReq;
		std::string sJsonReply;
		std::stringstream ss;
		return ss.str();
	}

	boost::shared_ptr<comminternal::PkgMsg> getClientMsg_InitStatus(comminternal::PkgMsg::EnMsgStatusCode status){
		boost::shared_ptr<comminternal::PkgMsg> pPkgMsg = boost::make_shared<comminternal::PkgMsg>();
		//pPkgMsg->set_msgtype(comminternal::PkgMsg::EnMsgType::MSG_STATUSNONE);
		pPkgMsg->set_enmsgstatuscode(status);
		return pPkgMsg;
	}

	boost::shared_ptr<comminternal::PkgMsg> getClientMsg_InitWithMsg(boost::shared_ptr<google::protobuf::Message>pMsg){
		boost::shared_ptr<comminternal::PkgMsg> pPkgMsg = boost::make_shared<comminternal::PkgMsg>();
		pPkgMsg->set_msgdirection(comminternal::PkgMsg::EnMsgDirection::PkgMsg_EnMsgDirection_MSG_CS);
		pPkgMsg->set_reqmsgtype(pMsg->GetTypeName());
		pPkgMsg->set_reqmsgdata(getHexFromMsg(pMsg));
		pPkgMsg->set_msgtype(comminternal::PkgMsg::EnMsgType::PkgMsg_EnMsgType_MSG_REQ);
		return pPkgMsg;

	}

};
