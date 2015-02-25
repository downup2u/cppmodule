#include "couchbasesrv.h"
#include <boost/bind.hpp>
#include "nodeexception.h"
#include "codec.h"

uv_async_t couchbaseSrv::exit_handle = { 0 };
uv_async_t couchbaseSrv::message_handle = { 0 };
lcb_t db_instance_ = NULL;
boost::asio::io_service* io_ = NULL;
std::map<int, boost::shared_ptr<messagepacket>> couchbaseSrv::mapPacket_;
//-----------------------------------------------
couchbaseSrv::couchbaseSrv(boost::asio::io_service&io){
	io_ = &io;
}

void couchbaseSrv::bootstrap_callback(lcb_t instance, lcb_error_t err)
{
	lcb_store_cmd_t cmd;
	const lcb_store_cmd_t *cmdlist = &cmd;

	if (err != LCB_SUCCESS) {
		throw nodeexception() << err_no(ERROR_DB) << errtype_str("dberror") << err_str(lcb_strerror(db_instance_, err));
	}

	messagedata*pNewMsgData = new messagedata;
	pNewMsgData->pMsgList = boost::make_shared<std::list<boost::shared_ptr<messagepacket>>>();
	message_handle.data = pNewMsgData;
}

lcb_t couchbaseSrv::create_libcouchbase_handle(lcb_io_opt_t ioops)
{
	lcb_t instance;
	lcb_error_t error;
	struct lcb_create_st copts = { 0 };

	copts.version = 3;
	copts.v.v3.connstr = connstr_.c_str();
	copts.v.v3.io = ioops;
	copts.v.v3.passwd = bucketpassword_.c_str();
	lcb_create(&instance, &copts);
	lcb_connect(instance);

	lcb_set_bootstrap_callback(instance, couchbaseSrv::bootstrap_callback);
	lcb_set_get_callback(instance, couchbaseSrv::get_callback);
	lcb_set_remove_callback(instance, couchbaseSrv::on_removed);
	lcb_set_store_callback(instance, couchbaseSrv::store_callback);
	lcb_set_http_data_callback(instance, couchbaseSrv::http_callback);

	return instance;
}

void couchbaseSrv::exit_async_cb(uv_async_t* handle) {
	uv_close((uv_handle_t*)&exit_handle, NULL);
	uv_close((uv_handle_t*)&message_handle, NULL);
	messagedata*pNewMsgData = (messagedata*)message_handle.data;
	delete pNewMsgData;
	message_handle.data = NULL;
}

void couchbaseSrv::on_message(uv_async_t* handle) {
	messagedata*pNewMsgData = (messagedata*)message_handle.data;
	while (pNewMsgData){
		boost::shared_ptr<messagepacket> pMsgpacket;
		{
			pNewMsgData->mutex_msgList_.lock();
			if (!pNewMsgData->pMsgList->empty()){
				pMsgpacket = pNewMsgData->pMsgList->front();
				pNewMsgData->pMsgList->pop_front();
			}
			pNewMsgData->mutex_msgList_.unlock();
		}
		if (!pMsgpacket){
			break;
		}
		do_message(pMsgpacket);
	} 

}

void couchbaseSrv::do_message(boost::shared_ptr<messagepacket> pmsgpacket){
	//messagepacket*pNew = new messagepacket(*pmsgpacket);
	static int mapkey = 0;
	mapPacket_[++mapkey] = pmsgpacket;
	std::cout << "do_message cookie:" << mapkey << std::endl;
	boost::shared_ptr<couchbasesrv::DBOperationReq> pDBOperationReq = pmsgpacket->pDBOperationReq;
	lcb_error_t lcberr;
	if (pDBOperationReq){
		boost::shared_ptr<couchbasesrv::DBOperationReply> pDBOperationReply = boost::make_shared<couchbasesrv::DBOperationReply>();
		pDBOperationReply->set_enlcboperation(pDBOperationReq->enlcboperation());
		pDBOperationReply->set_uuidstring(pDBOperationReq->uuidstring());
		pmsgpacket->pDBOperationReply = pDBOperationReply;
		if (pDBOperationReq->enlcboperation() == couchbasesrv::lcb_get){
			lcb_get_cmd_t cmd = { 0 };
			const lcb_get_cmd_t *cmdlist = &cmd;
			cmd.v.v0.key = pDBOperationReq->dockey().data();
			cmd.v.v0.nkey = pDBOperationReq->dockey().size();

			lcberr = lcb_get(db_instance_,(const void*)mapkey, 1, &cmdlist);
			if (lcberr == LCB_SUCCESS){
				return;
			}
		}
		else if (pDBOperationReq->enlcboperation() == couchbasesrv::lcb_store){
			struct lcb_store_cmd_st cmd;
			const lcb_store_cmd_t *cmdlist = &cmd;

			cmd.v.v0.key = pDBOperationReq->dockey().data();
			cmd.v.v0.nkey = pDBOperationReq->dockey().size();
			cmd.v.v0.bytes = pDBOperationReq->doctxt().data();
			cmd.v.v0.nbytes = pDBOperationReq->doctxt().size();
			cmd.v.v0.operation = (lcb_storage_t)pDBOperationReq->enlcbstorecmd();

			lcberr = lcb_store(db_instance_, (const void*)mapkey, 1, &cmdlist);
			if (lcberr == LCB_SUCCESS){
				return;
			}
		}
		else if (pDBOperationReq->enlcboperation() == couchbasesrv::lcb_remove){
			lcb_remove_cmd_t cmd = { 0 };
			const lcb_remove_cmd_t *cmdlist = &cmd;
			cmd.v.v0.key = pDBOperationReq->dockey().data();
			cmd.v.v0.nkey = pDBOperationReq->dockey().size();
			lcberr = lcb_remove(db_instance_, (const void*)mapkey, 1, &cmdlist);
			if (lcberr == LCB_SUCCESS){
				return;
			}

		}
		else if (pDBOperationReq->enlcboperation() == couchbasesrv::lcb_httprequest){
			lcb_http_request_t req;
			lcb_http_cmd_t cd;
			lcb_http_cmd_t *cmd = &cd;
			cmd->version = 0;
			cmd->v.v0.path = pDBOperationReq->path().data();
			cmd->v.v0.npath = pDBOperationReq->path().size();
			cmd->v.v0.body = pDBOperationReq->body().data();
			cmd->v.v0.nbody = pDBOperationReq->body().size();
			cmd->v.v0.method = (lcb_http_method_t)pDBOperationReq->enhttpmethod();
			cmd->v.v0.chunked = 1;
			cmd->v.v0.content_type = pDBOperationReq->contenttype().data();
			lcberr = lcb_make_http_request(db_instance_, (const void*)mapkey, LCB_HTTP_TYPE_VIEW, cmd, &req);
			if (lcberr == LCB_SUCCESS){
				return;
			}
		}
		pDBOperationReply->set_issuc(false);
		pDBOperationReply->set_errdesc(lcb_strerror(db_instance_, lcberr));
		io_->dispatch(boost::bind(pmsgpacket->callback_, pDBOperationReply));
	//	pNew->callback_(pDBOperationReply);
	}
	std::cout << "do_message delete:" << mapkey << std::endl;
	//delete pNew;	
	mapPacket_.erase(mapkey);
}

void couchbaseSrv::on_run(){
	uv_loop_t *loop = uv_loop_new();

	uv_async_init(loop, &exit_handle, couchbaseSrv::exit_async_cb);
	uv_async_init(loop, &message_handle, couchbaseSrv::on_message);

	lcb_io_opt_t io;
	lcbuv_options_t options;
	options.v.v0.loop = loop;
	options.v.v0.startsop_noop = 1;
	lcb_create_libuv_io_opts(0, &io, &options);

	db_instance_ = create_libcouchbase_handle(io);
	lcb_set_cookie(db_instance_, this);

	int r = uv_run(loop, UV_RUN_DEFAULT);

	lcb_destroy(db_instance_);
	db_instance_ = NULL;
	std::cout << "end thread..." << std::endl;
}

void couchbaseSrv::startRun(const std::string&conn, const std::string&bucketpassword){
	connstr_ = conn;
	bucketpassword_ = bucketpassword;
	pthread_ = boost::make_shared<boost::thread>(boost::bind(&couchbaseSrv::on_run,this));
}

void couchbaseSrv::notifyexit(){
	uv_async_send(&exit_handle);
	if (pthread_){
		pthread_->join();
		pthread_.reset();
	}
}

//»Øµ÷
void couchbaseSrv::get_callback(lcb_t instance, const void *cookie, lcb_error_t lcberr, const lcb_get_resp_t *resp){
	int mapkey = (int)cookie;
	std::map<int, boost::shared_ptr<messagepacket>>::iterator itermap = mapPacket_.find(mapkey);
	if (itermap == mapPacket_.end())
		return;
	boost::shared_ptr<messagepacket> pmsgpacket = (*itermap).second;
	mapPacket_.erase(itermap);

	if (lcberr != LCB_SUCCESS){
		pmsgpacket->pDBOperationReply->set_issuc(false);
		pmsgpacket->pDBOperationReply->set_errdesc(lcb_strerror(db_instance_, lcberr));
	}	
	else{
		pmsgpacket->pDBOperationReply->set_issuc(true);
		std::string key;
		key.assign((char*)resp->v.v0.key, resp->v.v0.nkey);
		std::string value;
		value.assign((char*)resp->v.v0.bytes, (char*)resp->v.v0.nbytes);
		pmsgpacket->pDBOperationReply->set_dockey(key);
		pmsgpacket->pDBOperationReply->set_doctxt(value);
	}
	if (pmsgpacket->callback_){
		io_->dispatch(boost::bind(pmsgpacket->callback_, pmsgpacket->pDBOperationReply));
		//pmsgpacket->callback_(pmsgpacket->pDBOperationReply);
	}
}

void couchbaseSrv::store_callback(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t lcberr, const lcb_store_resp_t *resp)
{
	int mapkey = (int)cookie;
	std::map<int, boost::shared_ptr<messagepacket>>::iterator itermap = mapPacket_.find(mapkey);
	if (itermap == mapPacket_.end())
		return;
	boost::shared_ptr<messagepacket> pmsgpacket = (*itermap).second;
	mapPacket_.erase(itermap);
	pmsgpacket->pDBOperationReply->set_issuc(true);
	if (lcberr != LCB_SUCCESS){
		pmsgpacket->pDBOperationReply->set_issuc(false);
		pmsgpacket->pDBOperationReply->set_errdesc(lcb_strerror(db_instance_, lcberr));
	}
	if (pmsgpacket->callback_){
		io_->dispatch(boost::bind(pmsgpacket->callback_, pmsgpacket->pDBOperationReply));
	//	pmsgpacket->callback_(pmsgpacket->pDBOperationReply);
	}
}

void couchbaseSrv::on_removed(lcb_t instance, const void *cookie, lcb_error_t lcberr, const lcb_remove_resp_t *resp) {
	int mapkey = (int)cookie;
	std::map<int, boost::shared_ptr<messagepacket>>::iterator itermap = mapPacket_.find(mapkey);
	if (itermap == mapPacket_.end())
		return;
	boost::shared_ptr<messagepacket> pmsgpacket = (*itermap).second;
	mapPacket_.erase(itermap);
	pmsgpacket->pDBOperationReply->set_issuc(true);
	if (lcberr != LCB_SUCCESS){
		pmsgpacket->pDBOperationReply->set_issuc(false);
		pmsgpacket->pDBOperationReply->set_errdesc(lcb_strerror(db_instance_, lcberr));
	}
	if (pmsgpacket->callback_){
		io_->dispatch(boost::bind(pmsgpacket->callback_, pmsgpacket->pDBOperationReply));

	//	pmsgpacket->callback_(pmsgpacket->pDBOperationReply);
	}
}

void couchbaseSrv::http_callback(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t lcberr, const lcb_http_resp_t *resp){
	int mapkey = (int)cookie;
	std::map<int, boost::shared_ptr<messagepacket>>::iterator itermap = mapPacket_.find(mapkey);
	if (itermap == mapPacket_.end())
		return;
	boost::shared_ptr<messagepacket> pmsgpacket = (*itermap).second;
	mapPacket_.erase(itermap);
	//messagepacket*pmsgpacket = (messagepacket*)cookie;
	pmsgpacket->pDBOperationReply->set_issuc(true);
	if (lcberr != LCB_SUCCESS){
		pmsgpacket->pDBOperationReply->set_issuc(false);
		pmsgpacket->pDBOperationReply->set_errdesc(lcb_strerror(db_instance_, lcberr));
	}
	else{
		pmsgpacket->pDBOperationReply->set_issuc(true);
		std::string path;
		path.assign((char*)resp->v.v0.path, resp->v.v0.npath);
		std::string header;
		//to debug...
		//value.assign((char*)resp->v.v0.headers, (char*)resp->v.v0.);
		std::string body;
		body.assign((char*)resp->v.v0.bytes, resp->v.v0.nbytes);

		pmsgpacket->pDBOperationReply->set_httpheaders(header);
		pmsgpacket->pDBOperationReply->set_httppath(path);
		pmsgpacket->pDBOperationReply->set_httpjsonresult(body);
	}
	if (pmsgpacket->callback_){
		io_->dispatch(boost::bind(pmsgpacket->callback_, pmsgpacket->pDBOperationReply));

	//	pmsgpacket->callback_(pmsgpacket->pDBOperationReply);
	}
}


void couchbaseSrv::processmessage(boost::shared_ptr<couchbasesrv::DBOperationReq> pMsg, const msgCallback&callback){
	messagedata*pNewMsgData = (messagedata*)message_handle.data;
	if (pNewMsgData){
		pNewMsgData->mutex_msgList_.lock();
		boost::shared_ptr<messagepacket> pmsgpacket = boost::make_shared<messagepacket>();
		pmsgpacket->pDBOperationReq = pMsg;
		pmsgpacket->pDBOperationReply = NULL;
		pmsgpacket->callback_ = callback;
		pNewMsgData->pMsgList->push_back(pmsgpacket);
		pNewMsgData->mutex_msgList_.unlock();
	}
	uv_async_send(&message_handle);

}