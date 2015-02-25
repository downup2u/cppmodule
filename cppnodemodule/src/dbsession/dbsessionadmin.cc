#include "dbsessionadmin.h"
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
#include <encryptstring.h>
//====================================


DBSessionAdmin::DBSessionAdmin()
{

}

DBSessionAdmin::~DBSessionAdmin()
{
}


bool DBSessionAdmin::initSession(size_t nPoolSize, const std::string& connectstring)
{
	std::string err;
	dbUtil_.initDB(nPoolSize, connectstring, err);
	return true;
}

bool DBSessionAdmin::exitSession(std::string&err)
{
	dbUtil_.exitDB();
	return true;
}



bool DBSessionAdmin::handleAdminMsg_userLogin(boost::shared_ptr<admin::PkgUserLoginReq> pPkgUserLoginReq, boost::shared_ptr<admin::PkgUserLoginReply> pPkgUserLoginReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	bool bSuc = false;
	try
	{
		typedef boost::tuple<int, std::string, std::string, int> BTUPLE;

		soci::session sql(*dbUtil_.getConnection());
		std::string sSQLSelect = "SELECT id,username,password,permission FROM adminuser WHERE username = '" + pPkgUserLoginReq->username() + "'";
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleAdminMsg_userLogin),SQL:" << sSQLSelect;

		soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
		soci::rowset<BTUPLE>::const_iterator it = rows.begin();
		if (it != rows.end())
		{
			const BTUPLE& dbvalues = *it;
			int id = dbvalues.get<0>();
			std::string username = dbvalues.get<1>();
			std::string password = dbvalues.get<2>();	
			int permission = dbvalues.get<3>();
			std::string adminuser_password_decoded;
			decodeString(password, adminuser_password_decoded);
			if (adminuser_password_decoded == pPkgUserLoginReq->password())
			{
				pPkgUserLoginReply->set_issuccess(true);
				pPkgUserLoginReply->set_userid(id);		
				pPkgUserLoginReply->set_permission(permission);

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
bool DBSessionAdmin::handleAdminMsg_userOperation(boost::shared_ptr<admin::PkgUserOperationReq> pPkgUserOperationReq, boost::shared_ptr<admin::PkgUserOperationReply> pPkgUserOperationReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	soci::session sql(*dbUtil_.getConnection());
	if (pPkgUserOperationReq->useroperation() == admin::PkgUserOperationReq::TO_INSERT_INFO)
	{
		const admin::PkgUserInfo&userInfo = pPkgUserOperationReq->userinfo();

		int userid = 0;
		std::string sSQLSelect = "SELECT id FROM adminuser WHERE username = '" + userInfo.username() + "' ORDER BY id DESC LIMIT 1";
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleAdminMsg_userOperation),SQL:" << sSQLSelect;
		soci::indicator ind(soci::indicator::i_null);
		sql << sSQLSelect, into(userid, ind);
		if (ind == soci::indicator::i_ok)
		{
			pPkgUserOperationReply->set_issuccess(false);
			pPkgUserOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.userexists"));
		}
		else
		{
			std::stringstream ss;
			std::string adminuser_password_encoded;
			encodeString(userInfo.password(), adminuser_password_encoded);
			ss << "INSERT INTO adminuser(username,password,permission) VALUES('"
				<< userInfo.username() << "','" << adminuser_password_encoded << "'," << userInfo.permission() << ")";
			std::string sSQLInsert = ss.str();
			BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleAdminMsg_userOperation),SQL:" << sSQLInsert;

			sql << sSQLInsert;
			//取得刚刚插入的id，可能有简单办法，需找时间研究
			userid = 0;
			std::string sSQLSelect = "SELECT id FROM adminuser WHERE username = '" + userInfo.username() + "' ORDER BY id DESC LIMIT 1";
			BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleAdminMsg_userOperation),SQL:" << sSQLSelect;

			ind = soci::indicator::i_null;
			sql << sSQLSelect, into(userid, ind);
			if (ind == soci::indicator::i_ok)
			{
				pPkgUserOperationReply->set_userid(userid);
				pPkgUserOperationReply->set_issuccess(true);
			}
			else
			{
				pPkgUserOperationReply->set_issuccess(false);
				pPkgUserOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.inserterror"));
			}
		}

	}
	else if (pPkgUserOperationReq->useroperation() == admin::PkgUserOperationReq::TO_UPDATE_INFO)
	{
		const admin::PkgUserInfo&userInfo = pPkgUserOperationReq->userinfo();
		std::stringstream ss;
		std::string sSQLUpdate;
		soci::session sql(*dbUtil_.getConnection());
		std::string curdatetime = UtilTime::getCurrentTime();
		std::string adminuser_password_encoded;
		encodeString(userInfo.password(), adminuser_password_encoded);
		ss << "UPDATE adminuser SET password = '" << adminuser_password_encoded << "',permission = " << userInfo.permission()
			<< " WHERE username = '" + userInfo.username() + "'";
		sSQLUpdate = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleAdminMsg_userOperation),SQL:" << sSQLUpdate;
		sql << sSQLUpdate;
		pPkgUserOperationReply->set_issuccess(true);
	}
	else if (pPkgUserOperationReq->useroperation() == admin::PkgUserOperationReq::TO_DELETE_INFO)
	{
		std::string sSQLDELETE;
		sSQLDELETE = "DELETE FROM adminuser WHERE id = " + boost::lexical_cast<std::string>(pPkgUserOperationReq->userid());
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleAdminMsg_userOperation),SQL:" << sSQLDELETE;
		sql << sSQLDELETE;
		pPkgUserOperationReply->set_issuccess(true);
	}
	else
	{
		pPkgUserOperationReply->set_issuccess(false);
		pPkgUserOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
	}
	return true;
}

bool DBSessionAdmin::handleMessage_advQuery(boost::shared_ptr<admin::PkgAdvQueryReq> pPkgAdvQueryReq, boost::shared_ptr<admin::PkgAdvQueryReply> pPkgAdvQueryReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string curdatetime = UtilTime::getCurrentTime();
	std::string sSQLSelect;
	std::string sqlWhereCondition;
	std::string sqlWhereS, sqlTail;
	std::vector<std::string> vectConditions;

	
	if (pPkgAdvQueryReq->queryflag() & admin::PkgAdvQueryReq::QT_ADVID)
	{
		sqlWhereS = "id = " + boost::lexical_cast<std::string>(pPkgAdvQueryReq->advid());
		vectConditions.push_back(sqlWhereS);
	}

	if (pPkgAdvQueryReq->queryflag() & admin::PkgAdvQueryReq::QT_KEYNAME)
	{
		std::string advtxtname_encoded;
		encodeString_Hex(pPkgAdvQueryReq->keyname(), advtxtname_encoded);
		sqlWhereS = "(name LIKE '%" + advtxtname_encoded + "%' OR advtxt LIKE '%" + advtxtname_encoded + "%')";
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
	sSQLSelect = "SELECT * FROM adv" + sqlWhereCondition;
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleMessage_advQuery),SQL:" << sSQLSelect;

	admin::PkgAdvInfo aif;
	soci::statement st = (sql.prepare << sSQLSelect, soci::into(aif));
	st.execute();
	while (st.fetch())
	{
		admin::PkgAdvInfo * p = pPkgAdvQueryReply->add_advinfolist();
		*p = aif;
	}
	pPkgAdvQueryReply->set_issuccess(true);

	return true;
}

bool DBSessionAdmin::handleMessage_advOperation(boost::shared_ptr<admin::PkgAdvOperationReq> pPkgAdvOperationReq, boost::shared_ptr<admin::PkgAdvOperationReply> pPkgAdvOperationReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	soci::session sql(*dbUtil_.getConnection());
	if (pPkgAdvOperationReq->advoperation() == admin::PkgAdvOperationReq::TO_INSERT_INFO)
	{
		std::string curdatetime = UtilTime::getCurrentTime();
		const admin::PkgAdvInfo&advInfo = pPkgAdvOperationReq->advinfo();
		std::stringstream ss;
		std::string advtxtname_encoded;
		std::string advdescname_encoded;
		encodeString_Hex(advInfo.advtxt(), advtxtname_encoded);
		encodeString_Hex(advInfo.advdesc(), advdescname_encoded);
		ss << "INSERT INTO adv(name,picpath,advtxt,advdesc,createtime,expiretime) VALUES('"
			<< advInfo.name() << "','" << advInfo.picpath() << "','" << advtxtname_encoded << "','" << advdescname_encoded
			<< "','" << curdatetime << "','" << advInfo.expiretime() << "'" << ")";
		std::string sSQLInsert = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleMessage_advOperation),SQL:" << sSQLInsert;
		sql << sSQLInsert;
		//取得刚刚插入的id，可能有简单办法，需找时间研究
		int advid = 0;
		std::string sSQLSelect = "SELECT id FROM adv WHERE createtime = '" + curdatetime + "' ORDER BY id DESC LIMIT 1";
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleMessage_advOperation),SQL:" << sSQLSelect;
		soci::indicator ind(soci::indicator::i_null);
		sql << sSQLSelect, into(advid, ind);
		if (ind == soci::indicator::i_ok)
		{
			pPkgAdvOperationReply->set_advid(advid);
			pPkgAdvOperationReply->set_issuccess(true);
		}
		else
		{
			pPkgAdvOperationReply->set_issuccess(false);
			pPkgAdvOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.inserterror"));
		}
	}
	else if (pPkgAdvOperationReq->advoperation() == admin::PkgAdvOperationReq::TO_UPDATE_INFO)
	{
		const admin::PkgAdvInfo&advInfo = pPkgAdvOperationReq->advinfo();
		std::stringstream ss;
		std::string sSQLUpdate;
		soci::session sql(*dbUtil_.getConnection());
		std::string curdatetime = UtilTime::getCurrentTime();
		std::string advtxtname_encoded;
		std::string advdescname_encoded;
		encodeString_Hex(advInfo.advtxt(), advtxtname_encoded);
		encodeString_Hex(advInfo.advdesc(), advdescname_encoded);

		ss << "UPDATE adv SET name = '" << advInfo.name() << "',picpath = '" << advInfo.picpath() << "',advtxt = '"
			<< advtxtname_encoded << "',advdesc = '" << advdescname_encoded << "',expiretime = '" << advInfo.expiretime() << "' WHERE id = "
			<< advInfo.id();
		sSQLUpdate = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleMessage_advOperation),SQL:" << sSQLUpdate;
		sql << sSQLUpdate;
		pPkgAdvOperationReply->set_issuccess(true);
	}
	else if (pPkgAdvOperationReq->advoperation() == admin::PkgAdvOperationReq::TO_DELETE_INFO)
	{
		std::string sSQLDELETE;
		sSQLDELETE = "DELETE FROM adv WHERE id = " + boost::lexical_cast<std::string>(pPkgAdvOperationReq->advid());
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleMessage_advOperation),SQL:" << sSQLDELETE;
		sql << sSQLDELETE;
		pPkgAdvOperationReply->set_issuccess(true);
	}
	else
	{
		pPkgAdvOperationReply->set_issuccess(false);
		pPkgAdvOperationReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.wrongparam"));
	}
	return true;
}


bool DBSessionAdmin::handleThirdMsg_setSysInfo(boost::shared_ptr<admin::PkgSysInfoSetReq> pPkgSysInfoSetReq, boost::shared_ptr<admin::PkgSysInfoSetReply> pPkgSysInfoReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{

	soci::session sql(*dbUtil_.getConnection());

	std::string sSQLDELETE;
	sSQLDELETE = "DELETE FROM sysinfo WHERE type ='" + pPkgSysInfoSetReq->keystring() + "'";
	sql << sSQLDELETE;
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_setSysInfo),SQL:" << sSQLDELETE;

	std::stringstream ss;
	ss << "INSERT INTO sysinfo(type,msginfo) VALUES('"
		<< pPkgSysInfoSetReq->keystring() << "','" << pPkgSysInfoSetReq->valuestring() + "')";
	std::string sSQLInsert = ss.str();
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_setSysInfo),SQL:" << sSQLInsert;
	sql << sSQLInsert;

	pPkgSysInfoReply->set_issuccess(true);

	return true;
}

bool DBSessionAdmin::handleThirdMsg_getSysInfo(boost::shared_ptr<admin::PkgSysInfoGetReq> pPkgSysInfoGetReq, boost::shared_ptr<admin::PkgSysInfoGetReply> pPkgSysInfoGetReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	soci::session sql(*dbUtil_.getConnection());

	std::string sSQLSelect = "SELECT msginfo FROM sysinfo WHERE type = '" + pPkgSysInfoGetReq->keystring() + "'";
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_getSysInfo),SQL:" << sSQLSelect;

	std::string smsginfo;
	soci::indicator ind(soci::indicator::i_null);
	sql << sSQLSelect, into(smsginfo, ind);
	if (ind == soci::indicator::i_ok)
	{
		pPkgSysInfoGetReply->set_issuccess(true);
		pPkgSysInfoGetReply->set_valuestring(smsginfo);
	}
	else
	{
		pPkgSysInfoGetReply->set_issuccess(false);
		pPkgSysInfoGetReply->set_err(UtilLanguage::get_mutable_instance().getLanguageString(pPkgSessionUser->applanguage(), "root.datamodule.dberror.nodata"));
	}

	return true;
}
