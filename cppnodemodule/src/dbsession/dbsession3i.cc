#include "dbsession3i.h"
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
#include <encryptstring.h>
//====================================
DBSession3I::DBSession3I()
{

}

DBSession3I::~DBSession3I()
{
}


bool DBSession3I::initSession(size_t nPoolSize, const std::string& connectstring)
{
	std::string err;
	dbUtil_.initDB(nPoolSize, connectstring, err);
	return true;
}

bool DBSession3I::exitSession(std::string&err)
{
	dbUtil_.exitDB();
	return true;
}



//==================================================
bool DBSession3I::handleThirdMsg_getGetPasswordTable(boost::shared_ptr<thirdinterface::PkgGetPasswordTableReq> pPkgGetPasswordTableReq, boost::shared_ptr<thirdinterface::PkgGetPasswordTableReply> pPkgGetPasswordTableReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sqlWhereCondition;
	std::string sqlWhereS;
	std::vector<std::string> vectConditions;

	if (pPkgGetPasswordTableReq->has_createtime())
	{
		if (pPkgGetPasswordTableReq->getcondition() == thirdinterface::PkgGetPasswordTableReq::EGC_TIMELESS)
		{
			sqlWhereS = "time <= '" + pPkgGetPasswordTableReq->createtime() + "'";
			vectConditions.push_back(sqlWhereS);
		}
		else if (pPkgGetPasswordTableReq->getcondition() == thirdinterface::PkgGetPasswordTableReq::EGC_TIMEGREATER)
		{
			sqlWhereS = "time >= '" + pPkgGetPasswordTableReq->createtime() + "'";
			vectConditions.push_back(sqlWhereS);
		}
	}
	if (pPkgGetPasswordTableReq->has_startid())
	{
		sqlWhereS = "id > " + boost::lexical_cast<std::string>(pPkgGetPasswordTableReq->startid());
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgGetPasswordTableReq->has_gettype())
	{
		if (pPkgGetPasswordTableReq->gettype() != thirdinterface::PkgGetPasswordTableReq::GT_ALL)
		{
			int nType = pPkgGetPasswordTableReq->gettype();
			sqlWhereS = "type = " + boost::lexical_cast<std::string>(nType);
		}
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgGetPasswordTableReq->has_status())
	{
		int nStatus = pPkgGetPasswordTableReq->status();
		sqlWhereS = "status = " + boost::lexical_cast<std::string>(nStatus);
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

	soci::session sql(*dbUtil_.getConnection());
	std::string sSQLSelect;

	thirdinterface::PkgGetPasswordTable gpt;
	sSQLSelect = "SELECT * FROM getpassword" + sqlWhereCondition;
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_getGetPasswordTable),SQL:" << sSQLSelect;
	soci::statement st = (sql.prepare << sSQLSelect, soci::into(gpt));
	st.execute();
	while (st.fetch())
	{
		thirdinterface::PkgGetPasswordTable * p = pPkgGetPasswordTableReply->add_infolist();
		*p = gpt;
	}
	pPkgGetPasswordTableReply->set_issuccess(true);
	
	return true;
}

bool DBSession3I::handleThirdMsg_setGetPasswordTableStatus(boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReq> pPkgSetGetPasswordTableStatusReq, boost::shared_ptr<thirdinterface::PkgSetGetPasswordTableStatusReply> pPkgSetGetPasswordTableStatusReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	if (pPkgSetGetPasswordTableStatusReq->statuslist_size() == 0)
	{
		pPkgSetGetPasswordTableStatusReply->set_issuccess(false);
		pPkgSetGetPasswordTableStatusReply->set_err("no data to UPDATE");
		return false;
	}
	soci::session sql(*dbUtil_.getConnection());
	for (int i = 0; i < pPkgSetGetPasswordTableStatusReq->statuslist_size(); i++)
	{
		const thirdinterface::PkgGetPasswordStatus &Status = pPkgSetGetPasswordTableStatusReq->statuslist().Get(i);
		std::stringstream ss;
		std::string sSQLUpdate;
		ss << "UPDATE getpassword SET status = " << boost::lexical_cast<std::string>(Status.status()) << " WHERE id = " << boost::lexical_cast<std::string>(Status.id());
		sSQLUpdate = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_setGetPasswordTableStatus),SQL:" << sSQLUpdate;
		sql << sSQLUpdate;
	}
	pPkgSetGetPasswordTableStatusReply->set_issuccess(true);
	return true;
}

bool DBSession3I::handleThirdMsg_getSysInfoList(boost::shared_ptr<thirdinterface::PkgSysInfoListGetReq> pPkgMsgInfoReq, boost::shared_ptr<thirdinterface::PkgSysInfoListGetReply> pPkgMsgInfoReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{

	std::string sqlWhereCondition;
	std::string sqlWhereS;

	std::vector<std::string> vectConditions;
	for (int i = 0; i < pPkgMsgInfoReq->keystring_size(); i++)
	{
		const std::string&sKey = pPkgMsgInfoReq->keystring().Get(i);
		sqlWhereS = "type = '" + sKey + "'";
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
			sqlWhereCondition += " OR ";
		}

		sqlWhereCondition += sqlWhere;
	}

	soci::session sql(*dbUtil_.getConnection());

	std::string sSQLSelect = "SELECT type,msginfo FROM sysinfo" + sqlWhereCondition;
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_getSysInfoList),SQL:" << sSQLSelect;
	typedef boost::tuple<std::string, std::string> BTUPLE;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		std::string type = dbvalues.get<0>();
		std::string keyvalue = dbvalues.get<1>();
		thirdinterface::PkgSysInfoValue* pPkgSysInfoValue = pPkgMsgInfoReply->add_valuemaps();
		pPkgSysInfoValue->set_keystring(type);
		pPkgSysInfoValue->set_valuestring(keyvalue);
		it++;
	}
	pPkgMsgInfoReply->set_issuccess(true);	
	return true;
}

bool DBSession3I::handleThirdMsg_getRegisterInvitionReq(boost::shared_ptr<thirdinterface::PkgRegisterInvitionReq> pPkgRegisterInvitionReq, boost::shared_ptr<thirdinterface::PkgRegisterInvitionReply> pPkgRegisterInvitionReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	std::string sqlWhereCondition;
	std::string sqlWhereS;
	std::vector<std::string> vectConditions;

	if (pPkgRegisterInvitionReq->has_createtime())
	{
		if (pPkgRegisterInvitionReq->getcondition() == thirdinterface::PkgRegisterInvitionReq::EGC_TIMELESS)
		{
			sqlWhereS = "invitiontime <= '" + pPkgRegisterInvitionReq->createtime() + "'";
			vectConditions.push_back(sqlWhereS);
		}
		else if (pPkgRegisterInvitionReq->getcondition() == thirdinterface::PkgRegisterInvitionReq::EGC_TIMEGREATER)
		{
			sqlWhereS = "invitiontime >= '" + pPkgRegisterInvitionReq->createtime() + "'";
			vectConditions.push_back(sqlWhereS);
		}
	}
	if (pPkgRegisterInvitionReq->has_startid())
	{
		sqlWhereS = "id > " + boost::lexical_cast<std::string>(pPkgRegisterInvitionReq->startid());
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgRegisterInvitionReq->has_gettype())
	{
		if (pPkgRegisterInvitionReq->gettype() != thirdinterface::PkgRegisterInvitionReq::GT_ALL)
		{
			int nType = pPkgRegisterInvitionReq->gettype();
			sqlWhereS = "type = " + boost::lexical_cast<std::string>(nType);
		}
		vectConditions.push_back(sqlWhereS);
	}
	if (pPkgRegisterInvitionReq->has_status())
	{
		int nStatus = pPkgRegisterInvitionReq->status();
		sqlWhereS = "status = " + boost::lexical_cast<std::string>(nStatus);
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

	soci::session sql(*dbUtil_.getConnection());
	std::string sSQLSelect;

	thirdinterface::PkgRegisterInvition gpt;
	sSQLSelect = "SELECT * FROM registerinvition" + sqlWhereCondition;
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_getRegisterInvitionReq),SQL:" << sSQLSelect;
	soci::statement st = (sql.prepare << sSQLSelect, soci::into(gpt));
	st.execute();
	while (st.fetch())
	{
		thirdinterface::PkgRegisterInvition * p = pPkgRegisterInvitionReply->add_infolist();
		std::string companyname = getCompanyName(gpt.companyid(),sql);
		std::string senderrealname = getUserName(gpt.userid(),sql);
		gpt.set_companyname(companyname);
		gpt.set_sendrealname(senderrealname);
		*p = gpt;
	}
	pPkgRegisterInvitionReply->set_issuccess(true);

	return true;
}

bool DBSession3I::handleThirdMsg_setRegisterInvitionStatus(boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReq> pPkgSetRegisterInvitionStatusReq, boost::shared_ptr<thirdinterface::PkgSetRegisterInvitionStatusReply> pPkgSetRegisterInvitionStatusReply, boost::shared_ptr<session::PkgSessionUser> pPkgSessionUser)
{
	if (pPkgSetRegisterInvitionStatusReq->statuslist_size() == 0)
	{
		pPkgSetRegisterInvitionStatusReply->set_issuccess(false);
		pPkgSetRegisterInvitionStatusReply->set_err("no data to UPDATE");
		return false;
	}
	soci::session sql(*dbUtil_.getConnection());
	for (int i = 0; i < pPkgSetRegisterInvitionStatusReq->statuslist_size(); i++)
	{
		const thirdinterface::PkgRegisterInvitionStatus &Status = pPkgSetRegisterInvitionStatusReq->statuslist().Get(i);
		std::stringstream ss;
		std::string sSQLUpdate;
		ss << "UPDATE registerinvition SET status = " << boost::lexical_cast<std::string>(Status.status()) << " WHERE id = " << boost::lexical_cast<std::string>(Status.id());
		sSQLUpdate = ss.str();
		BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(handleThirdMsg_setRegisterInvitionStatus),SQL:" << sSQLUpdate;
		sql << sSQLUpdate;
	}
	pPkgSetRegisterInvitionStatusReply->set_issuccess(true);
	return true;
}

std::string DBSession3I::getUserName(int userid, soci::session& sql){
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string> BTUPLE;
	sSQLSelect = "SELECT id,realname FROM user WHERE id = " + boost::lexical_cast<std::string>(userid);
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getUserName),SQL:" << sSQLSelect;
	std::string realname;
	std::string user_realname_decoded;

	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	if (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		realname = dbvalues.get<1>();
		decodeString(realname, user_realname_decoded);
		it++;
	}
	return user_realname_decoded;
}

std::string DBSession3I::getCompanyName(int companyid, soci::session& sql){
	std::string companyname;
	std::string company_name_decoded;
	std::string sSQLSelect = "SELECT name,createtime FROM company WHERE id = " + boost::lexical_cast<std::string>(companyid);
	std::string companycreatetime;

	typedef boost::tuple<std::string, tm> BTUPLE;
	BOOST_LOG_SEV(gLog_, boost::log::trivial::debug) << "(getCompanyInfo),SQL:" << sSQLSelect;


	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	if (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		companyname = dbvalues.get<0>();
		decodeString(companyname, company_name_decoded);
		tm tmcompanycreatetime = dbvalues.get<1>();
		std::string companycreatetime = UtilTime::convertToTimeString(tmcompanycreatetime);
	}
	return company_name_decoded;
}