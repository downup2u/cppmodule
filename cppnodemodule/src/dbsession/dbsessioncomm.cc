#include "dbsessioncomm.h"
#include <sstream>
#include <boost-tuple.h>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include <set>
#include <boost/foreach.hpp>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include "../mylib/util/utillanguage.h"
#include <glog.h>
#include <boost/algorithm/string.hpp>
#include <boost/assert.hpp>
#include "../nodeexception.h"

//====================================



DBSessionComm::DBSessionComm()
{

}

DBSessionComm::~DBSessionComm()
{
}


bool DBSessionComm::initSession(size_t nPoolSize, const std::string& connectstring)
{
	std::string err;
	dbUtil_.initDB(nPoolSize, connectstring, err);

	mapRoles.clear();
	{//init data to cache
		soci::session sql(*dbUtil_.getConnection());
		std::string sSQLSelect;
		sSQLSelect = "SELECT * FROM permissionrole";
		::session::PkgRoleUser roleuser;
		soci::statement st = (sql.prepare << sSQLSelect, soci::into(roleuser));
		st.execute();
		while (st.fetch())
		{
			boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = boost::make_shared<::session::PkgRoleUser>(roleuser);
			mapRoles[roleuser.id()] = pPkgRoleUser;
		}

	}	
	return true;
}

void DBSessionComm::getUserMap(std::map<int, std::string>& mapUseRealname, soci::session& sql)
{
	//init mapUseRealname
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string> BTUPLE;
	sSQLSelect = "SELECT id,realname FROM user";
	std::string realname;
	int userid;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		realname = dbvalues.get<1>();
		std::string user_realname_decoded;
		decodeString(realname, user_realname_decoded);
		mapUseRealname[userid] = user_realname_decoded;
		it++;
	}
}

bool DBSessionComm::exitSession(std::string&err)
{
	dbUtil_.exitDB();
	return true;
}



bool DBSessionComm::handleMessage_userCheck(boost::shared_ptr<comm::PkgUserCheckReq> pPkgUserCheckReq, boost::shared_ptr<comm::PkgUserCheckReply> pPkgUserCheckReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{

	bool bSuc = false;
	try
	{
		std::string sqlWhereCondition;
		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;
		encodeString(pPkgUserCheckReq->phonenumber(), user_phonenumber_encoded);
		encodeString(pPkgUserCheckReq->emailaddress(), user_emailaddress_encoded);

		switch (pPkgUserCheckReq->gettype())
		{
		case comm::GT_PHONE:
			sqlWhereCondition = "phonenumber = '" + user_phonenumber_encoded + "'";
			break;
		case comm::GT_EMAIL:
			sqlWhereCondition = "emailaddress = '" + user_emailaddress_encoded + "'";
			break;
		case comm::GT_USERNAME:
			sqlWhereCondition = "username = '" + pPkgUserCheckReq->username() + "'";
			break;
		default:
			pPkgUserCheckReply->set_issuccess(false);
			pPkgUserCheckReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return false;
		}


		soci::session sql(*dbUtil_.getConnection());
		std::string sSQLSelect = "SELECT id FROM user WHERE " + sqlWhereCondition;
		int id;
		soci::indicator ind(soci::indicator::i_null);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCheck),SQL:" << sSQLSelect ;
		sql << sSQLSelect, into(id, ind);

		if (ind == soci::indicator::i_ok)
		{
			pPkgUserCheckReply->set_issuccess(false);
			pPkgUserCheckReply->set_userid(id);
			pPkgUserCheckReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.userexists"));
			return false;
		}
		else
		{
			pPkgUserCheckReply->set_issuccess(true);
		}

	}
	catch (soci::soci_error &err)
	{
		pPkgUserCheckReply->set_issuccess(false);
		pPkgUserCheckReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserCheckReply->set_issuccess(false);
		pPkgUserCheckReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return bSuc;
}

bool DBSessionComm::handleMessage_userGetPassword(boost::shared_ptr<comm::PkgUserGetPasswordReq> pPkgUserGetPasswordReq, boost::shared_ptr<comm::PkgUserGetPasswordReply> pPkgUserGetPasswordReply,
	boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, ::session::EnAuthType authtype/* = ::session::EA_FINDPASSWORD*/)
{//找回密码
	bool bSuc = false;
	try
	{
		std::string sAuthCode = generateAuthCode();
		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;
		encodeString(pPkgUserGetPasswordReq->phonenumber(), user_phonenumber_encoded);
		encodeString(pPkgUserGetPasswordReq->emailaddress(), user_emailaddress_encoded);

		std::string sSQLInsert;
		soci::session sql(*dbUtil_.getConnection());
		std::string sqlWhereCondition;
		switch (pPkgUserGetPasswordReq->gettype())
		{
		case comm::GT_PHONE:
			sqlWhereCondition = "phonenumber = '" + user_phonenumber_encoded + "'";
			break;
		case comm::GT_EMAIL:
			sqlWhereCondition = "emailaddress = '" + user_emailaddress_encoded + "'";
			break;
		default:
			pPkgUserGetPasswordReply->set_issuccess(false);
			pPkgUserGetPasswordReply->set_err("data error");
			return bSuc;
		}

		std::string sSQLSelect = "SELECT id FROM user WHERE " + sqlWhereCondition;
		int id = 0;
		soci::indicator ind(soci::indicator::i_null);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userGetPassword),SQL:" << sSQLSelect ;
		sql << sSQLSelect, into(id, ind);

		if (authtype == ::session::EA_REGISTERAUTH)
		{//新注册
			if (ind == soci::indicator::i_ok)
			{
				pPkgUserGetPasswordReply->set_issuccess(false);
				pPkgUserGetPasswordReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.userexists"));
				return true;
			}
		}
		else if (authtype == ::session::EA_FINDPASSWORD)
		{//找回密码
			if (ind != soci::indicator::i_ok)
			{
				pPkgUserGetPasswordReply->set_issuccess(false);
				pPkgUserGetPasswordReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.usernotexists"));
				return true;
			}
		}
		std::string stime = UtilTime::getCurrentTime();
		std::string sSQLDelete = "DELETE FROM getpassword WHERE " + sqlWhereCondition;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userGetPassword),SQL:" << sSQLDelete ;
		sql << sSQLDelete;//以前的验证码无效

		//插入验证码
		sSQLInsert = "INSERT INTO getpassword(time,phonenumber,emailaddress,type,status,authcode,authtype) VALUES('" +
			stime + "','" + user_phonenumber_encoded + "','" + user_emailaddress_encoded + "'," + boost::lexical_cast<std::string>(pPkgUserGetPasswordReq->gettype()) + ",0,'"
			+ sAuthCode + "'," + boost::lexical_cast<std::string>(authtype)+")";
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userGetPassword),SQL:" << sSQLInsert ;
		sql << sSQLInsert;
		pPkgUserGetPasswordReply->set_issuccess(true);


	}
	catch (soci::soci_error &err)
	{
		pPkgUserGetPasswordReply->set_issuccess(false);
		pPkgUserGetPasswordReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserGetPasswordReply->set_issuccess(false);
		pPkgUserGetPasswordReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return bSuc;
}


bool DBSessionComm::handleMessage_userGetAuth(boost::shared_ptr<comm::PkgUserGetAuthReq> pPkgUserGetAuthReq, boost::shared_ptr<comm::PkgUserGetAuthReply> pPkgUserGetAuthReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//注册
	bool bSuc = false;
	try
	{
		//先判断是否已经注册
		comm::EnGetType type = pPkgUserGetAuthReq->gettype();
		boost::shared_ptr<comm::PkgUserGetPasswordReq> pPkgUserGetPasswordReq = boost::make_shared<comm::PkgUserGetPasswordReq>();
		pPkgUserGetPasswordReq->set_gettype(type);
		boost::shared_ptr<comm::PkgUserCheckReq> pPkgUserCheckReq = boost::make_shared<comm::PkgUserCheckReq>();
		pPkgUserCheckReq->set_gettype(type);
		switch (type)
		{
		case comm::GT_PHONE:
			pPkgUserCheckReq->set_phonenumber(pPkgUserGetAuthReq->phonenumber());
			pPkgUserGetPasswordReq->set_phonenumber(pPkgUserGetAuthReq->phonenumber());
			break;
		case comm::GT_EMAIL:
			pPkgUserCheckReq->set_emailaddress(pPkgUserGetAuthReq->emailaddress());
			pPkgUserGetPasswordReq->set_emailaddress(pPkgUserGetAuthReq->emailaddress());
			break;
		default:
			pPkgUserGetAuthReply->set_issuccess(false);
			pPkgUserGetAuthReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return false;
		}
		pPkgUserCheckReq->set_gettype(type);
		boost::shared_ptr<comm::PkgUserCheckReply> pPkgUserCheckReply = boost::make_shared<comm::PkgUserCheckReply>();
		handleMessage_userCheck(pPkgUserCheckReq, pPkgUserCheckReply, pPkgSessionUser);
		if (!pPkgUserCheckReply->issuccess())
		{//已经注册
			pPkgUserGetAuthReply->set_issuccess(false);
			pPkgUserGetAuthReply->set_err(pPkgUserCheckReply->err());
			return false;
		}
		boost::shared_ptr<comm::PkgUserGetPasswordReply> pPkgUserGetPasswordReply = boost::make_shared<comm::PkgUserGetPasswordReply>();
		handleMessage_userGetPassword(pPkgUserGetPasswordReq, pPkgUserGetPasswordReply, pPkgSessionUser, ::session::EA_REGISTERAUTH);
		//获取验证码
		if (!pPkgUserGetPasswordReply->issuccess())
		{
			pPkgUserGetAuthReply->set_issuccess(false);
			pPkgUserGetAuthReply->set_err(pPkgUserGetPasswordReply->err());
			return false;
		}
		pPkgUserGetAuthReply->set_issuccess(true);

	}
	catch (soci::soci_error &err)
	{
		pPkgUserGetAuthReply->set_issuccess(false);
		pPkgUserGetAuthReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserGetAuthReply->set_issuccess(false);
		pPkgUserGetAuthReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return bSuc;
}
comm::PkgCompanyInfo*DBSessionComm::getCompanyInfo(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser){
	soci::session sql(*dbUtil_.getConnection());
	return getCompanyInfo(pPkgSessionUser,sql);
}


bool DBSessionComm::handleMessage_userLogin(boost::shared_ptr<comm::PkgUserLoginReq> pPkgUserLoginReq, boost::shared_ptr<comm::PkgUserLoginReply> pPkgUserLoginReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	try
	{
		std::string user_password_encoded;
		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;

		std::string sqlWhereCondition;		
		switch (pPkgUserLoginReq->gettype())
		{
		case comm::GT_PHONE:
			encodeString(pPkgUserLoginReq->phonenumber(), user_phonenumber_encoded);
			sqlWhereCondition = "phonenumber = '" + user_phonenumber_encoded + "'";
			break;
		case comm::GT_EMAIL:
			encodeString(pPkgUserLoginReq->emailaddress(), user_emailaddress_encoded);
			sqlWhereCondition = "emailaddress = '" + user_emailaddress_encoded + "'";
			break;
		case comm::GT_USERID:
			sqlWhereCondition = "id = " + boost::lexical_cast<std::string>(pPkgUserLoginReq->userid());
			break;
		case comm::GT_USERNAME:
			sqlWhereCondition = "username = '" + pPkgUserLoginReq->username() + "'";
			break;
		default:
			pPkgUserLoginReply->set_issuccess(false);
			pPkgUserLoginReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return false;
		}

		::session::PkgUser pkgUser;
		soci::session sql(*dbUtil_.getConnection());
		std::string sSQLSelect = "SELECT * FROM user WHERE " + sqlWhereCondition;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userLogin),SQL:" << sSQLSelect ;
		soci::statement st = (sql.prepare << sSQLSelect, soci::into(pkgUser));
		st.execute();
		if (st.fetch())
		{		
			if (pkgUser.password() == pPkgUserLoginReq->password())
			{
				pPkgUserLoginReply->set_issuccess(true);
				pPkgUserLoginReply->set_userid(pkgUser.userid());
				pPkgUserLoginReply->set_phonenumber(pkgUser.phonenumber());
				pPkgUserLoginReply->set_emailaddress(pkgUser.emailaddress());
				pPkgUserLoginReply->set_username(pkgUser.username());
				pPkgUserLoginReply->set_realname(pkgUser.realname());
				pPkgUserLoginReply->set_companyid(pkgUser.companyid());
				pPkgUserLoginReply->set_createtime(pkgUser.createtime());

				boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = mapRoles[pkgUser.permissionroleid()];
				if (pPkgRoleUser)
				{
					pPkgUserLoginReply->set_rolename(pPkgRoleUser->rolename());
					pPkgUserLoginReply->set_permission(pPkgRoleUser->permission());
					pPkgUserLoginReply->set_permissionroleid(pkgUser.permissionroleid());
				}
				pPkgSessionUser->set_userid(pkgUser.userid());
				::session::PkgUser*pUser = new ::session::PkgUser(pkgUser);
				pPkgSessionUser->set_allocated_user(pUser);

				comm::PkgCompanyInfo*pPkgCompanyInfo = getCompanyInfo(pPkgSessionUser,sql);
				pPkgUserLoginReply->set_allocated_companyinfo(pPkgCompanyInfo);
				
				pPkgUserLoginReply->set_companyname(pPkgCompanyInfo->companyname());
				pPkgUserLoginReply->set_companycreatetime(pPkgCompanyInfo->companycreatetime());
				pPkgUserLoginReply->set_companyusernumber(pPkgCompanyInfo->companyusernumber());
				
				//SET ...
				bSuc = true;
			
			}
			else
			{//error
				pPkgUserLoginReply->set_issuccess(false);
				pPkgUserLoginReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongpassword"));
			}
		}
		else
		{
			pPkgUserLoginReply->set_issuccess(false);
			pPkgUserLoginReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.usernotexists"));
		}

	}
	catch (soci::soci_error &err)
	{
		pPkgUserLoginReply->set_issuccess(false);
		pPkgUserLoginReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserLoginReply->set_issuccess(false);
		pPkgUserLoginReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return bSuc;
}

comm::PkgCompanyInfo*DBSessionComm::getCompanyInfo(int companyid, soci::session& sql)
{
	comm::PkgCompanyInfo*pPkgCompanyInfo = new comm::PkgCompanyInfo();
	pPkgCompanyInfo->set_companyid(companyid);
	if (companyid > 0)
	{//CompanyName...					
		std::string sSQLSelect = "SELECT name,createtime FROM company WHERE id = " + boost::lexical_cast<std::string>(companyid);
		std::string companyname;
		std::string companycreatetime;

		typedef boost::tuple<std::string, tm> BTUPLE;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getCompanyInfo),SQL:" << sSQLSelect;


		soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
		soci::rowset<BTUPLE>::const_iterator it = rows.begin();
		if (it != rows.end())
		{
			const BTUPLE& dbvalues = *it;
			companyname = dbvalues.get<0>();
			tm tmcompanycreatetime = dbvalues.get<1>();
			std::string companycreatetime = UtilTime::convertToTimeString(tmcompanycreatetime);
			std::string comany_name_decoded;
			decodeString(companyname, comany_name_decoded);
			pPkgCompanyInfo->set_companyname(comany_name_decoded);
			pPkgCompanyInfo->set_companycreatetime(companycreatetime);

		}
	}

	if (companyid > 0)
	{
		//成员个数
		std::string sSQLSelect = "SELECT COUNT(id) FROM user WHERE companyid = " + boost::lexical_cast<std::string>(companyid);
		int usernumber = 0;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getCompanyInfo),SQL:" << sSQLSelect;
		soci::indicator ind(soci::indicator::i_null);

		sql << sSQLSelect, into(usernumber, ind);
		if (ind == soci::indicator::i_ok)
		{
			pPkgCompanyInfo->set_companyusernumber(usernumber);
		}

	}

	return pPkgCompanyInfo;
}

boost::shared_ptr<::session::PkgRoleUser> DBSessionComm::getRoles(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = mapRoles[pPkgSessionUser->user().permissionroleid()];
	return pPkgRoleUser;
}

comm::PkgCompanyInfo*DBSessionComm::getCompanyInfo(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, soci::session& sql)
{
	int companyid = 0;
	{
		std::string sSQLSelect = "SELECT companyid FROM user WHERE id = " + boost::lexical_cast<std::string>(pPkgSessionUser->user().userid());
		soci::indicator ind(soci::indicator::i_null);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getCompanyInfo),SQL:" << sSQLSelect;
		sql << sSQLSelect, into(companyid, ind);

		pPkgSessionUser->mutable_user()->set_companyid(companyid);
	}	
	comm::PkgCompanyInfo*pPkgCompanyInfo = getCompanyInfo(companyid, sql);
	pPkgSessionUser->mutable_user()->set_companyname(pPkgCompanyInfo->companyname());

	return pPkgCompanyInfo;
}

bool DBSessionComm::handleMessage_userCreateUser(boost::shared_ptr<comm::PkgUserCreateReq> pPkgUserCreateReq, boost::shared_ptr<comm::PkgUserCreateReply> pPkgUserCreateReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	try
	{
		bool notifyOthers = false;
		std::string sSQLInsert;
		soci::session sql(*dbUtil_.getConnection());
		comm::EnGetType type = pPkgUserCreateReq->gettype();
		std::string password;
		std::string curTime = UtilTime::getCurrentTime();
		int companyid = 0;
		std::string realname;
		std::string user_password_encoded;
		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;
		encodeString(pPkgUserCreateReq->phonenumber(), user_phonenumber_encoded);
		encodeString(pPkgUserCreateReq->emailaddress(), user_emailaddress_encoded);

		if (!pPkgUserCreateReq->authcode().empty())
		{//含有验证码
			//判断验证吗是否合法
			std::string sSQLSelect;
			switch (type)
			{
			case comm::GT_PHONE:
				sSQLSelect = "SELECT authcode FROM getpassword WHERE type = " + boost::lexical_cast<std::string>(pPkgUserCreateReq->gettype()) + " AND phonenumber = '" + user_phonenumber_encoded + "'";
				break;
			case comm::GT_EMAIL:
				sSQLSelect = "SELECT authcode FROM getpassword WHERE type = " + boost::lexical_cast<std::string>(pPkgUserCreateReq->gettype()) + " AND emailaddress = '" + user_emailaddress_encoded + "'";
				break;
			default:
				pPkgUserCreateReply->set_issuccess(false);
				pPkgUserCreateReply->set_err("data err");
				return bSuc;
			}			
			std::string authcode;
			soci::indicator ind(soci::indicator::i_null);
			BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateUser),SQL:" << sSQLSelect ;
			sql << sSQLSelect, into(authcode, ind);

			if (ind != soci::indicator::i_ok)
			{
				pPkgUserCreateReply->set_issuccess(false);
				pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.authcodenotexists"));
				return false;
			}

			if (authcode != pPkgUserCreateReq->authcode())
			{
				pPkgUserCreateReply->set_issuccess(false);
				pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongauthcode"));
				return false;
			}
			password = pPkgUserCreateReq->authcode();
		}

		if (!pPkgUserCreateReq->invitioncode().empty())
		{//含有邀请码
			if (!password.empty())
			{//同时含有邀请码和验证码，非法
				pPkgUserCreateReply->set_issuccess(false);
				pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
				return false;
			}
			//判断邀请码是否合法
			std::string sSQLSelect;
			switch (type)
			{
			case comm::GT_PHONE:
				sSQLSelect = "SELECT companyid,realname FROM registerinvition WHERE type = " 
					+ boost::lexical_cast<std::string>(pPkgUserCreateReq->gettype()) + " AND phonenumber = '" 
					+ user_phonenumber_encoded + "' AND invitioncode = '" + pPkgUserCreateReq->invitioncode() + "'";
				break;
			case comm::GT_EMAIL:
				sSQLSelect = "SELECT companyid,realname FROM registerinvition WHERE type = " 
					+ boost::lexical_cast<std::string>(pPkgUserCreateReq->gettype()) + " AND emailaddress = '"
					+ user_emailaddress_encoded + "' AND invitioncode = '" + pPkgUserCreateReq->invitioncode() + "'";
				break;
			default:
				pPkgUserCreateReply->set_issuccess(false);
				pPkgUserCreateReply->set_err("data err");
				return bSuc;
			}		
			//取得邀请码联系的数据
			typedef boost::tuple<int, std::string> BTUPLE;
			BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateUser),SQL:" << sSQLSelect ;
			soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
			soci::rowset<BTUPLE>::const_iterator it = rows.begin();
			if (it != rows.end())
			{
				const BTUPLE& dbvalues = *it;
				companyid = dbvalues.get<0>();
				realname = dbvalues.get<1>();
				std::string registerinvition_realname_decoded;
				decodeString(realname, registerinvition_realname_decoded);
				pPkgUserCreateReply->set_realname(registerinvition_realname_decoded);
			}
			else{
				pPkgUserCreateReply->set_issuccess(false);
				pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wronginvitioncode"));
				return false;
			}
			password = pPkgUserCreateReq->invitioncode();
			notifyOthers = true;
		}

		if (password.empty())
		{
			pPkgUserCreateReply->set_issuccess(false);
			pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return false;
		}

		std::string sqlWhereCondition;
		switch (type)
		{
		case comm::GT_PHONE:
			encodeString(password, user_password_encoded);
			sqlWhereCondition = "phonenumber = '" + user_phonenumber_encoded + "'";
			sSQLInsert = "INSERT INTO user(phonenumber,password,companyid,createtime) VALUES('" + user_phonenumber_encoded + "','" + user_password_encoded + "',"
				+ boost::lexical_cast<std::string>(companyid)+",'" + curTime + "')";
			break;
		case comm::GT_EMAIL:
			encodeString(password, user_password_encoded);
			sqlWhereCondition = "emailaddress = '" + user_emailaddress_encoded + "'";
			sSQLInsert = "INSERT INTO user(emailaddress,password,companyid,createtime) VALUES('" + user_emailaddress_encoded + "','" + user_password_encoded + "',"
				+ boost::lexical_cast<std::string>(companyid)+",'" + curTime + "')";
			break;
		default:
			pPkgUserCreateReply->set_issuccess(false);
			pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return bSuc;
		}
		std::string sSQLSelect = "SELECT id FROM user WHERE " + sqlWhereCondition;
		int userid = 0;
		soci::indicator ind(soci::indicator::i_null);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateUser),SQL:" << sSQLSelect ;
		sql << sSQLSelect, into(userid, ind);
		if (ind == soci::indicator::i_ok)
		{//已经存在该用户
			pPkgUserCreateReply->set_issuccess(false);
			pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.userexists"));
			return false;
		}

		//插入该用户
		ind = soci::indicator::i_null;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateUser),SQL:" << sSQLInsert ;
		sql << sSQLInsert;

		//获取插入的userid
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateUser),SQL:" << sSQLSelect ;
		sql << sSQLSelect, into(userid, ind);
		if (ind == soci::indicator::i_ok)
		{
			pPkgUserCreateReply->set_issuccess(true);
			pPkgUserCreateReply->set_userid(userid);
			bSuc = true;
			if (notifyOthers){
				//通知组织所有人
				std::vector<::session::PkgUser> vectUsers;
				getCompanyUsers(companyid, vectUsers,sql);

				//个人加入组织需通知所有其他成员
				{
					std::vector<int> vectNotifyUserIDs;
					BOOST_FOREACH(::session::PkgUser&u, vectUsers){
						vectNotifyUserIDs.push_back(u.userid());
					}
					updateUserUpdateFlags(vectNotifyUserIDs, comm::EnUpdatedFlag::UF_COMPANYINFO,sql);
				}

			}
		}
		else
		{
			pPkgUserCreateReply->set_issuccess(false);
			pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.inserterror"));
		}

	

	}
	catch (soci::soci_error &err)
	{
		pPkgUserCreateReply->set_issuccess(false);
		pPkgUserCreateReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserCreateReply->set_issuccess(false);
		pPkgUserCreateReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return bSuc;
}


bool DBSessionComm::handleMessage_userResetPassword(boost::shared_ptr<comm::PkgUserResetPasswordReq> pPkgUserResetPasswordReq, boost::shared_ptr<comm::PkgUserResetPasswordReply> pPkgUserResetPasswordReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	try
	{
		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;

		std::string sSQLInsert;
		soci::session sql(*dbUtil_.getConnection());
		std::string sqlWhereCondition;
		switch (pPkgUserResetPasswordReq->gettype())
		{
		case comm::GT_PHONE:
			encodeString(pPkgUserResetPasswordReq->phonenumber(), user_phonenumber_encoded);
			sqlWhereCondition = "phonenumber = '" + user_phonenumber_encoded + "'";
			break;
		case comm::GT_EMAIL:
			encodeString(pPkgUserResetPasswordReq->emailaddress(), user_emailaddress_encoded);
			sqlWhereCondition = "emailaddress = '" + user_emailaddress_encoded + "'";
			break;
		case comm::GT_USERID:
			sqlWhereCondition = "id = " + boost::lexical_cast<std::string>(pPkgUserResetPasswordReq->userid());
			break;
		case comm::GT_USERNAME:
			sqlWhereCondition = "username = '" + pPkgUserResetPasswordReq->username() + "'";
			break;
		default:
			pPkgUserResetPasswordReply->set_issuccess(false);
			pPkgUserResetPasswordReply->set_err("data error");
			return bSuc;
		}
		std::string sSQLSelect = "SELECT id FROM user WHERE " + sqlWhereCondition;
		int id;
		soci::indicator ind(soci::indicator::i_null);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userResetPassword),SQL:" << sSQLSelect ;
		sql << sSQLSelect, into(id, ind);

		if (ind == soci::indicator::i_ok)
		{
			ind = soci::indicator::i_null;
			//getpassword表
			std::string sqlWhereConditionGetPassword;
			if (pPkgUserResetPasswordReq->gettype() == comm::GT_PHONE)
			{
				sqlWhereConditionGetPassword = "AND phonenumber = '" + user_phonenumber_encoded + "'";
			}
			else if (pPkgUserResetPasswordReq->gettype() == comm::GT_PHONE)
			{
				sqlWhereConditionGetPassword = "AND emailaddress = '" + user_emailaddress_encoded + "'";
			}
	
			sSQLSelect = "SELECT id FROM getpassword WHERE authcode = '" + pPkgUserResetPasswordReq->authcode() + "' " + sqlWhereConditionGetPassword;
		//	sSQLSelect += " AND status = 2"; 测试模式不需要
			BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userResetPassword),SQL:" << sSQLSelect ;
			sql << sSQLSelect, into(id, ind);//看是否能找到

			if (ind == soci::indicator::i_ok)
			{
				ind = soci::indicator::i_null;
				std::string soldpassword;
				sSQLSelect = "SELECT password FROM user WHERE " + sqlWhereCondition;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userResetPassword),SQL:" << sSQLSelect ;
				sql << sSQLSelect, into(soldpassword, ind);
				std::string user_password_decoded_soldpassword;
				decodeString(soldpassword, user_password_decoded_soldpassword);
				if (ind == soci::indicator::i_ok)
				{
					pPkgUserResetPasswordReply->set_oldpassword(user_password_decoded_soldpassword);
					pPkgUserResetPasswordReply->set_issuccess(true);
				}
			}
			else
			{
				pPkgUserResetPasswordReply->set_issuccess(false);
				pPkgUserResetPasswordReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongauthcode"));
			}
		}
		else
		{
			pPkgUserResetPasswordReply->set_issuccess(false);
			pPkgUserResetPasswordReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.usernotexists"));
		}

	}
	catch (soci::soci_error &err)
	{
		pPkgUserResetPasswordReply->set_issuccess(false);
		pPkgUserResetPasswordReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserResetPasswordReply->set_issuccess(false);
		pPkgUserResetPasswordReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return bSuc;

}

bool DBSessionComm::handleMessage_userSet(boost::shared_ptr<comm::PkgUserSetReq> pPkgUserSetReq, boost::shared_ptr<comm::PkgUserSetReply> pPkgUserSetReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	try
	{
		int userid = 0;
		std::string sSQLSelect;
		soci::session sql(*dbUtil_.getConnection());
		std::string sqlWhereCondition;
		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;
		switch (pPkgUserSetReq->gettype())
		{
		case comm::GT_PHONE:
			encodeString(pPkgUserSetReq->phonenumber(), user_phonenumber_encoded);
			sqlWhereCondition = "phonenumber = '" + user_phonenumber_encoded + "'";
			break;
		case comm::GT_EMAIL:
			encodeString(pPkgUserSetReq->emailaddress(), user_emailaddress_encoded);
			sqlWhereCondition = "emailaddress = '" + user_emailaddress_encoded + "'";
			break;
		case comm::GT_USERID:
			sqlWhereCondition = "id = " + boost::lexical_cast<std::string>(pPkgUserSetReq->userid());
			break;
		case comm::GT_USERNAME:
			sqlWhereCondition = "username = '" + pPkgUserSetReq->username() + "'";
			break;
		default:
			pPkgUserSetReply->set_issuccess(false);
			pPkgUserSetReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return false;
		}
		typedef boost::tuple<int, std::string> BTUPLE;
		sSQLSelect = "SELECT id,password FROM user WHERE " + sqlWhereCondition;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userSet),SQL:" << sSQLSelect ;

		std::string user_password_decode;
		soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
		soci::rowset<BTUPLE>::const_iterator it = rows.begin();	
		if (it != rows.end())
		{
			const BTUPLE& dbvalues = *it;
			userid = dbvalues.get<0>();
			std::string password = dbvalues.get<1>();
			decodeString(password, user_password_decode);
		}
		else
		{
			pPkgUserSetReply->set_issuccess(false);
			pPkgUserSetReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.usernotexists"));
			return true;
		}

		if ((pPkgUserSetReq->settype() & 0x0F) == 0)
		{
			pPkgUserSetReply->set_issuccess(false);
			pPkgUserSetReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return true;
		}	

		if (user_password_decode != pPkgUserSetReq->password())
		{
			pPkgUserSetReply->set_issuccess(false);
			pPkgUserSetReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongpassword"));
			return true;
		}
		pPkgUserSetReply->set_issuccess(true);
		std::vector<std::string> vectSetFields;
		std::string sqlSetField;
		if (pPkgUserSetReq->settype() & comm::PkgUserSetReq::ST_PHONE)
		{
			encodeString(pPkgUserSetReq->phonenumber(), user_phonenumber_encoded);
			sqlSetField = "phonenumber = '" + user_phonenumber_encoded + "'";
			vectSetFields.push_back(sqlSetField);
		}
		if (pPkgUserSetReq->settype() & comm::PkgUserSetReq::ST_EMAIL)
		{
			encodeString(pPkgUserSetReq->emailaddress(), user_emailaddress_encoded);
			sqlSetField = "emailaddress = '" + user_emailaddress_encoded + "'";
			vectSetFields.push_back(sqlSetField);
		}
		if (pPkgUserSetReq->settype() & comm::PkgUserSetReq::ST_USERNAME)
		{
			sqlSetField = "username = '" + pPkgUserSetReq->username() + "'";
			vectSetFields.push_back(sqlSetField);
		}
		if (pPkgUserSetReq->settype() & comm::PkgUserSetReq::ST_NEWPASSWORD)
		{
			std::string user_password_encoded;
			encodeString(pPkgUserSetReq->newpassword(), user_password_encoded);
			sqlSetField = "password = '" + user_password_encoded + "'";
			vectSetFields.push_back(sqlSetField);
		}
		if (pPkgUserSetReq->settype() & comm::PkgUserSetReq::ST_REALNAME)
		{
			std::string user_realname_encoded;
			encodeString(pPkgUserSetReq->realname(), user_realname_encoded);
			sqlSetField = "realname = '" + user_realname_encoded + "'";
			vectSetFields.push_back(sqlSetField);
		}

		std::string sSQLUpdate = "UPDATE user SET "; 
		for (size_t i = 0; i < vectSetFields.size(); i++)
		{
			sSQLUpdate += vectSetFields[i];
			if (i != vectSetFields.size() - 1)
			{
				sSQLUpdate += ",";
			}
		}		
		sSQLUpdate = sSQLUpdate + " WHERE id = " + boost::lexical_cast<std::string>(userid);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userSet),SQL:" << sSQLUpdate ;
		sql << sSQLUpdate;

	}
	catch (soci::soci_error &err)
	{
		pPkgUserSetReply->set_issuccess(false);
		pPkgUserSetReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserSetReply->set_issuccess(false);
		pPkgUserSetReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return bSuc;
}

bool DBSessionComm::handleMessage_userInvitionUser(boost::shared_ptr<comm::PkgUserInvitionUserReq> pPkgUserInvitionUserReq, boost::shared_ptr<comm::PkgUserInvitionUserReply> pPkgUserInvitionUserReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sInvitionCode = generateAuthCode();
	std::string stime = UtilTime::getCurrentTime();

	std::string registerinvition_realname_encoded;
	std::string registerinvition_phonenumber_encoded;
	std::string registerinvition_emailaddress_encoded;
	encodeString(pPkgUserInvitionUserReq->realname(), registerinvition_realname_encoded);
	encodeString(pPkgUserInvitionUserReq->emailaddress(), registerinvition_emailaddress_encoded);
	encodeString(pPkgUserInvitionUserReq->phonenumber(), registerinvition_phonenumber_encoded);
	//插入验证码
	std::string sSQLInsert;
	soci::session sql(*dbUtil_.getConnection());
	sSQLInsert = "INSERT INTO registerinvition(userid,realname,companyid,invitiontime,phonenumber,emailaddress,type,status,invitioncode) VALUES(" + 
		boost::lexical_cast<std::string>(pPkgSessionUser->userid()) + ",'" + registerinvition_realname_encoded
		+ "'," + getUserCompanyId(pPkgSessionUser) + ",'" + stime + "','" + registerinvition_phonenumber_encoded + "','"
		+ registerinvition_emailaddress_encoded + "'," + boost::lexical_cast<std::string>(pPkgUserInvitionUserReq->gettype()) + ",0,'" + sInvitionCode + "')";
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userInvitionUser),SQL:" << sSQLInsert ;
	sql << sSQLInsert;

	pPkgUserInvitionUserReply->set_issuccess(true);
	pPkgUserInvitionUserReply->set_invitioncode(sInvitionCode);
	return true;
}

bool DBSessionComm::handleMessage_userCheckInvitionCode(boost::shared_ptr<comm::PkgUserCheckInvitionCodeReq> pPkgUserCheckInvitionCodeReq, boost::shared_ptr<comm::PkgUserCheckInvitionCodeReply> pPkgUserCheckInvitionCodeReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//验证邀请码是否合法
	//邀请码是否在数据库中
	soci::session sql(*dbUtil_.getConnection());
	try
	{
		//取得邀请码联系的数据
		typedef boost::tuple<std::string, std::string> BTUPLE;
		std::string sSQLSelect;
		sSQLSelect = "SELECT phonenumber,emailaddress FROM registerinvition WHERE type = " + boost::lexical_cast<std::string>(pPkgUserCheckInvitionCodeReq->gettype()) + " AND invitioncode = '" + pPkgUserCheckInvitionCodeReq->invitioncode() + "'";
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCheckInvitionCode),SQL:" << sSQLSelect ;
		soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
		soci::rowset<BTUPLE>::const_iterator it = rows.begin();
		if (it != rows.end())
		{
			const BTUPLE& dbvalues = *it;
			pPkgUserCheckInvitionCodeReply->set_issuccess(true);
			std::string phonenumber = dbvalues.get<0>();
			std::string emailaddress = dbvalues.get<1>();
			std::string registerinvition_phonenumber_decoded;
			std::string registerinvition_emailaddress_decoded;
			decodeString(phonenumber, registerinvition_phonenumber_decoded);
			decodeString(emailaddress, registerinvition_emailaddress_decoded);
			pPkgUserCheckInvitionCodeReply->set_phonenumber(registerinvition_phonenumber_decoded);
			pPkgUserCheckInvitionCodeReply->set_emailaddress(registerinvition_emailaddress_decoded);
		}
		else
		{
			pPkgUserCheckInvitionCodeReply->set_issuccess(false);
			pPkgUserCheckInvitionCodeReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wronginvitioncode"));
		}
	}
	catch (soci::soci_error &err)
	{
		pPkgUserCheckInvitionCodeReply->set_issuccess(false);
		pPkgUserCheckInvitionCodeReply->set_err(err.what());
		throw err;
	}
	catch (...)
	{
		pPkgUserCheckInvitionCodeReply->set_issuccess(false);
		pPkgUserCheckInvitionCodeReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.dataerror"));
	}
	return true;

}

bool DBSessionComm::handleMessage_userInvitionCheck(boost::shared_ptr<comm::PkgUserInvitionCheckReq> pPkgUserInvitionCheckReq, boost::shared_ptr<comm::PkgUserInvitionCheckReply> pPkgUserInvitionCheckReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//通过邀请码加入组织
	if (pPkgSessionUser->user().companyid() > 0)
	{
		pPkgUserInvitionCheckReply->set_issuccess(false);
		pPkgUserInvitionCheckReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.noaddcompany"));
		return true;
	}
	//判断邀请码是否合法，如果是，取出companyid
	int companyid = 0;
	soci::session sql(*dbUtil_.getConnection());
	std::string sSQLSelect;
	sSQLSelect = "SELECT companyid FROM registerinvition WHERE invitioncode = '" + pPkgUserInvitionCheckReq->invitioncode() + "'";
	soci::indicator ind(soci::indicator::i_null);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userInvitionCheck),SQL:" << sSQLSelect ;
	sql << sSQLSelect, into(companyid, ind);

	if (ind != soci::indicator::i_ok)
	{
		pPkgUserInvitionCheckReply->set_issuccess(false);
		pPkgUserInvitionCheckReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wronginvitioncode"));
		return false;
	}

	//通知组织所有人（不包括自己）
	std::vector<::session::PkgUser> vectUsers;
	getCompanyUsers(pPkgSessionUser->user().companyid(), vectUsers,sql);

	//设置自己为该companyid
	std::string sSQLUpdate = "UPDATE user SET companyid = " + boost::lexical_cast<std::string>(companyid) 
		+ " WHERE id = " + boost::lexical_cast<std::string>(pPkgSessionUser->userid());
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userInvitionCheck),SQL:" << sSQLUpdate ;
	sql << sSQLUpdate;

	{
		pPkgUserInvitionCheckReply->set_issuccess(true);
		//返回信息
		comm::PkgCompanyInfo*pPkgCompanyInfo = getCompanyInfo(pPkgSessionUser, sql);
		pPkgUserInvitionCheckReply->set_allocated_companyinfo(pPkgCompanyInfo);
		{
			//待废弃
			pPkgUserInvitionCheckReply->set_companyid(pPkgCompanyInfo->companyid());
			pPkgUserInvitionCheckReply->set_companyname(pPkgCompanyInfo->companyname());
			pPkgUserInvitionCheckReply->set_companycreatetime(pPkgCompanyInfo->companycreatetime());
			pPkgUserInvitionCheckReply->set_companyusernumber(pPkgCompanyInfo->companyusernumber());

		}
		
		int permissionroleid = 1;
		boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = mapRoles[permissionroleid];
		if (pPkgRoleUser)
		{
			pPkgUserInvitionCheckReply->set_rolename(pPkgRoleUser->rolename());
			pPkgUserInvitionCheckReply->set_permission(pPkgRoleUser->permission());
			pPkgUserInvitionCheckReply->set_permissionroleid(permissionroleid);
			pPkgSessionUser->mutable_user()->set_permissionroleid(permissionroleid);
			pPkgSessionUser->mutable_user()->set_permission(pPkgRoleUser->permission());
		}
		
	
		//个人加入组织需通知所有其他成员
		{
			std::vector<int> vectNotifyUserIDs;
			BOOST_FOREACH(::session::PkgUser&u, vectUsers){
				vectNotifyUserIDs.push_back(u.userid());
			}
			updateUserUpdateFlags(vectNotifyUserIDs, comm::EnUpdatedFlag::UF_COMPANYINFO, sql);
		}

	}
	return true;
}

bool DBSessionComm::handleMessage_userCreateCompany(boost::shared_ptr<comm::PkgUserCreateCompanyReq> pPkgUserCreateCompanyReq, boost::shared_ptr<comm::PkgUserCreateCompanyReply> pPkgUserCreateCompanyReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//创建组织
	if (pPkgSessionUser->user().companyid() > 0 )
	{
		pPkgUserCreateCompanyReply->set_issuccess(false);
		pPkgUserCreateCompanyReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.nocreatecompany"));
		return true;
	}

	if (pPkgUserCreateCompanyReq->companyname().empty())
	{
		pPkgUserCreateCompanyReply->set_issuccess(false);
		pPkgUserCreateCompanyReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.companynamenotnull"));
		return true;
	}

	std::string sSQLInsert;
	std::string sCurTime = UtilTime::getCurrentTime();
	std::string company_name_encoded;
	encodeString(pPkgUserCreateCompanyReq->companyname(), company_name_encoded);
	sSQLInsert = "INSERT INTO company(name,createuserid,createtime) VALUES('" + company_name_encoded + "',"
		+ boost::lexical_cast<std::string>(pPkgSessionUser->userid()) + ",'" + sCurTime + "')";
	soci::session sql(*dbUtil_.getConnection());
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateCompany),SQL:" << sSQLInsert ;
	sql << sSQLInsert;

	int companyid;
	std::string sSQLSelect = "SELECT id FROM company WHERE name = '" + company_name_encoded + "' ORDER BY id DESC";
	soci::indicator ind(soci::indicator::i_null);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateCompany),SQL:" << sSQLSelect ;

	sql << sSQLSelect, into(companyid, ind);
	if (ind != soci::indicator::i_ok)
	{
		pPkgUserCreateCompanyReply->set_issuccess(false);
		pPkgUserCreateCompanyReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.nocreatecompany"));
		return true;
	}

	pPkgUserCreateCompanyReply->set_issuccess(true);

	int permissionroleid = 2;
	std::string sSQLUpdate = "UPDATE user SET companyid = " + boost::lexical_cast<std::string>(companyid) + ",permissionroleid = "
		+ boost::lexical_cast<std::string>(permissionroleid)+" WHERE id = " + boost::lexical_cast<std::string>(pPkgSessionUser->userid());
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userCreateCompany),SQL:" << sSQLUpdate ;

	sql << sSQLUpdate;
	boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = mapRoles[permissionroleid];
	if (pPkgRoleUser)
	{
		pPkgUserCreateCompanyReply->set_rolename(pPkgRoleUser->rolename());
		pPkgUserCreateCompanyReply->set_permissionroleid(permissionroleid);

		pPkgSessionUser->mutable_user()->set_permission(pPkgRoleUser->permission());
		pPkgSessionUser->mutable_user()->set_permissionroleid(permissionroleid);
	}

	comm::PkgCompanyInfo*pPkgCompanyInfo = getCompanyInfo(pPkgSessionUser, sql);
	pPkgUserCreateCompanyReply->set_allocated_companyinfo(pPkgCompanyInfo);
	{
		//待废弃
		pPkgUserCreateCompanyReply->set_companyid(pPkgCompanyInfo->companyid());
		pPkgUserCreateCompanyReply->set_companyname(pPkgCompanyInfo->companyname());
		pPkgUserCreateCompanyReply->set_companycreatetime(pPkgCompanyInfo->companycreatetime());
		pPkgUserCreateCompanyReply->set_companyusernumber(pPkgCompanyInfo->companyusernumber());

	}

	
	return true;
}

bool DBSessionComm::handleMessage_userReassignAdmin(boost::shared_ptr<comm::PkgUserReassignAdminReq> pPkgUserReassignAdminReq, boost::shared_ptr<comm::PkgUserReassignAdminReply> pPkgUserReassignAdminReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//转让管理员
	if (!isUserAdmin(pPkgSessionUser))
	{
		pPkgUserReassignAdminReply->set_issuccess(false);
		pPkgUserReassignAdminReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.nopermission"));
		return true;
	}
	int permissionroleid = 2;
	soci::session sql(*dbUtil_.getConnection());
	std::string sSQLUpdate = "UPDATE user SET permissionroleid = " + boost::lexical_cast<std::string>(permissionroleid) 
		+" WHERE companyid = " + getUserCompanyId(pPkgSessionUser)
		+ " AND id = " + boost::lexical_cast<std::string>(pPkgUserReassignAdminReq->adminuserid());
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userReassignAdmin),SQL:" << sSQLUpdate ;
	sql << sSQLUpdate;

	permissionroleid = 1;
	sSQLUpdate = "UPDATE user SET permissionroleid = " + boost::lexical_cast<std::string>(permissionroleid)
		+" WHERE companyid = " + getUserCompanyId(pPkgSessionUser)
		+ " AND id = " + boost::lexical_cast<std::string>(pPkgSessionUser->userid());
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userReassignAdmin),SQL:" << sSQLUpdate ;
	sql << sSQLUpdate;

	pPkgSessionUser->mutable_user()->set_permissionroleid(permissionroleid);
	pPkgUserReassignAdminReply->set_issuccess(true);
	boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = mapRoles[permissionroleid];
	if (pPkgRoleUser)
	{
		pPkgUserReassignAdminReply->set_permission(pPkgRoleUser->permission());
		pPkgUserReassignAdminReply->set_rolename(pPkgRoleUser->rolename());
		pPkgUserReassignAdminReply->set_permissionroleid(permissionroleid);
	}
	updateUserUpdateFlags(pPkgUserReassignAdminReq->adminuserid(), comm::UF_USERROLE, sql);
	return true;
}

std::string DBSessionComm::getUserCompanyId(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sCompanyId;
	int nCompanyId = pPkgSessionUser->user().companyid();
	sCompanyId = boost::lexical_cast<std::string>(nCompanyId);
	return sCompanyId;
}

bool DBSessionComm::isUserAdmin(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	return pPkgSessionUser->user().permissionroleid() == 2;
}

bool DBSessionComm::handleMessage_userDeleteFromCompany(boost::shared_ptr<comm::PkgUserDeleteFromCompanyReq> pPkgUserDeleteFromCompanyReq, boost::shared_ptr<comm::PkgUserDeleteFromCompanyReply> pPkgUserDeleteFromCompanyReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//从组织中删除成员（仅限管理员）
	std::vector<int> vectNotifyUserIDs;
	if (!isUserAdmin(pPkgSessionUser))
	{
		pPkgUserDeleteFromCompanyReply->set_issuccess(false);
		pPkgUserDeleteFromCompanyReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.nopermission"));
		return true;
	}
	soci::session sql(*dbUtil_.getConnection());
	for (int i = 0; i < pPkgUserDeleteFromCompanyReq->useridlist_size(); i++){
		int userid = pPkgUserDeleteFromCompanyReq->useridlist().Get(i);
		std::string sSQLUpdate = "UPDATE user SET companyid = 0 WHERE companyid = " + getUserCompanyId(pPkgSessionUser) + " AND id = "
			+ boost::lexical_cast<std::string>(userid);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userDeleteFromCompany),SQL:" << sSQLUpdate ;
		sql << sSQLUpdate;

		vectNotifyUserIDs.push_back(userid);
	}
		
	{
		std::vector<::session::PkgUser> vectUsers;
		getCompanyUsers(pPkgSessionUser->user().companyid(), vectUsers,sql);

		BOOST_FOREACH(::session::PkgUser&u, vectUsers){
			vectNotifyUserIDs.push_back(u.userid());
		}
		//现存的成员数
		pPkgUserDeleteFromCompanyReply->set_companyusernumber(vectUsers.size());
	}
	pPkgUserDeleteFromCompanyReply->set_issuccess(true);
	//

	updateUserUpdateFlags(vectNotifyUserIDs, comm::EnUpdatedFlag::UF_COMPANYINFO, sql);
	return true;
}

void DBSessionComm::getCompanyUsers(int companyid, std::vector<::session::PkgUser>&vectUsers, soci::session& sql)
{
	::session::PkgUser pkgUser;
	std::string sSQLSelect = "SELECT * FROM user WHERE companyid = " + boost::lexical_cast<std::string>(companyid);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getCompanyUsers),SQL:" << sSQLSelect;
	soci::statement st = (sql.prepare << sSQLSelect, soci::into(pkgUser));
	st.execute();
	while (st.fetch())
	{	
		vectUsers.push_back(pkgUser);
	}
}


bool DBSessionComm::handleMessage_userExitFromCompany(boost::shared_ptr<comm::PkgUserExitCompanyReq> pPkgUserExitCompanyReq, boost::shared_ptr<comm::PkgUserExitCompanyReply> pPkgUserExitCompanyReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//个人退出组织或解散组织
	soci::session sql(*dbUtil_.getConnection());

	if (pPkgUserExitCompanyReq->encmd() == comm::PkgUserExitCompanyReq::CMD_EXIT)
	{
		if (isUserAdmin(pPkgSessionUser))
		{//管理员不能退出
			pPkgUserExitCompanyReply->set_issuccess(false);
			pPkgUserExitCompanyReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.admincannotexitcompany"));
			return true;
		}

		//
		std::string sSQLUpdate = "UPDATE user SET companyid = 0 WHERE id = " + boost::lexical_cast<std::string>(pPkgSessionUser->userid());
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userExitFromCompany),SQL:" << sSQLUpdate ;
		sql << sSQLUpdate;

		pPkgSessionUser->mutable_user()->set_companyid(0);
		pPkgUserExitCompanyReply->set_issuccess(true);
		//个人退出组织需通知所有其他成员
		{
			std::vector<int> vectNotifyUserIDs;
			std::vector<::session::PkgUser> vectUsers;
			getCompanyUsers(pPkgSessionUser->user().companyid(), vectUsers,sql);
			BOOST_FOREACH(::session::PkgUser&u, vectUsers){
				vectNotifyUserIDs.push_back(u.userid());
			}
			updateUserUpdateFlags(vectNotifyUserIDs, comm::EnUpdatedFlag::UF_COMPANYINFO, sql);
		}
	}
	else if (pPkgUserExitCompanyReq->encmd() == comm::PkgUserExitCompanyReq::CMD_DISMISS)
	{
		if (!isUserAdmin(pPkgSessionUser))
		{//无权限
			pPkgUserExitCompanyReply->set_issuccess(false);
			pPkgUserExitCompanyReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.nopermission"));
			return true;
		}

		//判断是否只有自己1个成员
		std::string sSQLSelect = "SELECT COUNT(id) FROM user WHERE companyid = " + getUserCompanyId(pPkgSessionUser)
			+ " AND id <> " + boost::lexical_cast<std::string>(pPkgSessionUser->userid());
		int count = 0;
		soci::indicator ind(soci::indicator::i_null);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userExitFromCompany),SQL:" << sSQLSelect ;

		sql << sSQLSelect, into(count, ind);
		if (ind == soci::indicator::i_ok)
		{
		}

		if (count > 0)
		{
			pPkgUserExitCompanyReply->set_issuccess(false);
			pPkgUserExitCompanyReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.cannotdismisscompany"));
			return true;
		}
		std::string sSQLUpdate = "UPDATE user SET companyid = 0 WHERE id = " + boost::lexical_cast<std::string>(pPkgSessionUser->userid());
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(userExitFromCompany),SQL:" << sSQLUpdate ;
		sql << sSQLUpdate;

		pPkgSessionUser->mutable_user()->set_companyid(0);
		pPkgUserExitCompanyReply->set_issuccess(true);
	}


	return true;
}

bool DBSessionComm::handleMessage_taskQuery(boost::shared_ptr<comm::PkgTaskQueryReq> pPkgTaskQueryReq, boost::shared_ptr<comm::PkgTaskQueryReply> pPkgTaskQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	soci::session sql(*dbUtil_.getConnection());

	std::map<int, std::string> mapUseRealname;
	getUserMap(mapUseRealname,sql);

	pPkgTaskQueryReply->set_issuccess(true);
	std::string sSQLSelectTask, sSQLSelectSubTask, sSQLSelectCoworkTask;
	if (pPkgTaskQueryReq->queryflag() & comm::PkgTaskQueryReq::QF_TASKINFO)
	{
		sSQLSelectTask = "SELECT * FROM task WHERE id = " + boost::lexical_cast<std::string>(pPkgTaskQueryReq->taskid());
	}

	if (pPkgTaskQueryReq->queryflag() & comm::PkgTaskQueryReq::QF_TASKWITHSUBTASK)
	{
		sSQLSelectSubTask = "SELECT * FROM task WHERE parentid = " + boost::lexical_cast<std::string>(pPkgTaskQueryReq->taskid());
	}

	if (pPkgTaskQueryReq->queryflag() & comm::PkgTaskQueryReq::QF_TASKWITHASSGIN)
	{
		sSQLSelectCoworkTask = "SELECT acceptedid FROM task WHERE acceptedid <> createuserid AND coworkid = " + boost::lexical_cast<std::string>(pPkgTaskQueryReq->taskid());
	}

	if (!sSQLSelectTask.empty())
	{
		comm::PkgTaskInfo task;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskQuery),SQL:" << sSQLSelectTask ;

		soci::statement st = (sql.prepare << sSQLSelectTask, soci::into(task));
		st.execute();
		if (st.fetch())
		{
			comm::PkgTaskInfo* pPkgTaskInfo = new comm::PkgTaskInfo(task);
			if (pPkgTaskInfo->accepteduserid() > 0){
				pPkgTaskInfo->set_acceptedrealname(mapUseRealname[pPkgTaskInfo->accepteduserid()]);
			}
			pPkgTaskInfo->set_createdrealname(mapUseRealname[pPkgTaskInfo->createuserid()]);

			pPkgTaskQueryReply->set_allocated_taskinfo(pPkgTaskInfo);
		}
	}

	if (!sSQLSelectSubTask.empty())
	{
		comm::PkgTaskInfo task;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskQuery),SQL:" << sSQLSelectSubTask ;

		soci::statement st = (sql.prepare << sSQLSelectSubTask, soci::into(task));
		st.execute();
		while (st.fetch())
		{
			comm::PkgTaskInfo* pPkgTaskInfo = pPkgTaskQueryReply->add_taskinfolist();
			*pPkgTaskInfo = task;
			
		}
	}

	if (!sSQLSelectCoworkTask.empty())
	{
		int accepteduserid;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskQuery),SQL:" << sSQLSelectCoworkTask ;
		soci::statement st = (sql.prepare << sSQLSelectCoworkTask, soci::into(accepteduserid));
		st.execute();
		while (st.fetch())
		{
			pPkgTaskQueryReply->add_accepteduseridlist(accepteduserid);
		}

	}
	return true;
}

std::string DBSessionComm::getTaskPageQueryWhereCondition(int nUserid, boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq)
{
	std::string sqlWhereCondition;
	std::string sqlWhereS;
	std::vector<std::string> vectConditions;
	const comm::PkgTaskPageQueryReq::PkgTaskQueryCondition&cTaskQueryCondition = pPkgTaskPageQueryReq->pkgtaskquerycondition();
	if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_READEDFLAG)
	{
		if (cTaskQueryCondition.enreadedflag() != comm::PkgTaskPageQueryReq::PkgTaskQueryCondition::PR_ALL){
			if (cTaskQueryCondition.enreadedflag() == comm::PkgTaskPageQueryReq::PkgTaskQueryCondition::PR_READED){
				sqlWhereS = "isreaded > 0";
			}
			else{
				sqlWhereS = "isreaded = 0";
			}
			vectConditions.push_back(sqlWhereS);
		}
	}
	if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_LASTUPDATETIME)
	{
		sqlWhereS = "updatetime >= '" + cTaskQueryCondition.lastupdatetime() + "'";
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_DATESTART)
	{
		sqlWhereS = "taskdate >= '" + cTaskQueryCondition.taskdatestart() + "'";
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_DATEEND)
	{
		sqlWhereS = "taskdate <= '" + cTaskQueryCondition.taskdateend() + "'";
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_KEYFORNAME
		&& pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_KEYFORCONTENT){
		std::string keyname_encoded_hex;
		std::string keycontent_encoded_hex;
		encodeString_Hex(cTaskQueryCondition.keyforname(), keyname_encoded_hex);
		encodeString_Hex(cTaskQueryCondition.keyforcontent(), keycontent_encoded_hex);
		std::string sqlWhereS1 = "name LIKE '%" + keyname_encoded_hex + "%'";
		std::string sqlWhereS2 = "content LIKE '%" + keycontent_encoded_hex + "%'";
		sqlWhereS = "(" + sqlWhereS1 + " OR " + sqlWhereS2 + ")";
		vectConditions.push_back(sqlWhereS);
	}
	else{
		if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_KEYFORNAME)
		{
			std::string keyname_encoded_hex;
			encodeString_Hex(cTaskQueryCondition.keyforname(), keyname_encoded_hex);
			sqlWhereS = "name LIKE '%" + keyname_encoded_hex + "%'";
			vectConditions.push_back(sqlWhereS);
		}
		if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_KEYFORCONTENT)
		{
			std::string keycontent_encoded_hex;
			encodeString_Hex(cTaskQueryCondition.keyforcontent(), keycontent_encoded_hex);
			sqlWhereS = "content LIKE '%" + keycontent_encoded_hex + "%'";
			vectConditions.push_back(sqlWhereS);
		}
	}
	if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_TASKSTATUS)
	{
		sqlWhereS.clear();
		for (int i = 0; i < cTaskQueryCondition.taskstatuslist_size(); i++)
		{
			int status = cTaskQueryCondition.taskstatuslist().Get(i);
			if (cTaskQueryCondition.taskstatuslist_size() >1)
			{
				if (i == 0)
				{
					sqlWhereS = "(";
				}
				else
				{
					sqlWhereS += " OR ";
				}
				sqlWhereS += "status = " + boost::lexical_cast<std::string>(status);

				if (i == cTaskQueryCondition.taskstatuslist_size() - 1)
				{
					sqlWhereS += ")";
				}
			}
			else
			{
				assert(i == 0);
				sqlWhereS = "status = " + boost::lexical_cast<std::string>(status);
			}

		}
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_TASKNOTSTATUS)
	{
		sqlWhereS.clear();
		for (int i = 0; i < cTaskQueryCondition.taskstatuslist_size(); i++)
		{
			int status = cTaskQueryCondition.taskstatuslist().Get(i);
			if (cTaskQueryCondition.taskstatuslist_size() >1)
			{
				if (i == 0)
				{
					sqlWhereS = "(";
				}
				else
				{
					sqlWhereS += " AND ";
				}
				sqlWhereS += "status <> " + boost::lexical_cast<std::string>(status);

				if (i == cTaskQueryCondition.taskstatuslist_size() - 1)
				{
					sqlWhereS += ")";
				}
			}
			else
			{
				assert(i == 0);
				sqlWhereS = "status <> " + boost::lexical_cast<std::string>(status);
			}

		}
		vectConditions.push_back(sqlWhereS);
	}

	const comm::PkgTaskPageQueryReq::EnTaskFlag& taskFlag = pPkgTaskPageQueryReq->taskflag();
	if (taskFlag == comm::PkgTaskPageQueryReq::TF_MYTASK)
	{
		std::string sqlWhere1 = "createuserid = " + boost::lexical_cast<std::string>(nUserid)+" AND coworkid = 0";
		std::string sqlWhere2 = "createuserid = " + boost::lexical_cast<std::string>(nUserid)+" AND coworkid = id";
		std::string sqlWhere3 = "accepteduserid = " + boost::lexical_cast<std::string>(nUserid)+
			" AND coworkid <> id AND status <> " + boost::lexical_cast<std::string>(comm::PkgTaskInfo::TS_NEW) + " AND status <> " + boost::lexical_cast<std::string>(comm::PkgTaskInfo::TS_DENY);

		sqlWhereS = "((" + sqlWhere1 + ") OR (" + sqlWhere2 + ") OR (" + sqlWhere3 + "))";
		vectConditions.push_back(sqlWhereS);

		std::string sqlWhereDelete = "status <> " + boost::lexical_cast<std::string>(comm::PkgTaskInfo::TS_DELETED);
		vectConditions.push_back(sqlWhereDelete);

	}
	else if (taskFlag == comm::PkgTaskPageQueryReq::TF_COWORKTASK)
	{
		std::string sWhereToMySelf = "(accepteduserid = " + boost::lexical_cast<std::string>(nUserid)+" AND coworkid <> id)";
		std::string sWhereToOthers = "(createuserid = " + boost::lexical_cast<std::string>(nUserid)+" AND coworkid = id)";

		if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_ONLYSEND)
		{
			sqlWhereS = sWhereToOthers;
		}
		else if (pPkgTaskPageQueryReq->enconditon() & comm::PkgTaskPageQueryReq::TQC_ONLYRECV)
		{
			sqlWhereS = sWhereToMySelf;
		}
		else
		{
			sqlWhereS = "((" + sWhereToOthers + ") OR (" + sWhereToMySelf + "))";
		}
		vectConditions.push_back(sqlWhereS);

		std::string sqlWhereDelete = "status <> " + boost::lexical_cast<std::string>(comm::PkgTaskInfo::TS_DELETED);
		vectConditions.push_back(sqlWhereDelete);
	}
	else if (taskFlag == comm::PkgTaskPageQueryReq::TF_TASKTABLE)
	{
		sqlWhereS = "(createuserid = " + boost::lexical_cast<std::string>(nUserid)
			+" OR accepteduserid = " + boost::lexical_cast<std::string>(nUserid)+")";
		vectConditions.push_back(sqlWhereS);
	}
	else
	{
		sqlWhereS = "createuserid = " + boost::lexical_cast<std::string>(nUserid);
		vectConditions.push_back(sqlWhereS);
	}

	sqlWhereCondition.clear();
	for (size_t i = 0; i< vectConditions.size(); i++)
	{
		const std::string&sqlWhere = vectConditions[i];
		if (i == 0)
		{
			sqlWhereCondition = " WHERE ";
		}
		else
		{
			sqlWhereCondition += " AND ";
		}

		sqlWhereCondition += sqlWhere;
	}
	return sqlWhereCondition;
}

bool DBSessionComm::handleMessage_taskPageQuery(boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq, boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	
	soci::session sql(*dbUtil_.getConnection());
	std::map<int, std::string> mapUseRealname;
	getUserMap(mapUseRealname,sql);


	std::string sqlWhereCondition = getTaskPageQueryWhereCondition(pPkgSessionUser->userid(), pPkgTaskPageQueryReq);
	std::string sOrderBy = " ORDER BY isreaded,taskdate DESC,sortflag,id DESC";
	std::string sSQLSelect;
	if (pPkgTaskPageQueryReq->pageflag() == comm::PkgTaskPageQueryReq::P_ONLYNUM)
	{
		sSQLSelect = "SELECT COUNT(id) FROM task" + sqlWhereCondition + sOrderBy;
		int count;
		soci::indicator ind(soci::indicator::i_null);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskPageQuery),SQL:" << sSQLSelect ;

		sql << sSQLSelect, into(count, ind);
		if (ind == soci::indicator::i_ok)
		{
			pPkgTaskPageQueryReply->set_issuccess(true);
			pPkgTaskPageQueryReply->set_returnnum(count);
		}
		else
		{
			pPkgTaskPageQueryReply->set_issuccess(false);
		}
	
	}
	else
	{
		const comm::PkgTaskPageQueryReq::PkgTaskQueryCondition&cTaskQueryCondition = pPkgTaskPageQueryReq->pkgtaskquerycondition();
		const comm::PkgTaskPageQueryReq::EnTaskFlag& taskFlag = pPkgTaskPageQueryReq->taskflag();
		//今日任务
		if (taskFlag == comm::PkgTaskPageQueryReq::TF_MYTASK){
			if (pPkgTaskPageQueryReq->enconditon() == 0){
				clearUserUpdateFlag(pPkgSessionUser->userid(), comm::EnUpdatedFlag::UF_MYTASK);
			}
			//日期相等
			if (pPkgTaskPageQueryReq->enconditon() == (comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND)){
				if (cTaskQueryCondition.taskdatestart() == cTaskQueryCondition.taskdateend()
					&& pPkgTaskPageQueryReq->pageflag() == comm::PkgTaskPageQueryReq::P_RETURNALL){				
					sOrderBy = " ORDER BY updatetime DESC";
				}
			}
		}
		else if (taskFlag == comm::PkgTaskPageQueryReq::TF_COWORKTASK){
			clearUserUpdateFlag(pPkgSessionUser->userid(), comm::EnUpdatedFlag::UF_TASKCOWORK);
		}
		else if (taskFlag == comm::PkgTaskPageQueryReq::TF_TASKTABLE){
			clearUserUpdateFlag(pPkgSessionUser->userid(), comm::EnUpdatedFlag::UF_MYTASK);
			clearUserUpdateFlag(pPkgSessionUser->userid(), comm::EnUpdatedFlag::UF_TASKCOWORK);

		}

		std::vector<boost::shared_ptr<comm::PkgTaskInfo>> vectTasks;
		comm::PkgTaskInfo task;
		sSQLSelect = "SELECT * FROM task" + sqlWhereCondition + sOrderBy;
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskPageQuery),SQL:" << sSQLSelect ;

		soci::statement st = (sql.prepare << sSQLSelect, soci::into(task));
		st.execute();
		while (st.fetch())
		{
			boost::shared_ptr<comm::PkgTaskInfo> pPkgTaskInfo = boost::make_shared<comm::PkgTaskInfo>(task);
			//设置acceptname
			if (pPkgTaskInfo->accepteduserid() > 0){
				pPkgTaskInfo->set_acceptedrealname(mapUseRealname[pPkgTaskInfo->accepteduserid()]);
			}
			pPkgTaskInfo->set_createdrealname(mapUseRealname[pPkgTaskInfo->createuserid()]);
			vectTasks.push_back(pPkgTaskInfo);
		}

		//今日任务
		//if (isTodayTask){
		//	sortTodayTaskArray(vectTasks, pPkgSessionUser);
		//}

		pPkgTaskPageQueryReply->set_issuccess(true);
		int iStart = 0;
		int iEnd = vectTasks.size();
		if (pPkgTaskPageQueryReq->pageflag() == comm::PkgTaskPageQueryReq::P_RETURNPAGE)
		{
			if (pPkgTaskPageQueryReq->indexstart() > iStart)
			{
				iStart = pPkgTaskPageQueryReq->indexstart();
			}

			if (pPkgTaskPageQueryReq->indexend() < iEnd)
			{
				iEnd = pPkgTaskPageQueryReq->indexend();
			}
		}
		
		for (int i = iStart; i < iEnd; i++)
		{
			boost::shared_ptr<comm::PkgTaskInfo> pPkgTaskInfo = vectTasks[i];
			comm::PkgTaskInfo * pTask = pPkgTaskPageQueryReply->add_taskinfolist();

			*pTask = *pPkgTaskInfo;		
		}
	}

	return bSuc;

}

void DBSessionComm::sortDateTask(const std::string&sTaskDate, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
	comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
	pPkgTaskQueryCondition->set_taskdatestart(sTaskDate);
	pPkgTaskQueryCondition->set_taskdateend(sTaskDate);
	pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
	pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_RETURNALL);
	pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
	pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_DATESTART | comm::PkgTaskPageQueryReq::TQC_DATEEND);
	boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::make_shared<comm::PkgTaskPageQueryReply>();
	handleMessage_taskPageQuery(pPkgTaskPageQueryReq, pPkgTaskPageQueryReply, pPkgSessionUser);

	std::vector<boost::shared_ptr<comm::PkgTaskInfo>> vectTasks;
	for (int i = 0; i < pPkgTaskPageQueryReply->taskinfolist_size(); i++)
	{
		const comm::PkgTaskInfo&task = pPkgTaskPageQueryReply->taskinfolist().Get(i);
		boost::shared_ptr<comm::PkgTaskInfo> pPkgTaskInfo = boost::make_shared<comm::PkgTaskInfo>(task);
		vectTasks.push_back(pPkgTaskInfo);
	}
	sortTodayTaskArray(vectTasks, pPkgSessionUser);
}

void DBSessionComm::onSortCurTask(int taskid, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, soci::session& sql)
{
	std::string sSQLSelect = "SELECT taskdate FROM task WHERE id = " + boost::lexical_cast<std::string>(taskid);
	std::string taskdate;
	soci::indicator ind(soci::indicator::i_null);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(onSortCurTask),SQL:" << sSQLSelect;
	sql << sSQLSelect, into(taskdate, ind);

	if (ind == soci::indicator::i_ok)
	{
	}

	if (!taskdate.empty())
	{
		sortDateTask(taskdate, pPkgSessionUser);
	}
}

bool DBSessionComm::getTaskInfo(comm::PkgTaskInfo& pkgTaskInfo, int taskid, soci::session& sql)
{
	if (taskid == 0)
		return false;
	std::string sSQLSelectTask = "SELECT * FROM task WHERE id = " + boost::lexical_cast<std::string>(taskid);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getTaskInfo),SQL:" << sSQLSelectTask;

	soci::statement st = (sql.prepare << sSQLSelectTask, soci::into(pkgTaskInfo));
	st.execute();
	if (!st.fetch())
	{
		return false;
	}

	return true;
}

bool DBSessionComm::isTaskDisableUpdateStatus(const comm::PkgTaskInfo&pkgTaskInfo, int updatestatus, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool isDisableUpdateStatus = false;
	if (pkgTaskInfo.coworkid() == 0){
		if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_NEW){
			if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED
				&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED){
				isDisableUpdateStatus = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[mytask,tsnew only set to tsFinished/tsDeleted]" <<
					"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
			}
		}
		else if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED){
			if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_NEW
				&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED){
				isDisableUpdateStatus = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[mytask finished,only set to new/deleted]" <<
					"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
			}
		}
	}
	else{
		if (pkgTaskInfo.createuserid() == pPkgSessionUser->userid()){
			//自己发送给别人
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_NEW){
				//新建
				if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED
					&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED){
					//完成/删除
					isDisableUpdateStatus = true;
					BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[cworktask send others new only set to finished/deleted]" <<
						"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
				}
			}
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_DENY){
				//拒绝
				if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED
					&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED
					&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_NEW){
					//完成/删除/新建
					isDisableUpdateStatus = true;
					BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[coworktask sendto others,deny only set to finished/deleted/new]" <<
						"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
				}
			}
			else if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED){
				if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_NEW
					&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED){
					//新建/删除
					isDisableUpdateStatus = true;
					BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[coworktask sendto others,finished only set to new/deleted]" <<
						"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
				}
			}
		}
		if (pkgTaskInfo.accepteduserid() == pPkgSessionUser->userid()){
			//别人发送给自己，自己只能接受或拒绝
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_NEW){
				if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_GOING
					&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DENY){
					//参数错误
					isDisableUpdateStatus = true;
					BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[coworktask recvfrom others,new only set to accept/deny]" <<
						"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
				}
			}
			else if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_GOING){
				if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED
					&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED){
					//参数错误
					isDisableUpdateStatus = true;
					BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[coworktask recvfrom others,accepted only set to finished/deleted]" <<
						"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
				}
			}
			else if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED){
				if (updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_GOING
					&& updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED){
					//参数错误
					isDisableUpdateStatus = true;
					BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[coworktask recvfrom others,finished only set to going/deleted]" <<
						"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
				}
			}
			else if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_DENY){
				if ( updatestatus != comm::PkgTaskInfo_EnTaskStatus_TS_DELETED){
					//参数错误
					isDisableUpdateStatus = true;
					BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableUpdateStatus)[coworktask recvfrom others,denied only set to deleted]" <<
						"--[taskid:]" << pkgTaskInfo.id() << ",[updatestatus:]" << updatestatus;
				}
			}

		}
	}
	return isDisableUpdateStatus;
}

bool DBSessionComm::isTaskDisableEdit(const comm::PkgTaskInfo&pkgTaskInfo, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool isDisableEdit = false;
	if (pkgTaskInfo.coworkid() == 0){
		if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED){
			isDisableEdit = true;
			BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[mytask finished can't be edited]" << 
				"--[taskid:]" << pkgTaskInfo.id();
		}
	}
	else {
		if (pkgTaskInfo.createuserid() == pPkgSessionUser->userid()){
			//发送给别人
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_NEW){
				isDisableEdit = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[coworktask sendtoothers new can't be edited]" <<
					"--[taskid:]" << pkgTaskInfo.id();
			}
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED){
				isDisableEdit = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[coworktask sendtoothers finished can't be edited]" <<
					"--[taskid:]" << pkgTaskInfo.id();
			}
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_GOING){
				isDisableEdit = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[coworktask sendtoothers going can't be edited]" <<
					"--[taskid:]" << pkgTaskInfo.id();
			}
		}

		if (pkgTaskInfo.accepteduserid() == pPkgSessionUser->userid()){
			//别人发送给自己
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_NEW){
				isDisableEdit = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[coworktask recvfrom others new can't be edited]" <<
					"--[taskid:]" << pkgTaskInfo.id();
			}
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_FINISHED){
				isDisableEdit = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[coworktask recvfrom others finished can't be edited]" <<
					"--[taskid:]" << pkgTaskInfo.id();
			}
			//if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_GOING){
			//	isDisableEdit = true;
			//	BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[别人发送给自己任务正在进行中不能编辑??]" <<
			//		"--[taskid:]" << pkgTaskInfo.id();
			//}
			if (pkgTaskInfo.status() == comm::PkgTaskInfo_EnTaskStatus_TS_DENY){
				isDisableEdit = true;
				BOOST_LOG_SEV(gLog_, boost::log::trivial::fatal) << "(isTaskDisableEdit)[coworktask recvfrom others denied can't be edited]" <<
					"--[taskid:]" << pkgTaskInfo.id();
			}
		}
	}
	return isDisableEdit;
}

bool DBSessionComm::getTaskDateBeforeUpdate(const comm::PkgTaskInfo&pkgTaskInfo, std::string&oldDate, std::string&newDate, soci::session& sql)
{

	std::string sSQLSelect;
	typedef boost::tuple<int, std::string> BTUPLE;
	sSQLSelect = "SELECT sortflag,taskdate FROM task WHERE id = " + boost::lexical_cast<std::string>(pkgTaskInfo.id());
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getTaskDateBeforeUpdate),SQL:" << sSQLSelect;

	std::string password;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	if (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		int flag = dbvalues.get<0>();
		std::string date = dbvalues.get<1>();
		if (date != pkgTaskInfo.taskdate())
		{//日期不同,旧日期和新日期都需要排序
			oldDate = date;
			newDate = pkgTaskInfo.taskdate();
		}
		else
		{
			if (flag != pkgTaskInfo.sortflag())
			{//标志不同，仅排序当前日期
				newDate = pkgTaskInfo.taskdate();
			}
		}
		return true;
	}
	return false;	

}

void DBSessionComm::onUpdateTask(const comm::PkgTaskInfo&newtask, const comm::PkgTaskInfo&oldtask, int nUserid, soci::session& sql)
{
	std::string curdatetime = UtilTime::getCurrentTime();
	//do update
	std::stringstream ss;
	std::string sSQLUpdate;

	std::string task_name_encoded;
	std::string task_content_encoded;
	if (oldtask.coworkid() == 0){
		encodeString_Hex(newtask.name(), task_name_encoded);
		encodeString_Hex(newtask.content(), task_content_encoded);
		ss << "UPDATE task SET name = '" << task_name_encoded << "',content = '" << task_content_encoded
			<< "',  taskdate = '" << newtask.taskdate() << "',tasklevel = " << newtask.tasklevel()
			<< ", sortflag = " << newtask.sortflag() << ",updatetime ='" << curdatetime << "' WHERE id = " << newtask.id();
	}
	else{
		if (oldtask.accepteduserid() == nUserid
			&& oldtask.status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_GOING){
			//别人发送给自己，自己已经接受，仅可以修改重要性，排序，任务日期
			ss << "UPDATE task SET taskdate = '" << newtask.taskdate() << "',tasklevel = " << newtask.tasklevel()
				<< ", sortflag = " << newtask.sortflag() << ",updatetime ='" << curdatetime << "' WHERE id = " << newtask.id();
		}
		else{
			//修改公共字段:
			std::stringstream ssComm;
			encodeString_Hex(newtask.name(), task_name_encoded);
			encodeString_Hex(newtask.content(), task_content_encoded);

			ssComm << "UPDATE task SET name = '" << task_name_encoded << "',content = '" << task_content_encoded
				<< "' WHERE coworkid = " << oldtask.coworkid();
			sSQLUpdate = ssComm.str();
			BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(onUpdateTask),SQL:" << sSQLUpdate;
			sql << sSQLUpdate;

			//修改自己的私有字段
			ss << "UPDATE task SET taskdate = '" << newtask.taskdate() << "',tasklevel = " << newtask.tasklevel()
				<< ", sortflag = " << newtask.sortflag() << ",updatetime ='" << curdatetime << "' WHERE id = " << newtask.id();
		}
	}
	sSQLUpdate = ss.str();
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(onUpdateTask),SQL:" << sSQLUpdate;

	sql << sSQLUpdate;
}

bool DBSessionComm::isMyTaskNotFinished(boost::shared_ptr<comm::PkgTaskInfo> p, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{//返回true表示未完成，返回false表示已完成
	/*
	我的任务包括:
	//1、自己创建的非协作任务(coworkid = 0)
	//2、自己创建的协作任务, 但别人不是进行中的任务(coworkid = taskid AND (status=TS_NEW OR status=TS_DENY))
	//3、别人发送给自己(处于不是:尚未接受 / 拒绝 状态的任务)
	*/
	if (p->coworkid() == 0)
	{
		//自己发送给自己的非协作任务
		if (p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_NEW)
			return true;
		if(p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_FINISHED)
			return false;
		
		return true;
	}
	else{
		//协作任务
		if (pPkgSessionUser->userid() == p->createuserid()){
			//自己发送给别人
			BOOST_ASSERT(p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_NEW
				|| p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_DENY
				|| p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_FINISHED);
			if (p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_NEW){
				return true;//自己发送给别人,别人尚未接受-->未完成
			}
			if (p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_DENY){
				return true;//自己发送给别人,别人拒绝-->未完成
			}
			if (p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_FINISHED){
				return false;//自己发送给别人,别人或自己已完成
			}
			return true;
		}
		else if (pPkgSessionUser->userid() == p->accepteduserid()){
			//别人发送给自己
			BOOST_ASSERT(p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_GOING
				|| p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_FINISHED);
			if (p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_GOING)
				return true;//别人发送给自己，自己正在做-->未完成
			if (p->status() == comm::PkgTaskInfo::EnTaskStatus::PkgTaskInfo_EnTaskStatus_TS_FINISHED)
				return false;//别人发送给自己，自己已完成-->已完成
			return true;
		}
	
	}
	return true;

}

void DBSessionComm::sortTodayTaskArray(std::vector<boost::shared_ptr<comm::PkgTaskInfo>>& vectTasks, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::list<boost::shared_ptr<comm::PkgTaskInfo>> vlTasks[7];//未完成
	std::list<boost::shared_ptr<comm::PkgTaskInfo>> vlTasksFinished;//已完成

	boost::shared_ptr<comm::PkgTaskSetSortFlagReq> pPkgTaskSetSortFlagReq = boost::make_shared<comm::PkgTaskSetSortFlagReq>();
	BOOST_FOREACH(boost::shared_ptr<comm::PkgTaskInfo> p, vectTasks){
		if (p->sortflag() > 6 || p->sortflag() < 0){//非法数据，容错，一般不可能执行
			p->set_sortflag(0);

			comm::PkgTaskSortFlag*pPkgTaskSortFlag = pPkgTaskSetSortFlagReq->add_tasksortflaglist();
			pPkgTaskSortFlag->set_taskid(p->id());
			pPkgTaskSortFlag->set_sortflag(0);
		}			


		if (!isMyTaskNotFinished(p, pPkgSessionUser)){
			if (p->sortflag() != 0){
				p->set_sortflag(0);
				comm::PkgTaskSortFlag*pPkgTaskSortFlag = pPkgTaskSetSortFlagReq->add_tasksortflaglist();
				pPkgTaskSortFlag->set_taskid(p->id());
				pPkgTaskSortFlag->set_sortflag(0);
			}

			vlTasksFinished.push_back(p);
		}
		else
		{//关注未完成
			vlTasks[p->sortflag()].push_back(p);
		}

		
	}

	std::vector<boost::shared_ptr<comm::PkgTaskInfo>> sortedTasks;
	//处理顺序放入sortedTasks
	//处理有flag的数据，1~6
	for (int i = 1; i <= 6; i++){
		std::list<boost::shared_ptr<comm::PkgTaskInfo>> &lstFlag = vlTasks[i];
		if (lstFlag.size() > 1){
			//有重复需要处理
			//Sort list with updatetime
			while (sortedTasks.size() < i - 1 && !lstFlag.empty()){
				//把后面的提前
				boost::shared_ptr<comm::PkgTaskInfo> pTaskBack = lstFlag.back();
				sortedTasks.push_back(pTaskBack);
				lstFlag.pop_back();
			}
				
		}
			
		while (!lstFlag.empty()){
			sortedTasks.push_back(lstFlag.front());
			lstFlag.pop_front();
		}			
	}
	//处理无Flag的数据，即原本为0的数据，直接追加
	std::list<boost::shared_ptr<comm::PkgTaskInfo>> &lstFlag0 = vlTasks[0];
	while (!lstFlag0.empty()){
		sortedTasks.push_back(lstFlag0.front());
		lstFlag0.pop_front();
	}

	//重新归纳顺序
	int i = 1;
	BOOST_FOREACH(boost::shared_ptr<comm::PkgTaskInfo> p, sortedTasks){
		int newflag = i > 6 ? 0 : i;
		i++;

		if (newflag != p->sortflag()){
			p->set_sortflag(newflag);

			comm::PkgTaskSortFlag*pPkgTaskSortFlag = pPkgTaskSetSortFlagReq->add_tasksortflaglist();
			pPkgTaskSortFlag->set_taskid(p->id());
			pPkgTaskSortFlag->set_sortflag(newflag);

		}
	}

	//追加已完成数据
	BOOST_FOREACH(boost::shared_ptr<comm::PkgTaskInfo> p, vlTasksFinished){
		sortedTasks.push_back(p);
	}
	//交换数据
	vectTasks.swap(sortedTasks);

	if (pPkgTaskSetSortFlagReq->tasksortflaglist_size() > 0){
		//设置更新的数据到数据库
		boost::shared_ptr<comm::PkgTaskSetSortFlagReply> pPkgTaskSetOrderReply = boost::make_shared<comm::PkgTaskSetSortFlagReply>();
		handleMessage_taskSetSortFlag(pPkgTaskSetSortFlagReq, pPkgTaskSetOrderReply, pPkgSessionUser);
	}

}

bool DBSessionComm::handleMessage_taskSetSortFlag(boost::shared_ptr<comm::PkgTaskSetSortFlagReq> pPkgTaskSetOrderReq, boost::shared_ptr<comm::PkgTaskSetSortFlagReply> pPkgTaskSetOrderReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	soci::session sql(*dbUtil_.getConnection());
	std::string sSQLUpdate;
	std::string curdatetime = UtilTime::getCurrentTime();
	for (int i = 0; i < pPkgTaskSetOrderReq->tasksortflaglist_size(); i++)
	{
		const comm::PkgTaskSortFlag& t = pPkgTaskSetOrderReq->tasksortflaglist().Get(i);
		sSQLUpdate = "UPDATE task SET updatetime = '" + curdatetime + "',sortflag = " + boost::lexical_cast<std::string>(t.sortflag()) + " WHERE id = " + boost::lexical_cast<std::string>(t.taskid());
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskSetSortFlag),SQL:" << sSQLUpdate ;
		sql << sSQLUpdate;
	}
	pPkgTaskSetOrderReply->set_issuccess(true);
	return bSuc;

}

bool DBSessionComm::handleMessage_taskOperation(boost::shared_ptr<comm::PkgTaskOperationReq> pPkgTaskOperationReq, boost::shared_ptr<comm::PkgTaskOperationReply> pPkgTaskOperationReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	soci::session sql(*dbUtil_.getConnection());
	const comm::PkgTaskInfo& cpkgTaskInfo = pPkgTaskOperationReq->taskinfo();
	comm::PkgTaskInfo pkgTaskInfo(cpkgTaskInfo);
	std::string curdatetime = UtilTime::getCurrentTime();
	pPkgTaskOperationReply->set_issuccess(true);
	if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_INSERT_INFO 
		|| pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_INSERT_INFO_COWORK 
		|| pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_INSERT_INFO_SUBTASKINFO)
	{
		if (pkgTaskInfo.name().empty())
		{
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.tasknamenotnull"));
			return false;
		}
		if (pkgTaskInfo.name().size() > 500 || pkgTaskInfo.content().size() > 1000){
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "insert task content too long:[" << pkgTaskInfo.name() << "],[" << pkgTaskInfo.content() << "]";

			return true;
		}



		try
		{
			std::string sSQLInsert;
			pkgTaskInfo.set_createtime(curdatetime);

			int taskid = pPkgTaskOperationReq->taskid();
			if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_INSERT_INFO)
			{
				if (taskid > 0)
				{
					pPkgTaskOperationReply->set_issuccess(false);
					pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
					return false;
				}

				{
					comm::PkgTaskInfo& pkgTmp = pkgTaskInfo;
					std::stringstream ss;
					std::string task_name_encoded_tmp;
					std::string task_content_encoded_tmp;
					encodeString_Hex(pkgTmp.name(), task_name_encoded_tmp);
					encodeString_Hex(pkgTmp.content(), task_content_encoded_tmp);
					ss << "INSERT INTO task(partentid,taskdate,name,content,tasklevel,createuserid,createtime,updatetime,sortflag,status) VALUES("
						<< pkgTmp.parentid() << ",'" << pkgTmp.taskdate() << "','" << task_name_encoded_tmp << "','" << task_content_encoded_tmp
						<< "'," << pkgTmp.tasklevel() << "," << pPkgSessionUser->userid() << ",'" << curdatetime << "','" << curdatetime << "'," << pkgTmp.sortflag()
						<< "," << pkgTmp.status() << ")";
					sSQLInsert = ss.str();
					BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLInsert ;

					sql << sSQLInsert;
				}
				//取得刚刚插入的id，可能有简单办法，需找时间研究
				std::string sSQLSelect = "SELECT id FROM task WHERE createtime = '" + curdatetime + "' AND createuserid = " + boost::lexical_cast<std::string>(pPkgSessionUser->userid());
				soci::indicator ind(soci::indicator::i_null);
				BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLSelect ;

				sql << sSQLSelect, into(taskid, ind);
				if (ind == soci::indicator::i_ok)
				{
					pPkgTaskOperationReply->set_issuccess(true);
					pPkgTaskOperationReply->set_taskid(taskid);
				}
				else
				{
					pPkgTaskOperationReply->set_issuccess(false);
					pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.inserterror"));
					return false;
				}
				pkgTaskInfo.set_id(taskid);
			}

			if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_INSERT_INFO_SUBTASKINFO)
			{//插入子任务
				for (int i = 0; i < pPkgTaskOperationReq->taskinfolist_size(); i++)
				{
					const comm::PkgTaskInfo& cpkgTaskInfoSub = pPkgTaskOperationReq->taskinfolist().Get(i);
					comm::PkgTaskInfo pkgTaskInfoSub(cpkgTaskInfoSub);
					pkgTaskInfoSub.set_parentid(taskid);
					{
						comm::PkgTaskInfo& pkgTmp = pkgTaskInfoSub;
						std::string task_name_encoded_tmp;
						std::string task_content_encoded_tmp;
						encodeString_Hex(pkgTmp.name(), task_name_encoded_tmp);
						encodeString_Hex(pkgTmp.content(), task_content_encoded_tmp);

						std::stringstream ss;
						ss << "INSERT INTO task(partentid,taskdate,name,content,tasklevel,createuserid,createtime,updatetime,sortflag,status) VALUES("
							<< pkgTmp.parentid() << ",'" << pkgTmp.taskdate() << "','" << task_name_encoded_tmp << "','" << task_content_encoded_tmp
								<< "'," << pkgTmp.tasklevel() << "," << pkgTmp.createuserid() << ",'" << curdatetime << "','" << curdatetime << "'," << pkgTmp.sortflag()
								<< "," << pkgTmp.status() << ")";
						sSQLInsert = ss.str();
						BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLInsert ;

						sql << sSQLInsert;
						//sql << sSQLInsert, soci::use(pkgTaskInfoSub);
					}
				}
			}
			
			if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_INSERT_INFO_COWORK)
			{
				//如果是重新分配给其他人,则需要考虑条件:
				if (pPkgTaskOperationReq->taskid() > 0)
				{
					//是否自己是任务创建者？
					{
						comm::PkgTaskInfo task;
						if (!getTaskInfo(task, pPkgTaskOperationReq->taskid(),sql))
						{//task不存在
							pPkgTaskOperationReply->set_issuccess(false);
							pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
							BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "(taskOperation),cannot find task id:" << pPkgTaskOperationReq->taskid();
							return true;
						}
						
						if (task.createuserid() != pPkgSessionUser->userid()){
							//自己非创建者，无权限做此操作
							pPkgTaskOperationReply->set_issuccess(false);
							pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
							BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "(taskOperation),not createuser self:" << task.createuserid();
							return true;
						}
					}
					//1、接受者是否已接受任务？
					std::string sSQLSelectTask = "SELECT * FROM task WHERE coworkid = " + boost::lexical_cast<std::string>(pPkgTaskOperationReq->taskid())
						+ " AND createuserid <> accepteduserid AND accepteduserid <> 0 AND status <> " + boost::lexical_cast<std::string>(comm::PkgTaskInfo::TS_DELETED);
					comm::PkgTaskInfo task;
					soci::statement st = (sql.prepare << sSQLSelectTask, soci::into(task));
					st.execute();
					while (st.fetch())
					{//找到接受者
						if (task.status() == comm::PkgTaskInfo::TS_GOING)//已经接受，返回失败
						{
							pPkgTaskOperationReply->set_issuccess(false);
							pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.tasknoassgin"));
							return false;
						}
						else if (task.status() == comm::PkgTaskInfo::TS_DENY || task.status() == comm::PkgTaskInfo::TS_NEW)//尚未接受或拒绝
						{
							//DELETE.删除接受者
							std::string sSQLDelete = "UPDATE task SET updatetime = '" +curdatetime + "',status = " + boost::lexical_cast<std::string>(comm::PkgTaskInfo::TS_DELETED) + " WHERE coworkid <> id AND coworkid = " + boost::lexical_cast<std::string>(task.id());
							BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLDelete;

							sql << sSQLDelete;
							//以前的接受者需要更新
							if (task.accepteduserid() > 0){
								updateUserUpdateFlags(task.accepteduserid(), comm::UF_TASKCOWORK | comm::UF_MYTASK, sql);
							}

						}
					}
				}
			
				//新分配更新cowork id
				int acceptuserid = 0;
				std::vector<int> vectNotifyUserIDs;
				pkgTaskInfo.set_coworkid(taskid);
				pkgTaskInfo.set_createuserid(pPkgSessionUser->userid());
				for (int i = 0; i < pPkgTaskOperationReq->accepteduseridlist_size(); i++)
				{
				    acceptuserid = pPkgTaskOperationReq->accepteduseridlist().Get(i);
					vectNotifyUserIDs.push_back(acceptuserid);
					pkgTaskInfo.set_accepteduserid(acceptuserid);
					{
						comm::PkgTaskInfo& pkgTmp = pkgTaskInfo;
						std::stringstream ss;
						std::string task_name_encoded_tmp;
						std::string task_content_encoded_tmp;
						encodeString_Hex(pkgTmp.name(), task_name_encoded_tmp);
						encodeString_Hex(pkgTmp.content(), task_content_encoded_tmp);

						ss << "INSERT INTO task(partentid,taskdate,name,content,tasklevel,createuserid,createtime,updatetime,sortflag,status,accepteduserid,coworkid) VALUES("
							<< pkgTmp.parentid() << ",'" << pkgTmp.taskdate() << "','" << task_name_encoded_tmp << "','" << task_content_encoded_tmp
							<< "'," << pkgTmp.tasklevel() << "," << pPkgSessionUser->userid() << ",'" << curdatetime << "','" << curdatetime << "'," << pkgTmp.sortflag()
							<< "," << pkgTmp.status() << "," << acceptuserid << "," << taskid << ")";
						sSQLInsert = ss.str();
						BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLInsert ;
						sql << sSQLInsert;
					}
				}
				//更新原任务的accepteduserid和coworkid
				std::string sSQLUpdate = "UPDATE task SET updatetime = '" + curdatetime + "',coworkid = " + boost::lexical_cast<std::string>(taskid)+",accepteduserid = " +
					boost::lexical_cast<std::string>(acceptuserid)+" WHERE id = " + boost::lexical_cast<std::string>(taskid);
				BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLUpdate ;
				sql << sSQLUpdate;
				
				updateCoworkTaskReaded(pkgTaskInfo, 0, pPkgSessionUser->userid(), sql);
				//updateUserUpdateFlags(vectNotifyUserIDs, comm::UF_TASKCOWORK,sql);			

			}
			onSortCurTask(taskid, pPkgSessionUser,sql);
		}
		catch (soci::soci_error & e)
		{
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(e.what());
			throw e;
		}
		catch (...)
		{
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.inserterror"));
		}
		updateUserUpdateFlags(pPkgSessionUser->userid(), comm::UF_MYTASKFINISHEDNUMBERS, sql);
	}
	
	if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_UPDATE_INFO)
	{
		//判断任务内容修改条件
		comm::PkgTaskInfo task;
		if (pkgTaskInfo.name().empty())
		{
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.tasknamenotnull"));
			return true;
		}
		if (pkgTaskInfo.name().size() > 150 || pkgTaskInfo.content().size() > 500){
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "task content is too long:[" << pkgTaskInfo.name() << "],[" << pkgTaskInfo.content() << "]";

			return true;
		}


		int taskid = pkgTaskInfo.id(); 
		if (taskid == 0){
			taskid = pPkgTaskOperationReq->taskid();
		}
		if (!getTaskInfo(task, taskid, sql))
		{//task不存在
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return true;
		}

		if (isTaskDisableEdit(pkgTaskInfo, pPkgSessionUser)){
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return true;
		}
		//***如果是TO_UPDATE_INFO，表示更新任务
		//普通任务，taskinfo有效，仅设置普通任务信息（字段包括:任务名、任务内容、时间、紧急度、排序）
		std::string oldDate;
		std::string newDate;
		if (!getTaskDateBeforeUpdate(pkgTaskInfo, oldDate, newDate, sql)){
			//任务可能已经被删除
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return true;
		}

		onUpdateTask(pkgTaskInfo, task, pPkgSessionUser->userid(), sql);

		if (task.coworkid() > 0){
			updateCoworkTaskReaded(task, 0, pPkgSessionUser->userid(), sql);
		}

		if (!oldDate.empty()){
			sortDateTask(oldDate, pPkgSessionUser);
		}

		if (!newDate.empty()){
			sortDateTask(newDate, pPkgSessionUser);
		}

		pPkgTaskOperationReply->set_taskid(pkgTaskInfo.id());
		pPkgTaskOperationReply->set_issuccess(true);
	}
	
	if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_UPDATE_STATUS)
	{
		comm::PkgTaskInfo task;
		if (!getTaskInfo(task, pPkgTaskOperationReq->taskid(), sql))
		{
			//task不存在
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return true;
		}
	
		if (isTaskDisableUpdateStatus(task, pPkgTaskOperationReq->updatestatus(),pPkgSessionUser)){
			//业务上禁止更新状态
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return true;
		}

		std::stringstream ss;
		std::string curdatetime = UtilTime::getCurrentTime();

		if (task.coworkid() > 0)
		{//协作任务的更新状态,需更新所有相关的状态	

			std::string remark;
			if (pPkgTaskOperationReq->has_reason()){
				remark = pPkgTaskOperationReq->reason();
			}

			if (pPkgTaskOperationReq->updatestatus() == comm::PkgTaskInfo::TS_DENY){
				std::string task_remark_encoded;
				encodeString_Hex(remark, task_remark_encoded);
				ss << "UPDATE task SET status = " << pPkgTaskOperationReq->updatestatus() << ",updatetime = '" << curdatetime 
					<< "',finishtime = '" << curdatetime << "',remark ='" << task_remark_encoded << "' WHERE coworkid = " << task.coworkid();
			}
			else if (pPkgTaskOperationReq->updatestatus() == comm::PkgTaskInfo::TS_FINISHED){
				ss << "UPDATE task SET status = " << pPkgTaskOperationReq->updatestatus() << ",updatetime = '" << curdatetime << "',finishtime = '" << curdatetime << "' WHERE coworkid = " << task.coworkid();
			}
			else{
				ss << "UPDATE task SET status = " << pPkgTaskOperationReq->updatestatus() << ",updatetime = '" << curdatetime << "' WHERE coworkid = " << task.coworkid();
			}

			updateCoworkTaskReaded(task, 0, pPkgSessionUser->userid(), sql);
			//get cowork creatid,acceptlistid.
			//int createid;
			//std::vector<int> acceptlist;
			//getCoworkUserIds(task.coworkid(), createid, acceptlist);
			//updateUserUpdateFlags(createid, comm::UF_TASKCOWORK | comm::UF_MYTASK,sql);
			//updateUserUpdateFlags(acceptlist, comm::UF_TASKCOWORK | comm::UF_MYTASK,sql);
		}
		else
		{		
			if (pPkgTaskOperationReq->updatestatus() == comm::PkgTaskInfo::TS_DENY || pPkgTaskOperationReq->updatestatus() == comm::PkgTaskInfo::TS_FINISHED)
			{
				ss << "UPDATE task SET status = " << pPkgTaskOperationReq->updatestatus() << ",updatetime = '" << curdatetime << "',finishtime = '" << curdatetime << "' WHERE id = " << pPkgTaskOperationReq->taskid();
			}
			else
			{
				ss << "UPDATE task SET status = " << pPkgTaskOperationReq->updatestatus() << ",updatetime = '" << curdatetime << "' WHERE id = " << pPkgTaskOperationReq->taskid();
			}

		}
		updateUserUpdateFlags(pPkgSessionUser->userid(), comm::UF_MYTASKFINISHEDNUMBERS, sql);
		std::string sSQLUpdate;
		sSQLUpdate = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLUpdate ;

		sql << sSQLUpdate;
		pPkgTaskOperationReply->set_taskid(pPkgTaskOperationReq->taskid());
		pPkgTaskOperationReply->set_issuccess(true);
		onSortCurTask(pPkgTaskOperationReq->taskid(), pPkgSessionUser,sql);
	}		
	
	if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_DELETE_INFO)
	{
		if (pPkgTaskOperationReq->taskid() == 0)
		{
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.taskidnotnull"));
			return true;
		}
		std::stringstream ss;
		std::string sSQLDelete;
		ss << "UPDATE task SET updatetime = '" + curdatetime + "',status = " << comm::PkgTaskInfo::TS_DELETED << " WHERE id = " << pPkgTaskOperationReq->taskid();
		
	//	ss << "DELETE FROM task WHERE id = " << pPkgTaskOperationReq->taskid();
		sSQLDelete = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLDelete ;

		sql << sSQLDelete;
	
		pPkgTaskOperationReply->set_taskid(pPkgTaskOperationReq->taskid());
		pPkgTaskOperationReply->set_issuccess(true);
		updateUserUpdateFlags(pPkgSessionUser->userid(), comm::UF_MYTASKFINISHEDNUMBERS, sql);

		onSortCurTask(pPkgTaskOperationReq->taskid(), pPkgSessionUser,sql);
	}
	
	if (pPkgTaskOperationReq->taskoperation() & comm::PkgTaskOperationReq::TO_UPDATE_READED)
	{
		comm::PkgTaskInfo task;
		if (!getTaskInfo(task, pPkgTaskOperationReq->taskid(), sql))
		{
			//task不存在
			pPkgTaskOperationReply->set_issuccess(false);
			pPkgTaskOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
			return true;
		}
		std::stringstream ss;
		//updateCoworkTaskReaded(task, pPkgTaskOperationReq->updatereaded(), pPkgSessionUser->userid());
		ss << "UPDATE task SET updatetime = '" + curdatetime + "',isreaded = " << pPkgTaskOperationReq->updatereaded() << " WHERE id = " << task.id();
		std::string sSQLUpdate = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(taskOperation),SQL:" << sSQLUpdate;

		sql << sSQLUpdate;

	}
	return bSuc;

}

bool DBSessionComm::updateCoworkTaskReaded(comm::PkgTaskInfo& pkgTaskInfo, int isreaded, int nUserid, soci::session& sql)
{
	std::string sqlStr;
	std::stringstream ss;
	std::string curdatetime = UtilTime::getCurrentTime();
	if (pkgTaskInfo.coworkid() > 0 && isreaded == 0){
		//更新对方
		if (pkgTaskInfo.createuserid() == nUserid){
			ss << "UPDATE task SET  updatetime = '" + curdatetime + "',isreaded = " << isreaded << " WHERE id <> coworkid AND coworkid = " << pkgTaskInfo.coworkid();
			std::vector<int> acceptlist;
			getCoworkUserIds(pkgTaskInfo.coworkid(), nUserid, acceptlist,sql);
			updateUserUpdateFlags(acceptlist, comm::UF_TASKCOWORK | comm::UF_MYTASK, sql);
		}
		else if (pkgTaskInfo.accepteduserid() == nUserid){
			ss << "UPDATE task SET  updatetime = '" + curdatetime + "',isreaded = " << isreaded << " WHERE id = coworkid AND coworkid = " << pkgTaskInfo.coworkid();
			updateUserUpdateFlags(pkgTaskInfo.createuserid(), comm::UF_TASKCOWORK | comm::UF_MYTASK, sql);
		}
		else{
			BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "(updateCoworkTaskReaded),no possible:" << pkgTaskInfo.name() << ",id:" << pkgTaskInfo.id();
		}
		sqlStr = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(updateCoworkTaskReaded),SQL:" << sqlStr;
		sql << sqlStr;

		
	}
	else{
		BOOST_LOG_SEV(gLog_, boost::log::trivial::error) << "(updateCoworkTaskReaded),no possible:" << pkgTaskInfo.name() << ",id:" << pkgTaskInfo.id();
	}
	return true;
}

void DBSessionComm::getCoworkUserIds(int coworktaskid, int&createuserid, std::vector<int>&vectAcceptsIds, soci::session& sql)
{
	std::string sSQLSelectTask = "SELECT * FROM task WHERE coworkid = " + boost::lexical_cast<std::string>(coworktaskid);
	comm::PkgTaskInfo task;
	soci::statement st = (sql.prepare << sSQLSelectTask, soci::into(task));
	st.execute();
	while (st.fetch())
	{//找到接受者
		if (task.id() == coworktaskid)
		{
			createuserid = task.createuserid();
		}
		else
		{
			vectAcceptsIds.push_back(task.accepteduserid());
		}
	}
}

bool DBSessionComm::handleMessage_groupQuery(boost::shared_ptr<comm::PkgCompanyGroupQueryReq> pPkgCompanyGroupQueryReq, boost::shared_ptr<comm::PkgCompanyGroupQueryReply> pPkgCompanyGroupQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;	
	pPkgCompanyGroupQueryReply->set_issuccess(true);

	soci::session sql(*dbUtil_.getConnection());
	
	int companyid = pPkgSessionUser->user().companyid();
	if (companyid == 0)
	{
		pPkgCompanyGroupQueryReply->set_issuccess(false);
		pPkgCompanyGroupQueryReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.nocompany"));
		return true;
	}

	typedef boost::tuple<int, std::string> BTUPLE;
	std::vector<comm::PkgGroupUser> vectGroupUsers;
	/*先获取所有用户列表*/
	{
		std::vector<::session::PkgUser> vectUsers;
		getCompanyUsers(companyid, vectUsers,sql);
		BOOST_FOREACH(::session::PkgUser&pkgUser, vectUsers)
		{
			comm::PkgGroupUser guser;
			guser.set_groupid(0);
			
			guser.set_userid(pkgUser.userid());
			guser.set_realname(pkgUser.realname());
			guser.set_phonenumber(pkgUser.phonenumber());
			guser.set_emailaddress(pkgUser.emailaddress());
			guser.set_permissionroleid(pkgUser.permissionroleid());
			guser.set_createtime(pkgUser.createtime());
			boost::shared_ptr<::session::PkgRoleUser> pPkgRoleUser = mapRoles[pkgUser.permissionroleid()];
			if (pPkgRoleUser)
			{
				guser.set_permission(pPkgRoleUser->permission());
				guser.set_rolename(pPkgRoleUser->rolename());
			}
			vectGroupUsers.push_back(guser);
		}

	}
	/*获取所有组列表*/
	{
		std::map<int, int> mapUserGroup;
		typedef boost::tuple<int, int> BTUPLEGROUPUSER;
		std::string sSQLSelect = "SELECT groupid,userid FROM groupuser WHERE companyid = " + boost::lexical_cast<std::string>(companyid);
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(groupQuery),SQL:" << sSQLSelect ;
		soci::rowset<BTUPLEGROUPUSER> rows = sql.prepare << sSQLSelect;
		soci::rowset<BTUPLEGROUPUSER>::const_iterator it = rows.begin();
		while (it != rows.end())
		{
			const BTUPLEGROUPUSER& dbvalues = *it;
			int groupid = dbvalues.get<0>();
			int userid = dbvalues.get<1>();
			mapUserGroup[userid] = groupid;
			it++;
		}

		//update vectGroupUsers
		BOOST_FOREACH(comm::PkgGroupUser& guser, vectGroupUsers){
			if (mapUserGroup.find(guser.userid()) != mapUserGroup.end())
			{
				guser.set_groupid(mapUserGroup[guser.userid()]);
			}
			comm::PkgGroupUser *pPkgGroupUser = pPkgCompanyGroupQueryReply->add_groupuserlist();
			pPkgGroupUser->CopyFrom(guser);
		}
	}


	std::string sqlWhereCondition;
	std::string sSQLSelect = "SELECT id,name FROM companygroup WHERE companyid = " + boost::lexical_cast<std::string>(companyid);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(groupQuery),SQL:" << sSQLSelect ;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		int groupid = dbvalues.get<0>();
		std::string groupname = dbvalues.get<1>();

		comm::PkgCompanyGroup *pPkgCompanyGroup = pPkgCompanyGroupQueryReply->add_companygrouplist();
		pPkgCompanyGroup->set_companyid(companyid);
		pPkgCompanyGroup->set_groupid(groupid);
		pPkgCompanyGroup->set_groupname(groupname);		

		it++;
	}
	return bSuc;

}
std::string DBSessionComm::generateAuthCode()
{
	std::string sAuth;
	boost::mt19937 rng((int)time(NULL)); 
	for (int i = 0; i < 6; i++)
	{
		char idigit = rng() % 10;//0~9
		char c = '0' + idigit;
		sAuth.push_back(c);
	}
	
	return sAuth;
}

bool DBSessionComm::handleMessage_advQuery(boost::shared_ptr<comm::PkgAdvQueryReq> pPkgAdvQueryReq, boost::shared_ptr<comm::PkgAdvQueryReply> pPkgAdvQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string curdatetime = UtilTime::getCurrentTime();
	std::string sSQLSelect;
	std::string sqlWhereCondition;
	std::string sqlWhereS, sqlTail;
	std::vector<std::string> vectConditions;

	if (pPkgAdvQueryReq->queryflag() & comm::PkgAdvQueryReq::QT_SPNUM)
	{//末尾
		sqlTail = " LIMIT " + boost::lexical_cast<std::string>(pPkgAdvQueryReq->numreq());
	}
	
	if (pPkgAdvQueryReq->queryflag() & comm::PkgAdvQueryReq::QT_VAILDTIME)
	{
		sqlWhereS = "expiretime > '" + curdatetime + "'";
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgAdvQueryReq->queryflag() & comm::PkgAdvQueryReq::QT_ADVID)
	{
		sqlWhereS = "id = " + boost::lexical_cast<std::string>(pPkgAdvQueryReq->advid());
		vectConditions.push_back(sqlWhereS);
	}

	soci::session sql(*dbUtil_.getConnection());

	sqlWhereCondition.clear();
	for (size_t i = 0; i< vectConditions.size(); i++)
	{
		const std::string&sqlWhere = vectConditions[i];
		if (i == 0)
		{
			sqlWhereCondition = " WHERE ";
		}
		else
		{
			sqlWhereCondition += " AND ";
		}

		sqlWhereCondition += sqlWhere;
	}
	sqlWhereCondition += sqlTail;
	std::string sUrlBase;
	sSQLSelect = "SELECT msginfo FROM sysinfo WHERE type = 'advurlbase'";
	soci::indicator ind(soci::indicator::i_null);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(advQuery),SQL:" << sSQLSelect ;

	sql << sSQLSelect, into(sUrlBase, ind);
	if (ind != soci::indicator::i_ok)
	{
		pPkgAdvQueryReply->set_issuccess(false);
		pPkgAdvQueryReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.noadvrulbase"));
		return true;
	}

	sSQLSelect = "SELECT * FROM adv" + sqlWhereCondition;
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(advQuery),SQL:" << sSQLSelect ;

	comm::PkgAdvInfo aif;
	soci::statement st = (sql.prepare << sSQLSelect, soci::into(aif));
	st.execute();
	while (st.fetch())
	{
		comm::PkgAdvInfo * p = pPkgAdvQueryReply->add_advinfolist();
		std::string url= sUrlBase + boost::lexical_cast<std::string>(aif.id());
		aif.set_urlpic(url);
		*p = aif;
	}
	pPkgAdvQueryReply->set_issuccess(true);

	return true;
}



bool DBSessionComm::handleMessage_sysQueryMyProfile(boost::shared_ptr<comm::PkgQueryMyProfileReq> pPkgQueryMyProfileReq, boost::shared_ptr<comm::PkgQueryMyProfileReply> pPkgQueryMyProfileReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	//关于我的信息
	std::string versionlastest;
	std::string versiondownloadurl;
	getLastestVersion(pPkgQueryMyProfileReq->platmform(), versionlastest, versiondownloadurl);
	
	pPkgQueryMyProfileReply->set_versionlastest(versionlastest);
	pPkgQueryMyProfileReply->set_versiondownloadurl(versiondownloadurl);

	//关于任务
	if (pPkgSessionUser->userid() > 0)
	{
		int taskFinished = 0;
		int taskNotFinished = 0;
		getTaskNumber(pPkgSessionUser, taskFinished, taskNotFinished);
		pPkgQueryMyProfileReply->set_tasknumberfinshed(taskFinished);
		pPkgQueryMyProfileReply->set_tasknumbernotfinished(taskNotFinished);

	}
	return true;
}
void DBSessionComm::getLastestVersion(const std::string&platform, std::string &versionlastest, std::string &versiondownloadurl)
{
	//关于版本信息
	soci::session sql(*dbUtil_.getConnection());
	std::map<std::string, std::string> mapSysInfo;
	getSysinfoMap(mapSysInfo,sql);
	versionlastest = mapSysInfo["versionlatest"];
	versiondownloadurl = mapSysInfo["versiondownloadurl"];
	if (platform == "android")
	{
		if (!mapSysInfo["versionlatest_android"].empty())
		{
			versionlastest = mapSysInfo["versionlatest_android"];
		}
		if (!mapSysInfo["versiondownloadurl_android"].empty())
		{
			versiondownloadurl = mapSysInfo["versiondownloadurl_android"];
		}
	}
	else if (platform == "ios")
	{
		if (!mapSysInfo["versionlatest_ios"].empty())
		{
			versionlastest = mapSysInfo["versionlatest_ios"];
		}
		if (!mapSysInfo["versiondownloadurl_ios"].empty())
		{
			versiondownloadurl = mapSysInfo["versiondownloadurl_ios"];
		}
	}
}

void DBSessionComm::getTaskNumber(boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser, int&taskFinished, int&taskNotFinished)
{
	if (pPkgSessionUser->userid() > 0)
	{
		//所有已完成任务
		boost::shared_ptr<comm::PkgTaskPageQueryReq> pPkgTaskPageQueryReq = boost::make_shared<comm::PkgTaskPageQueryReq>();
		comm::PkgTaskPageQueryReq::PkgTaskQueryCondition* pPkgTaskQueryCondition = new comm::PkgTaskPageQueryReq::PkgTaskQueryCondition;
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_FINISHED);
		pPkgTaskPageQueryReq->set_allocated_pkgtaskquerycondition(pPkgTaskQueryCondition);
		pPkgTaskPageQueryReq->set_pageflag(comm::PkgTaskPageQueryReq::P_ONLYNUM);
		pPkgTaskPageQueryReq->set_taskflag(comm::PkgTaskPageQueryReq::TF_MYTASK);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS);
		boost::shared_ptr<comm::PkgTaskPageQueryReply> pPkgTaskPageQueryReply = boost::make_shared<comm::PkgTaskPageQueryReply>();
		handleMessage_taskPageQuery(pPkgTaskPageQueryReq, pPkgTaskPageQueryReply, pPkgSessionUser);
		taskFinished = pPkgTaskPageQueryReply->returnnum();
		//所有未完成任务= 状态为新建或已接受的
		pPkgTaskQueryCondition->clear_taskstatuslist();
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_NEW);
		pPkgTaskQueryCondition->add_taskstatuslist(comm::PkgTaskInfo::TS_GOING);
		pPkgTaskPageQueryReq->set_enconditon(comm::PkgTaskPageQueryReq::TQC_TASKSTATUS);
		handleMessage_taskPageQuery(pPkgTaskPageQueryReq, pPkgTaskPageQueryReply, pPkgSessionUser);
		taskNotFinished = pPkgTaskPageQueryReply->returnnum();
	}
}


void DBSessionComm::getSysinfoMap(std::map<std::string, std::string>&mapSysInfo, soci::session& sql)
{
	std::string sSQLSelect = "SELECT type,msginfo FROM sysinfo";
	typedef boost::tuple<std::string, std::string> BTUPLE;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		std::string type = dbvalues.get<0>();
		std::string keyvalue = dbvalues.get<1>();
	
		mapSysInfo[type] = keyvalue;
		it++;
	}
}


//FOR　NOTIFY
bool DBSessionComm::getUserNotify(int userid, userInfo&user, soci::session& sql)
{
	typedef boost::tuple<std::string, std::string, int, std::string> BTUPLENOTIFYUSER;
	std::string sSQLSelect = "SELECT loginsn,lastlogintime,notifyupdatedflag,updatetime FROM usernotify WHERE userid = " + boost::lexical_cast<std::string>(userid);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getUserNotify),SQL:" << sSQLSelect;
	soci::rowset<BTUPLENOTIFYUSER> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLENOTIFYUSER>::const_iterator it = rows.begin();
	if (it != rows.end())
	{
		const BTUPLENOTIFYUSER& dbvalues = *it;
		user.sLoginSn = dbvalues.get<0>();
		user.sLastLoginTime = dbvalues.get<1>();
		user.notifyupdatedflag = dbvalues.get<2>();
		user.sUpdatedTime = dbvalues.get<3>();
		return true;
	}
	return false;
}

void DBSessionComm::setUserNotify(int userid, userInfo&user, bool bupdate /*= true*/, soci::session& sql)
{
	std::string sqlStr;
	std::stringstream ss;
	if (bupdate)
	{
		ss << "UPDATE usernotify SET loginsn = '" << user.sLoginSn << "',lastlogintime = '" << user.sLastLoginTime << "',notifyupdatedflag = "
			<< user.notifyupdatedflag << ",updatetime = '" << user.sUpdatedTime << "' WHERE userid = " << userid;
	}
	else
	{
		ss << "INSERT INTO usernotify(userid,loginsn,lastlogintime,notifyupdatedflag,updatetime) VALUES("
			<< userid << ",'" << user.sLoginSn << "','" << user.sLastLoginTime << "'," << user.notifyupdatedflag
			<< ",'" << user.sUpdatedTime << "')";
	}
	sqlStr = ss.str();
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(setUserNotify),SQL:" << sqlStr;
	sql << sqlStr;
}

void DBSessionComm::setUserLoginSn(int userid, const std::string&sLoginSn)
{
	soci::session sql(*dbUtil_.getConnection());
	userInfo user;
	bool bSuc = getUserNotify(userid, user, sql);
	user.sLastLoginTime = UtilTime::getCurrentTime();
	user.sLoginSn = sLoginSn;
	user.notifyupdatedflag = 0;
	user.sUpdatedTime = "";
	setUserNotify(userid, user, bSuc, sql);
}

void DBSessionComm::isSessionUserVaildWithException(boost::shared_ptr<session::PkgSessionUser>pPkgSessionUser)
{//直接抛出异常
	if (pPkgSessionUser->userid() == 0)
	{
		std::string serr = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.logonfirst");
		throw nodeexception() << err_no(ERROR_SESSION) << errtype_str("sessiontimeout") << err_str(serr);
		//throw std::exception("sessiontimeout");
	}
	soci::session sql(*dbUtil_.getConnection());

	userInfo user;
	getUserNotify(pPkgSessionUser->userid(), user, sql);
	if (pPkgSessionUser->loginsn() != user.sLoginSn)
	{
		std::string serr = UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.errdata.logoutsomewhere");
		throw nodeexception() << err_no(ERROR_SESSION) << errtype_str("sessionlogout") << err_str(serr);
		//throw std::exception("sessionlogout");
	}
}

int DBSessionComm::getUserUpdateFlag(int userid, std::string&sUpdatedTime)
{
	soci::session sql(*dbUtil_.getConnection());

	userInfo user;
	bool bSuc = getUserNotify(userid, user,sql);

	if (user.sUpdatedTime == sUpdatedTime)
		return 0;
	sUpdatedTime = user.sUpdatedTime;
	int nUFlag = user.notifyupdatedflag;

	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "User:[" << userid << "],getUpdate:[" << nUFlag << "],UpdateTime:" << sUpdatedTime;
	return nUFlag;
}

void DBSessionComm::updateUserUpdateFlags(std::vector<int>&vectUsers, int nUFlag, soci::session& sql)
{
	BOOST_FOREACH(int &userid, vectUsers){
		updateUserUpdateFlags(userid, nUFlag, sql);
	}
}

void DBSessionComm::updateUserUpdateFlags(int userid, int nUFlag,soci::session& sql)
{
	userInfo user;
	bool bSuc = getUserNotify(userid, user,sql);
	user.notifyupdatedflag |= nUFlag;
	user.sUpdatedTime = UtilTime::getCurrentTime();
	setUserNotify(userid, user, bSuc,sql);

	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "NotifyUser:[" << userid << "],Update:[" << nUFlag << "],CurrentTime:" << user.sUpdatedTime;

}

void DBSessionComm::clearUserUpdateFlag(int userid, int nUFlag)
{
	userInfo user;
	soci::session sql(*dbUtil_.getConnection());

	bool bSuc = getUserNotify(userid, user,sql);
	user.notifyupdatedflag &= ~nUFlag;
	setUserNotify(userid, user, bSuc,sql);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "User:[" << userid << "],GetFlag:[" << nUFlag << "],CurrentTime:" << user.sUpdatedTime;
}