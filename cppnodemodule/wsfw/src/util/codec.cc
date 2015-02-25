#include "codec.h"
#include "nodeexception.h"
#include <boost/algorithm/hex.hpp>
#include <json2pb.h>
#include <boost/uuid/uuid.hpp>  
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace codecutil
{

boost::shared_ptr<google::protobuf::Message> createMsgFromTypeName(const std::string&type_name)
{
	google::protobuf::Message*message=NULL;
	const google::protobuf::Descriptor* descriptor =
		google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
	if (descriptor)
	{
		const google::protobuf::Message* prototype =
			google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	return boost::shared_ptr<google::protobuf::Message>(message);
}

boost::shared_ptr<google::protobuf::Message> getMsgFromHex(const std::string& classname, const std::string& hex)
{
	boost::shared_ptr<google::protobuf::Message> pMsg = codecutil::createMsgFromTypeName(classname);
	if (pMsg)
	{
		try
		{
			std::string sRawData = boost::algorithm::unhex(hex);
			pMsg->ParseFromString(sRawData);
		}
		catch (const std::exception&e)
		{
			std::string err = e.what();
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str(err);
		}
		catch (...)
		{
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str("unknowerror");
		}
	}
	return pMsg;
}

std::string getJsonFromMsg(boost::shared_ptr<google::protobuf::Message>pMsg){
	std::string sJson;
	if (pMsg){
		sJson = pb2json(*pMsg);
	}
	return sJson;
}
std::string getHexFromMsg(boost::shared_ptr<google::protobuf::Message>pMsg){
	std::string sHex;
	if (pMsg){
		pMsg->AppendToString(&sHex);
		sHex = boost::algorithm::hex(sHex);
	}
	return sHex;
}

boost::shared_ptr<google::protobuf::Message> getMsgFromJson(const std::string& classname, const std::string& json)
{
	boost::shared_ptr<google::protobuf::Message> pMsg = codecutil::createMsgFromTypeName(classname);
	if (pMsg)
	{
		try
		{
			json2pb(*pMsg, json.c_str(), json.length());
		}
		catch (const std::exception&e)
		{
			std::string err = e.what();
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str(err);
		}
		catch (...)
		{
			pMsg.reset();
			throw nodeexception() << err_no(ERROR_MSG) << errtype_str("msgerror") << err_str("unknowerror");
		}
	}
	return pMsg;
}

std::string generateRandomSn()
{
	boost::uuids::random_generator rgen;
	boost::uuids::uuid ranUUID = rgen();
	std::stringstream ss;
	ss << ranUUID;
	return ss.str();
}
std::string generateDocumentuuid(const std::string&Prex){
	return Prex + generateRandomSn();
}
};
