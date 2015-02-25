#ifndef _CODEUTIL_STRING
#define _CODEUTIL_STRING
#include <string>
#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>
namespace CodecUtil
{
	boost::shared_ptr<google::protobuf::Message> createMsgFromTypeName(const std::string&name);
};

#endif