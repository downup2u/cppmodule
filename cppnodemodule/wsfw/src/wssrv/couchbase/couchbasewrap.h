#ifndef _COUCHBASE_WRAP_INCLUDE_H
#define _COUCHBASE_WRAP_INCLUDE_H

#include <google/protobuf/message.h>

#include "couchbasesrv.h"
#include "packmsg.pb.h"

#include <boost/asio.hpp>
#include <boost/serialization/singleton.hpp> 


class couchbasewrap :public boost::serialization::singleton<couchbasewrap>{
public:
	couchbasewrap();
	void startSrv(boost::asio::io_service&io, const std::string& connstr, const std::string&bucketpassword = "");
	void processmessage(boost::shared_ptr<couchbasesrv::DBOperationReq> pMsg, const msgCallback&callback);
	void stopSrv();
private:
	boost::shared_ptr<couchbaseSrv> pSrv_;
protected:
};

#endif