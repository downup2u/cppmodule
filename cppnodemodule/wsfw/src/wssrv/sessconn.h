#ifndef _SESSION_INCLUDE_H
#define _SESSION_INCLUDE_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>


#include "sessdbdispatcher.h"

class sessconn {
public:
	sessconn();
	~sessconn();

public:
	void onMessage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq, SessionCallBack callback);
public:
	boost::shared_ptr<comminternal::PkgSessionUser> pPkgSessionUser_;
	boost::shared_ptr<sessdbdispatcher> psessdbdispatcher_;
	boost::shared_ptr<ProtobufDispatcher> msgdispatcher_;
};
#endif