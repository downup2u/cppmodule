#ifndef _DataWrap_Include
#define _DataWrap_Include

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/singleton.hpp> 
#include <google/protobuf/message.h>
#include "../gcomm/session.pb.h"

class DataWrap
{
public:
	DataWrap();
	~DataWrap();
public:
	bool initDB(const std::string&sXML);
public:
	bool getDataMsg(const std::string& sInTypeName, const std::string& sInHexData, std::string& sSessionData, std::string& sOutTypeName, std::string&sOutHexData,bool isHex = true);
public:
	boost::shared_ptr<google::protobuf::Message> getMsgFromJson(const std::string& classname, const std::string& json);
	boost::shared_ptr<google::protobuf::Message> getMsgFromHex(const std::string& classname, const std::string& json);
private:
	boost::shared_ptr<google::protobuf::Message> recvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser);
protected:
	bool initDBPool(int poolsize, const std::string& connectString);
	virtual boost::shared_ptr<google::protobuf::Message> onRecvMsgWithSession(boost::shared_ptr<google::protobuf::Message>pMsg, boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser)=0;
	virtual bool onOpenDB() = 0;
protected:
	bool getConnectDBString(const std::string&sXML,std::string&sConnectString,int&nNum);
protected:
	int poolsize_;
	std::string connectstring_;
};
#endif