#include "wssrvmain.h"
#include "couchbasewrap.h"

wssrvmain::wssrvmain(){
	
}

void wssrvmain::start(uint16_t port, const std::string& connstr, const std::string&bucketpassword /*= ""*/) {
	wssrv_ = boost::make_shared<wssrv>(io_);
	couchbasewrap::get_mutable_instance().startSrv(io_, connstr, bucketpassword);
	workPtr_ = boost::make_shared<boost::asio::io_service::work>(io_);
	wssrv_->run(9002);
	t_ = boost::make_shared<boost::thread>(boost::bind(&boost::asio::io_service::run, &io_));

}

void wssrvmain::stop(){
	if (wssrv_){
		wssrv_->stop();
		io_.stop();
		t_->join();
	}
}