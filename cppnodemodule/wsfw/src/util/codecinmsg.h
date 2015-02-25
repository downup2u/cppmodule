#ifndef _CODECINMSGUTIL_STRING
#define _CODECINMSGUTIL_STRING
#include "codec.h"
#include "packmsg.pb.h"
#include <string>

namespace codecutil
{
	boost::shared_ptr<comminternal::PkgMsg> getInMsgFromHex(const std::string& hex);
	std::string printInMsg(const boost::shared_ptr<comminternal::PkgMsg> pMsg);

	boost::shared_ptr<comminternal::PkgMsg> getClientMsg_InitStatus(comminternal::PkgMsg::EnMsgStatusCode status);
	boost::shared_ptr<comminternal::PkgMsg> getClientMsg_InitWithMsg(boost::shared_ptr<google::protobuf::Message>pMsg);
};

#endif