#ifndef _WEBSOCKETSERVER_INCLUDE_H
#define _WEBSOCKETSERVER_INCLUDE_H
#include "glog.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/smart_ptr.hpp>
#include <functional>
#include <mutex>
#include <set>
#include <thread>
#include <map>
#include "sessconn.h"



struct custom_config : public websocketpp::config::asio {
	typedef websocketpp::config::asio core;

	typedef core::concurrency_type concurrency_type;
	typedef core::request_type request_type;
	typedef core::response_type response_type;
	typedef core::message_type message_type;
	typedef core::con_msg_manager_type con_msg_manager_type;
	typedef core::endpoint_msg_manager_type endpoint_msg_manager_type;
	typedef core::alog_type alog_type;
	typedef core::elog_type elog_type;
	typedef core::rng_type rng_type;
	typedef core::transport_type transport_type;
	typedef core::endpoint_base endpoint_base;

	typedef sessconn connection_base;
};

typedef websocketpp::server<custom_config> server;
typedef server::connection_ptr connection_ptr;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class wssrv {
public:
	wssrv(boost::asio::io_service&io);
	void run(uint16_t port);
	void stop();

	void on_open(connection_hdl hdl);
	void on_close(connection_hdl hdl);
	void on_message(connection_hdl hdl, server::message_ptr msg);
	void on_message_callback(connection_hdl hdl, server::message_ptr msg);
private:
	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;
	con_list m_connections;
	std::mutex m_mutex;
    server m_server;
};

#endif