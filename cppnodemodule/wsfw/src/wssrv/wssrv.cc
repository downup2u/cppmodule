#include "wssrv.h"
#include "codecinmsg.h"

wssrv::wssrv(boost::asio::io_service&io){
	// Initialize Asio Transport
	m_server.init_asio(&io);
	// Register handler callbacks
	m_server.set_open_handler(bind(&wssrv::on_open, this, ::_1));
	m_server.set_close_handler(bind(&wssrv::on_close, this, ::_1));
	m_server.set_message_handler(bind(&wssrv::on_message, this, ::_1, ::_2));
}

void wssrv::run(uint16_t port) {
	// listen on specified port
	m_server.listen(port);
	// Start the server accept loop
	m_server.start_accept();
}

void wssrv::on_open(connection_hdl hdl){
	//同步
	LOG(INFO) << "(on_open)from client:";

	connection_ptr con = m_server.get_con_from_hdl(hdl);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_connections.insert(hdl);
}


void wssrv::on_close(connection_hdl hdl) {
	LOG(INFO) << "(on_close)from client:";
	//同步
	std::lock_guard<std::mutex> lock(m_mutex);
	m_connections.erase(hdl);
}

void wssrv::on_message(connection_hdl hdl, server::message_ptr msg) {
	//get message from msg
	boost::shared_ptr<comminternal::PkgMsg> pPkgMsgReq = codecutil::getInMsgFromHex(msg->get_payload());
	if (!pPkgMsgReq){
		LOG(INFO) << "(on_message)unknow client:" << msg->get_payload();
		m_server.close(hdl, 0, "unknow client");
		return;
	}
	connection_ptr con = m_server.get_con_from_hdl(hdl);
	if (con){
		con->onMessage(pPkgMsgReq, [pPkgMsgReq, hdl, this](boost::shared_ptr<comminternal::PkgMsg> pPkgMsgReply){
			pPkgMsgReply->set_msgtype(comminternal::PkgMsg_EnMsgType_MSG_REQ);
			std::string sOut = codecutil::getHexFromMsg(pPkgMsgReply);
			m_server.send(hdl, sOut, websocketpp::frame::opcode::text);
			if (!pPkgMsgReply->issuc()){
				m_server.close(hdl, 0, pPkgMsgReply->errdevdesc());
			}
		});
	}
}

void wssrv::stop(){

}
