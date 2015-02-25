#ifndef _CODEUTIL_STRING
#define _CODEUTIL_STRING
#include <string>
#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>
namespace codecutil
{
	boost::shared_ptr<google::protobuf::Message> createMsgFromTypeName(const std::string&name);

	boost::shared_ptr<google::protobuf::Message> getMsgFromHex(const std::string& classname, const std::string& hex);
	boost::shared_ptr<google::protobuf::Message> getMsgFromJson(const std::string& classname, const std::string& json);
	
	std::string getJsonFromMsg(boost::shared_ptr<google::protobuf::Message>pMsg);
	std::string getHexFromMsg(boost::shared_ptr<google::protobuf::Message>pMsg);

	std::string generateRandomSn();
	std::string generateDocumentuuid(const std::string&Prex);
};

#endif