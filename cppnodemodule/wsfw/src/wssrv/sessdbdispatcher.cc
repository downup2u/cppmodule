#include "sessdbdispatcher.h"
#include "couchbasewrap.h"
#include "codec.h"
#include <jansson.h>

sessdbdispatcher::sessdbdispatcher(){
}

sessdbdispatcher::~sessdbdispatcher()
{

}

void sessdbdispatcher::registerCallback(boost::shared_ptr<ProtobufDispatcher> msgdispatcher){

	//initData();
	queryData();
	msgdispatcher->registerMessageCallback<xkcrm::PkgUserLoginReq>(boost::bind(&sessdbdispatcher::onmessage_login, this, _1, _2, _3, _4));
}

void sessdbdispatcher::onmessage_login(boost::shared_ptr<comminternal::PkgMsg> pMsgReq, boost::shared_ptr<xkcrm::PkgUserLoginReq> pReq, boost::shared_ptr<comminternal::PkgSessionUser> pSessionUsr, const SessionCallBack callback){
	std::string sPhoneNumber = pReq->phonenumber();
	std::string sPassword = pReq->password();
	boost::shared_ptr<comminternal::PkgMsg> pMsgReply = boost::make_shared<comminternal::PkgMsg>(*pMsgReq);
	//pMsgReply->
	boost::shared_ptr<couchbasesrv::DBOperationReq> pDBOperationReq = boost::make_shared<couchbasesrv::DBOperationReq>();
	pDBOperationReq->set_enlcboperation(couchbasesrv::lcb_httprequest);
	pDBOperationReq->set_enhttpmethod(couchbasesrv::DBOperationReq::LCB_HTTP_METHOD_GET);
	pDBOperationReq->set_enreqtype(couchbasesrv::DBOperationReq::LCB_HTTP_TYPE_VIEW);
	std::stringstream ss;
	ss << "/_design/dev_user/_view/loginbyphone?keys=[\"" << sPhoneNumber << "\",\"" << sPassword << "\"]";
	std::string path = ss.str();
	pDBOperationReq->set_path(path);
	couchbasewrap::get_mutable_instance().processmessage(pDBOperationReq, [=](boost::shared_ptr<couchbasesrv::DBOperationReply> pDBOperationReply){
		std::string sJsonResult = codecutil::getJsonFromMsg(pDBOperationReply);
		std::cout << sJsonResult << std::endl;
	});
	callback(pMsgReply);
}

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

std::string sessdbdispatcher::getCouchbaseResult(const std::string&sJson){
	std::string sJsonValue;
	size_t total_rows = 0;
	json_t *root;
	json_error_t error;
	root = json_loads(sJson.c_str(), 0, &error);
	if (root) {
		boost::shared_ptr<json_t> proot(root, [](json_t *p){ if (p){ json_decref(p); }});
		boost::shared_ptr<json_t> prootnew(json_object(), [](json_t *p){ if (p){ json_decref(p); }});
		size_t size;
		const char *keyroot;
		const char *keyrow;
		json_t *value;
		size = json_object_size(root);
		json_t *arrnew = json_array();
		json_object_set_new(prootnew.get(), "issuccess", json_true());
		json_object_set_new(prootnew.get(), "rows", arrnew);

		json_object_foreach(root, keyroot, value) {
			if (equals("total_rows", keyroot, is_iequal())){
				total_rows = (size_t)json_integer_value(value);				
			}
			else if (equals("rows", keyroot, is_iequal())){
				size_t size = json_array_size(value);
				for (size_t i = 0; i < size; i++){
					json_t * row = json_array_get(value, i);
					json_t *docvalue;
					json_object_foreach(row, keyrow, docvalue){
						if (equals("value", keyrow, is_iequal())){
							//docvalue->string
							
							json_array_append_new(arrnew, json_copy(docvalue));
							//break;
						}
						else if (equals("id", keyrow, is_iequal())){
							//docvalue->string
							std::string id = json_dumps(docvalue, JSON_ENCODE_ANY);
						}
						else if (equals("key", keyrow, is_iequal())){
							//docvalue->string
							std::string key = json_dumps(docvalue, JSON_ENCODE_ANY);
						}

					}
				}
				//break;
			}
		}
		boost::shared_ptr<char> psvalue(json_dumps(prootnew.get(), JSON_ENCODE_ANY), [](char*p){if (p){ free(p); } });

		sJsonValue = psvalue.get();
		
	}
	return sJsonValue;
}

void sessdbdispatcher::queryData(){
	std::string sPhoneNumber ="15961125167";
	std::string sPassword = "123456";
	//pMsgReply->
	boost::shared_ptr<couchbasesrv::DBOperationReq> pDBOperationReq = boost::make_shared<couchbasesrv::DBOperationReq>();
	pDBOperationReq->set_enlcboperation(couchbasesrv::lcb_httprequest);
	pDBOperationReq->set_enhttpmethod(couchbasesrv::DBOperationReq::LCB_HTTP_METHOD_GET);
	pDBOperationReq->set_enreqtype(couchbasesrv::DBOperationReq::LCB_HTTP_TYPE_VIEW);
	pDBOperationReq->set_contenttype("application/json");
	std::stringstream ss;
	ss << "_design/dev_user/_view/loginbyphone?key=[\"" << sPhoneNumber << "\",\"" << sPassword << "\"]";
	std::string path = ss.str();
	pDBOperationReq->set_path(path);
	couchbasewrap::get_mutable_instance().processmessage(pDBOperationReq, [=](boost::shared_ptr<couchbasesrv::DBOperationReply> pDBOperationReply){
		std::string sJsonResult = codecutil::getJsonFromMsg(pDBOperationReply);
		std::cout << sJsonResult << std::endl;

		if (pDBOperationReply->issuc()){
			sJsonResult = getCouchbaseResult(pDBOperationReply->httpjsonresult());
			
			std::cout << sJsonResult << std::endl;
		}
	});

}
void sessdbdispatcher::initData(){
	//insert user
	std::string useruuid = codecutil::generateDocumentuuid("xkcrm_res_user_");
	boost::shared_ptr<xkcrm::db_res_users> pDBDocumentUserInfo = boost::make_shared<xkcrm::db_res_users>();
	pDBDocumentUserInfo->set_uuiduserid(useruuid);
	pDBDocumentUserInfo->set_realname("wangxiaoqing");
	pDBDocumentUserInfo->set_type("res_user");
	pDBDocumentUserInfo->add_channel("channel_" + useruuid);
	pDBDocumentUserInfo->set_phonenumber("15961125167");
	pDBDocumentUserInfo->set_hashedpassword("123456");
	std::string docJson = codecutil::getJsonFromMsg(pDBDocumentUserInfo);
	
	boost::shared_ptr<couchbasesrv::DBOperationReq> pDBOperationReq = boost::make_shared<couchbasesrv::DBOperationReq>();
	pDBOperationReq->set_enlcboperation(couchbasesrv::lcb_store);
	pDBOperationReq->set_dockey(useruuid);
	pDBOperationReq->set_doctxt(docJson);
	pDBOperationReq->set_enlcbstorecmd(couchbasesrv::DBOperationReq::LCB_SET);
	
	couchbasewrap::get_mutable_instance().processmessage(pDBOperationReq, [=](boost::shared_ptr<couchbasesrv::DBOperationReply> pDBOperationReply){
		std::string sJsonResult = codecutil::getJsonFromMsg(pDBOperationReply);
		std::cout << sJsonResult << std::endl;
	});

	//insert company
	std::string companyuuid = codecutil::generateDocumentuuid("xkcrm_res_company_");
	boost::shared_ptr<xkcrm::db_res_company> pDBDocumentCompanyInfo = boost::make_shared<xkcrm::db_res_company>();
	pDBDocumentCompanyInfo->set_uuidcompanyid(companyuuid);
	pDBDocumentCompanyInfo->set_type("res_company");
	pDBDocumentCompanyInfo->add_channel("channel_" + companyuuid);
	pDBDocumentCompanyInfo->set_companyname("xkcompanyname");
	docJson = codecutil::getJsonFromMsg(pDBDocumentCompanyInfo);
	pDBOperationReq = boost::make_shared<couchbasesrv::DBOperationReq>();
	pDBOperationReq->set_enlcboperation(couchbasesrv::lcb_store);
	pDBOperationReq->set_dockey(companyuuid);
	pDBOperationReq->set_doctxt(docJson);
	pDBOperationReq->set_enlcbstorecmd(couchbasesrv::DBOperationReq::LCB_SET);

	couchbasewrap::get_mutable_instance().processmessage(pDBOperationReq, [=](boost::shared_ptr<couchbasesrv::DBOperationReply> pDBOperationReply){
		std::string sJsonResult = codecutil::getJsonFromMsg(pDBOperationReply);
		std::cout << sJsonResult << std::endl;
	});

}