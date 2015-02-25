#include "codec.h"

namespace CodecUtil
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
};
