#ifndef _WEBSOCKETSERVER_MAIN_INCLUDE_H
#define _WEBSOCKETSERVER_MAIN_INCLUDE_H

#include "wssrv.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/singleton.hpp> 

class wssrvmain :public boost::serialization::singleton<wssrvmain> {
public:
	wssrvmain();
	void start(uint16_t port, const std::string& connstr, const std::string&bucketpassword = "");
	void stop();
	boost::asio::io_service&getIO(){ return io_; };
private:
	boost::asio::io_service io_;
	boost::shared_ptr<boost::asio::io_service::work> workPtr_;
	boost::shared_ptr<boost::thread> t_; 
	boost::shared_ptr<wssrv> wssrv_;

};

#endif