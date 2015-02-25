#ifndef _DataWrapComm_Include
#define _DataWrapComm_Include

#include <string>
#include <boost/serialization/singleton.hpp> 
#include <google/protobuf/message.h>


#include "dbsession/dbsessioncomm.h"
#include "./DataWrap.h"

class DataWrapComm :public DataWrap, public boost::serialization::singleton<DataWrapComm>
{
public:
	DataWrapComm();
	~DataWrapComm();
protected:
	boost::shared_ptr<DBSessionComm> dbSession_;
	boost::shared_ptr<google::protobuf::Message> onRecvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser);
	virtual bool onOpenDB();

};
#endif