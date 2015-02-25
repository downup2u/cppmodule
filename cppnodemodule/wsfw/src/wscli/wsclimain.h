#ifndef _WEBSOCKETCLIENT_MAIN_INCLUDE_H
#define _WEBSOCKETCLIENT_MAIN_INCLUDE_H

#include "wscli.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/singleton.hpp> 
#include <boost/asio.hpp>

class wsclimain :public boost::serialization::singleton<wsclimain>{
public:
	wsclimain();
public:
	void start(const std::string& uri, SessionCallBack callback);
	void send_messsage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq);
	void stop();
private:
	boost::asio::io_service io_;
	boost::shared_ptr<boost::asio::io_service::work> workPtr_;
	boost::shared_ptr<boost::thread> t_;
	boost::shared_ptr<wscli> wscli_;
	SessionCallBack callback_;

};
#endif