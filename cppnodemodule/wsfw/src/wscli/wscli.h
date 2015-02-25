#ifndef _WEBSOCKETCLIENT_INCLUDE_H
#define _WEBSOCKETCLIENT_INCLUDE_H
#include "glog.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include "codecinmsg.h"
typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;


typedef boost::function<void(boost::shared_ptr<comminternal::PkgMsg> pMsgReply)> SessionCallBack;

class wscli{
public:
	wscli(boost::asio::io_service&io);

public:
	void start(const std::string& uri, SessionCallBack callback);
	void stop();
	void send_messsage(boost::shared_ptr<comminternal::PkgMsg> pMsgReq);
	void on_open(websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);
	void on_message(websocketpp::connection_hdl hdl, message_ptr msg);
private:
	client client_;
	SessionCallBack callback_;
	websocketpp::connection_hdl hdl_;
};
#endif