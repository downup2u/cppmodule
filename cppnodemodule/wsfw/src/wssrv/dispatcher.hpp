#ifndef DISPATCH_INCLUDE_HPP
#define DISPATCH_INCLUDE_HPP
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "codecinmsg.h"
#include <iostream>

typedef boost::function<void(boost::shared_ptr<comminternal::PkgMsg> pMsgReply)> SessionCallBack;


class Callback : boost::noncopyable
{
public:
	virtual ~Callback() {};
	virtual void onMessage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq,boost::shared_ptr<google::protobuf::Message> message, boost::shared_ptr<comminternal::PkgSessionUser>pPkgSessionUser, const SessionCallBack callback) const = 0;
};


template <typename T>
class CallbackT : public Callback
{
public:
	typedef boost::function<void(boost::shared_ptr<comminternal::PkgMsg> pMsgReq, boost::shared_ptr<T> message, boost::shared_ptr<comminternal::PkgSessionUser>, const SessionCallBack)> ProtobufMessageCallback;

	CallbackT(const ProtobufMessageCallback& callback)
		: callback_(callback)
	{
	}

	virtual void onMessage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq, boost::shared_ptr<google::protobuf::Message> message, boost::shared_ptr<comminternal::PkgSessionUser>pPkgSessionUser, const SessionCallBack callback) const
	{
		boost::shared_ptr<T> t = boost::dynamic_pointer_cast<T>(message);
		//T* t = dynamic_cast<T*>(message);
		assert(t != NULL);
		callback_(pMsgReq,t, pPkgSessionUser,callback);
	}

private:
	ProtobufMessageCallback callback_;
};



class ProtobufDispatcher
{
public:

	ProtobufDispatcher()		
	{
	}

	void onMessage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq, boost::shared_ptr<google::protobuf::Message> message, boost::shared_ptr<comminternal::PkgSessionUser>pPkgSessionUser, const SessionCallBack callback) const
	{
		CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
		if (it != callbacks_.end())
		{
			it->second->onMessage(pMsgReq,message, pPkgSessionUser, callback);
		}
	
	}

	template<typename T>
	void registerMessageCallback(const typename CallbackT<T>::ProtobufMessageCallback& callback)
	{
		boost::shared_ptr<CallbackT<T> > pd(new CallbackT<T>(callback));
		callbacks_[T::descriptor()] = pd;
	}

	typedef std::map<const google::protobuf::Descriptor*, boost::shared_ptr<Callback> > CallbackMap;
	CallbackMap callbacks_;
	boost::function<void(boost::shared_ptr<comminternal::PkgMsg> pMsgReq, boost::shared_ptr<google::protobuf::Message> message, boost::shared_ptr<comminternal::PkgSessionUser>, const SessionCallBack)> defaultCallback_;
};


#endif