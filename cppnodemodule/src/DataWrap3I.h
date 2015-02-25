#ifndef _DataWrap3I_Include
#define _DataWrap3I_Include

#include <string>
#include <boost/serialization/singleton.hpp> 
#include <google/protobuf/message.h>

#include "dbsession/dbsession3i.h"
#include "./DataWrap.h"

class DataWrap3I :public DataWrap, public boost::serialization::singleton<DataWrap3I>
{
public:
	DataWrap3I();
	~DataWrap3I();
protected:
	boost::shared_ptr<DBSession3I> dbSession_;
	virtual bool onOpenDB();
	virtual boost::shared_ptr<google::protobuf::Message> onRecvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser);
};



#endif