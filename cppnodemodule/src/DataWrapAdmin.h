#ifndef _DataWrapAdmin_Include
#define _DataWrapAdmin_Include

#include <string>
#include <boost/serialization/singleton.hpp> 
#include <google/protobuf/message.h>
#include "dbsession/dbsessionadmin.h"
#include "./DataWrap.h"
class DataWrapAdmin :public DataWrap, public boost::serialization::singleton<DataWrapAdmin>
{
public:
	DataWrapAdmin();
	~DataWrapAdmin();
protected:
	boost::shared_ptr<DBSessionAdmin> dbSession_;
	virtual boost::shared_ptr<google::protobuf::Message> onRecvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser);
	virtual bool onOpenDB();

};
#endif