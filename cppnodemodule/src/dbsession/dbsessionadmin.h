#ifndef _DBSESSIONADMIN_INCLUDE
#define _DBSESSIONADMIN_INCLUDE
#include <string>
#include "../mylib/util/utiltime.h"
#include "../mylib/db/mysqldbutil.h"
#include "../gcomm/admin.pb.h"
#include "../gcomm/session.pb.h"
#include <encryptstring.h>

template<> struct soci::type_conversion<admin::PkgAdvInfo>
{
	typedef soci::values base_type;

	static void from_base(soci::values const &v, soci::indicator /* ind */, admin::PkgAdvInfo &t)
	{
		// here we ignore the possibility the the whole object might be NULL
		t.set_id(v.get<int>("id"));
		t.set_name(v.get<std::string>("name", ""));
		t.set_picpath(v.get<std::string>("picpath", ""));
		std::string advtext_decoded;
		std::string advdesc_decoded;
		decodeString_Hex(v.get<std::string>("advtxt", ""), advtext_decoded);
		decodeString_Hex(v.get<std::string>("advdesc", ""), advdesc_decoded);
		t.set_advtxt(advtext_decoded);
		t.set_advdesc(advdesc_decoded);
		tm t1 = { 0 };
		t1 = v.get<tm>("createtime", t1);
		t.set_createtime(UtilTime::convertToTimeString(t1));
		tm t2 = { 0 };
		t2 = v.get<tm>("expiretime", t2);
		t.set_expiretime(UtilTime::convertToTimeString(t2));
	}

	//static void to_base(admin::PkgAdvInfo const &t, soci::values &v, soci::indicator &ind)
	//{
	//	v.set("id", t.id());
	//	v.set("name", t.name());
	//	v.set("picpath", t.picpath());
	//	v.set("advtxt", t.advtxt());
	//	v.set("advdesc", t.advdesc());
	//	tm tm1 = { 0 };
	//	tm tm2 = { 0 };
	//	UtilTime::convertStringToTime(t.createtime(), tm1);
	//	v.set("createtime", tm1);
	//	UtilTime::convertStringToTime(t.expiretime(), tm2);
	//	v.set("expiretime", tm2);
	//	ind = i_ok;
	//}
};

class DBSessionAdmin
{
public:
	DBSessionAdmin();
	~DBSessionAdmin();
private:
	mysqlDBUtil dbUtil_;
public:
	bool initSession(size_t nPoolSize, const std::string& connectstring);
	bool exitSession(std::string&err);
public:
	bool handleAdminMsg_userLogin(boost::shared_ptr<admin::PkgUserLoginReq> pPkgUserLoginReq, boost::shared_ptr<admin::PkgUserLoginReply> pPkgUserLoginReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleAdminMsg_userOperation(boost::shared_ptr<admin::PkgUserOperationReq> pPkgUserOperationReq, boost::shared_ptr<admin::PkgUserOperationReply> pPkgUserOperationReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_advQuery(boost::shared_ptr<admin::PkgAdvQueryReq> pPkgAdvQueryReq, boost::shared_ptr<admin::PkgAdvQueryReply> pPkgAdvQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleMessage_advOperation(boost::shared_ptr<admin::PkgAdvOperationReq> pPkgAdvOperationReq, boost::shared_ptr<admin::PkgAdvOperationReply> pPkgAdvOperationReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleThirdMsg_setSysInfo(boost::shared_ptr<admin::PkgSysInfoSetReq> pPkgSysInfoReq, boost::shared_ptr<admin::PkgSysInfoSetReply> pPkgSysInfoSetReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleThirdMsg_getSysInfo(boost::shared_ptr<admin::PkgSysInfoGetReq> pPkgSysInfoReq, boost::shared_ptr<admin::PkgSysInfoGetReply> pPkgSysInfoGetReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
};

#endif