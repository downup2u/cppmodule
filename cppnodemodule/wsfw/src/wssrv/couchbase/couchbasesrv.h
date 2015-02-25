#ifndef _COUCHBASE_SERVER_INCLUDE_H
#define _COUCHBASE_SERVER_INCLUDE_H

#include <uv.h>
#include <iostream>
#include <libcouchbase/couchbase.h>
#include <libuvplugin/libuv_io_opts.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <list>
#include <map>
#include <boost/thread/mutex.hpp>
#include "couchbasesrv.pb.h"


typedef boost::function<void(boost::shared_ptr<couchbasesrv::DBOperationReply> pPkgMsg)> msgCallback;
struct messagepacket{
	boost::shared_ptr<couchbasesrv::DBOperationReq> pDBOperationReq;
	boost::shared_ptr<couchbasesrv::DBOperationReply> pDBOperationReply;
	msgCallback callback_;
};
struct messagedata{
	boost::shared_ptr<std::list<boost::shared_ptr<messagepacket>>> pMsgList;
	boost::mutex mutex_msgList_;
};

class couchbaseSrv{
public:
	couchbaseSrv(boost::asio::io_service&io);
public:
	void startRun(const std::string&conn, const std::string&bucketpassword="");
private:
	static uv_async_t exit_handle;
	static uv_async_t message_handle;
	static std::map<int, boost::shared_ptr<messagepacket>> mapPacket_;

	lcb_t create_libcouchbase_handle(lcb_io_opt_t ioops);
	static void exit_async_cb(uv_async_t* handle);
	static void on_message(uv_async_t* handle);
	static void do_message(boost::shared_ptr<messagepacket> pmsgpacket);
	boost::shared_ptr<boost::thread> pthread_;
	void on_run();
	std::string connstr_;
	std::string bucketpassword_;
public:
	static void on_removed(lcb_t instance, const void *cookie, lcb_error_t err, const lcb_remove_resp_t *resp);
	static void bootstrap_callback(lcb_t instance, lcb_error_t err);
	static void get_callback(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
	static void store_callback(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
	static void http_callback(lcb_http_request_t request, lcb_t instance, const void *cookie,lcb_error_t error, const lcb_http_resp_t *resp);

	void notifyexit();

	void processmessage(boost::shared_ptr<couchbasesrv::DBOperationReq> pMsg, const msgCallback&callback);

};

#endif