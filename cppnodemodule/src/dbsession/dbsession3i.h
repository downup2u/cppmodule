#ifndef _DBSESSION3I_INCLUDE
#define _DBSESSION3I_INCLUDE
#include <string>
#include "../mylib/util/utiltime.h"
#include "../mylib/db/mysqldbutil.h"
#include "../gcomm/thirdinterface.pb.h"
#include "../gcomm/session.pb.h"
#include <encryptstring.h>

template<> struct soci::type_conversion<thirdinterface::PkgGetPasswordTable>
{
	typedef soci::values base_type;

	static void from_base(soci::values const &v, soci::indicator /* ind */, thirdinterface::PkgGetPasswordTable &t)
	{
		// here we ignore the possibility the the whole object might be NULL
		std::string getpassword_phonenumber_decoded;
		std::string getpassword_emailaddress_decoded;

		t.set_id(v.get<int>("id"));
		tm t1 = { 0 };
		t1 = v.get<tm>("time", t1);
		t.set_time(UtilTime::convertToTimeString(t1));
		t.set_authtype(v.get<int>("authtype"));
		int nType = v.get<int>("type");
		t.set_type((thirdinterface::PkgGetPasswordTableReq::EnGetType)nType);
		decodeString(v.get<std::string>("phonenumber"), getpassword_phonenumber_decoded);
		t.set_phonenumber(getpassword_phonenumber_decoded);
		decodeString(v.get<std::string>("emailaddress"), getpassword_emailaddress_decoded);
		t.set_emailaddress(getpassword_emailaddress_decoded);
		int nStatus = v.get<int>("status");
		t.set_status((thirdinterface::PkgGetPasswordTableReq::EnStatus)nStatus);
		t.set_authcode(v.get<std::string>("authcode"));
	}

	//static void to_base(thirdinterface::PkgGetPasswordTable const &t, soci::values &v, soci::indicator &ind)
	//{
	//	tm tm1;
	//	UtilTime::convertStringToTime(t.time(), tm1);
	//	v.set("id", t.id());
	//	v.set("time", tm1);
	//	v.set("authtype", t.authtype());
	//	v.set("type", t.type());
	//	v.set("phonenumber", t.phonenumber());
	//	v.set("emailaddress", t.emailaddress());
	//	v.set("status", t.status());
	//	v.set("authcode", t.authcode());

	//	ind = i_ok;
	//}
};


	template<> struct soci::type_conversion<thirdinterface::PkgRegisterInvition>
	{
		typedef soci::values base_type;

		static void from_base(soci::values const &v, soci::indicator /* ind */, thirdinterface::PkgRegisterInvition &t)
		{
			// here we ignore the possibility the the whole object might be NULL
			std::string registerinvition_phonenumber_decoded;
			std::string registerinvition_emailaddress_decoded;
			std::string registerinvition_realname_decoded;

			t.set_id(v.get<int>("id"));
			tm t1 = { 0 };
			t1 = v.get<tm>("invitiontime", t1);
			t.set_invitiontime(UtilTime::convertToTimeString(t1));
			t.set_userid(v.get<int>("userid"));
			t.set_companyid(v.get<int>("companyid"));
			int nType = v.get<int>("type");
			t.set_type((thirdinterface::PkgRegisterInvitionReq::EnGetType)nType);
			decodeString(v.get<std::string>("phonenumber"), registerinvition_phonenumber_decoded);
			t.set_phonenumber(registerinvition_phonenumber_decoded);
			decodeString(v.get<std::string>("emailaddress"), registerinvition_emailaddress_decoded);
			t.set_emailaddress(registerinvition_emailaddress_decoded);
			int nStatus = v.get<int>("status");
			t.set_status((thirdinterface::PkgGetPasswordTableReq::EnStatus)nStatus);
			t.set_invitioncode(v.get<std::string>("invitioncode"));
			decodeString(v.get<std::string>("realname"), registerinvition_realname_decoded);
			t.set_torealname(registerinvition_realname_decoded);
		}

		//static void to_base(thirdinterface::PkgRegisterInvition const &t, soci::values &v, soci::indicator &ind)
		//{
		//	tm tm1;
		//	UtilTime::convertStringToTime(t.invitiontime(), tm1);
		//	v.set("id", t.id());
		//	v.set("invitiontime", tm1);
		//	v.set("companyid", t.companyid());
		//	v.set("userid", t.userid());
		//	v.set("type", t.type());
		//	v.set("phonenumber", t.phonenumber());
		//	v.set("emailaddress", t.emailaddress());
		//	v.set("status", t.status());
		//	v.set("invitioncode", t.invitioncode());
		//	v.set("realname", t.torealname());

		//	ind = i_ok;
		//}
	};
class DBSession3I
{
public:
	DBSession3I();
	~DBSession3I();
private:
	mysqlDBUtil dbUtil_;
public:
	bool initSession(size_t nPoolSize, const std::string& connectstring);
	bool exitSession(std::string&err);
public:

//=========================================================
public:
	bool handleThirdMsg_getGetPasswordTable(boost::shared_ptr<thirdinterface::PkgGetPasswordTableReq> pPkgGetPasswordTableReq, boost::shared_ptr<thirdinterface::PkgGetPasswordTableReply> pPkgGetPasswordTableReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleThirdMsg_setGetPasswordTableStatus(boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReq> pPkgSetGetPasswordTableStatusReq, boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReply> pPkgSetGetPasswordTableStatusReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleThirdMsg_getSysInfoList(boost::shared_ptr<thirdinterface::PkgSysInfoListGetReq> pPkgMsgInfoReq, boost::shared_ptr<thirdinterface::PkgSysInfoListGetReply> pPkgMsgInfoReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleThirdMsg_getRegisterInvitionReq(boost::shared_ptr<thirdinterface::PkgRegisterInvitionReq> pPkgRegisterInvitionReq, boost::shared_ptr<thirdinterface::PkgRegisterInvitionReply> pPkgRegisterInvitionReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);
	bool handleThirdMsg_setRegisterInvitionStatus(boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReq> pPkgSetRegisterInvitionStatusReq, boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReply> pPkgSetRegisterInvitionStatusReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser);

private:
	std::string getCompanyName(int companyid, soci::session& sql);
	std::string getUserName(int userid, soci::session& sql);
};

#endif