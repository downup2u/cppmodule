#include "upgradedb010.h"

//#include <boost/algorithm/hex.hpp>
//#include <boost/uuid/uuid.hpp>  
//#include <boost/uuid/uuid_io.hpp>
//#include <boost/uuid/uuid_generators.hpp>
//#include <json2pb.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <fstream> 
#include <iostream>
#include <soci.h>
#include "soci-mysql.h"
#include <boost-tuple.h>
//#include "../../src/mylib/util/utilstring.h"
//#include "../../src/mylib/util/codec.h"
//#include "../../src/mylib/util/utillanguage.h"
//#include "../../src/gcomm/comm.pb.h"
//#include "../../src/DataWrapComm.h"
//#include "../../src/nodeexception.h"

#include "encryptstring.h"

void startupgrade_011(soci::session& sql);
void startupgrade(soci::session& sql);
void altertables(soci::session& sql);
void upgradetable_adminuser(soci::session& sql);
void upgradetable_user(soci::session& sql);
void upgradetable_getpassword(soci::session& sql);
void upgradetable_registerinvition(soci::session& sql);
void upgradetable_company(soci::session& sql);
void upgradetable_task(soci::session& sql);

soci::backend_factory const &backEnd = *soci::factory_mysql();

bool myEncodeString(const std::string&src, std::string&dst){

	bool isEncoded = true;

	for (int i = 0; i < src.size(); i++){
		if (src[i] >= '0' && src[i] <= '9')
			continue;
		if (src[i] >= 'a' && src[i] <= 'f')
			continue;
		if (src[i] >= 'A' && src[i] <= 'F')
			continue;
		isEncoded = false;
		break;
	}

	//if (isEncoded){
	//	std::cout << src << "seems already encoded,continue?y to encode,n to return" << std::endl;
	//	char c = getchar();
	//	if (c == 'y' || c == 'Y'){

	//	}
	//	else{
	//		//dst = src;
	//		//return true;
	//	}
	//}
	return encodeString(src, dst);
}

void altertables(soci::session& sql){
	sql << "ALTER TABLE adminuser MODIFY password varchar(100)";

	sql << "ALTER TABLE user MODIFY phonenumber varchar(100)";
	sql << "ALTER TABLE user MODIFY emailaddress varchar(100)";
	sql << "ALTER TABLE user MODIFY realname varchar(100)";
	sql << "ALTER TABLE user MODIFY password varchar(100)";

	sql << "ALTER TABLE getpassword MODIFY phonenumber varchar(100)";
	sql << "ALTER TABLE getpassword MODIFY emailaddress varchar(100)";

	sql << "ALTER TABLE registerinvition MODIFY realname varchar(100)";
	sql << "ALTER TABLE registerinvition MODIFY emailaddress varchar(100)";
	sql << "ALTER TABLE registerinvition MODIFY phonenumber varchar(100)";

	sql << "ALTER TABLE company MODIFY name varchar(500)";

	sql << "ALTER TABLE task MODIFY name varchar(500)";
	sql << "ALTER TABLE task MODIFY content varchar(2000)";
	sql << "ALTER TABLE task MODIFY remark varchar(1000)";

}

void upgradeto010()
{
	std::cout << "start upgrade to v0.1.0" << std::endl;

	try {

		std::string dbversion;
		// int count = 0;
		std::string connectstring = "db=task host=127.0.0.1 user=root password=123456 charset=utf8";
		boost::shared_ptr<soci::connection_pool> pConnectionPool_ = boost::make_shared<soci::connection_pool>(8);
		{
			soci::session & sql = pConnectionPool_->at(0);
			sql.open(backEnd, connectstring);

		}
		std::cout << "open db success!" << std::endl;

		soci::session sql(*pConnectionPool_);
		sql << "select msginfo from sysinfo where type = 'dbversion'", soci::into(dbversion);

		if (dbversion != "0.1.0"){
			if (dbversion < "0.1.0"){
				startupgrade(sql);
			}
			else{
				std::cout << "dbversion is " << dbversion << "greater than v0.1.0, no need to upgrade!" << std::endl;
			}
		}
		else{
			std::cout << "dbversion is 0.1.0,no need to upgrade!" << std::endl;
		}

	}
	catch (std::exception const& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
void upgradeto011()
{//DC6D90B64EFC0A9DC5388CC19F019ACE
	//std::string s = "89DBC35C81095D3E7EAF2D1259D396BE";
	//std::string sOut;
	//decodeString_Hex(s, sOut);

	std::cout << "start upgrade to v0.1.1" << std::endl;

	try {

		std::string dbversion;
		// int count = 0;
		std::string connectstring = "db=task host=192.168.2.199 user=root password=123456 charset=utf8";
		boost::shared_ptr<soci::connection_pool> pConnectionPool_ = boost::make_shared<soci::connection_pool>(8);
		{
			soci::session & sql = pConnectionPool_->at(0);
			sql.open(backEnd, connectstring);

		}
		std::cout << "open db success!" << std::endl;

		soci::session sql(*pConnectionPool_);
		sql << "select msginfo from sysinfo where type = 'dbversion'", soci::into(dbversion);

		if (dbversion != "0.1.1"){
			if (dbversion < "0.1.1"){
				startupgrade_011(sql);
			}
			else{
				std::cout << "dbversion is " << dbversion << "greater than v0.1.1, no need to upgrade!" << std::endl;
			}
		}
		else{
			std::cout << "dbversion is 0.1.1,no need to upgrade!" << std::endl;
		}

	}
	catch (std::exception const& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
void startupgrade(soci::session& sql){

	altertables(sql);

	std::string strSQL = "delete from sysinfo where type = 'dbversion'";
	sql << strSQL;

	strSQL = "Insert into sysinfo(type,msginfo) VALUES('dbversion','0.1.0')";
	sql << strSQL;

	std::cout << "start upgrade table adminuser..." << std::endl;
	upgradetable_adminuser(sql);

	std::cout << "start upgrade table user..." << std::endl;
	upgradetable_user(sql);

	std::cout << "start upgrade table getpassword..." << std::endl;
	upgradetable_getpassword(sql);

	std::cout << "start upgrade table registerinvition..." << std::endl;
	upgradetable_registerinvition(sql);

	std::cout << "start upgrade table company..." << std::endl;
	upgradetable_company(sql);

	std::cout << "start upgrade table task..." << std::endl;
	upgradetable_task(sql);
	
	std::cout << "start upgrade table finished!" << std::endl;

}

void startupgrade_011(soci::session& sql)
{
	std::string strSQL = "delete from sysinfo where type = 'dbversion'";
	sql << strSQL;

	strSQL = "Insert into sysinfo(type,msginfo) VALUES('dbversion','0.1.1')";
	sql << strSQL;

	strSQL = "update task set remark='' where remark is null";
	sql << strSQL;
	strSQL = "update task set remark='' where remark='10'";
	sql << strSQL;
	strSQL = "update task set content='' where content='10'";
	sql << strSQL;
	strSQL = "update user set emailaddress='' where emailaddress is null";
	sql << strSQL;
	strSQL = "update user set phonenumber='' where phonenumber is null";
	sql << strSQL;

	std::cout << "start upgrade table task..." << std::endl;
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string, std::string, std::string> BTUPLE;
	sSQLSelect = "SELECT id,name,content,remark FROM task";
	int userid;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	std::string name;
	std::string content;
	std::string remark;	
	
	while (it != rows.end())
	{

		std::string task_name_encoded;
		std::string task_content_encoded;
		std::string task_remark_encoded;
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		std::cout << "--------taskid:" << userid << std::endl;
		try{
			name = dbvalues.get<1>();
			content = dbvalues.get<2>();
			remark = dbvalues.get<3>();

			std::string task_name_decoded;
			std::string task_content_decoded;
			std::string task_remark_decoded;

			decodeString(name, task_name_decoded);
			decodeString(content, task_content_decoded);
			decodeString(remark, task_remark_decoded);

			encodeString_Hex(task_name_decoded, task_name_encoded);
			encodeString_Hex(task_content_decoded, task_content_encoded);
			encodeString_Hex(task_remark_decoded, task_remark_encoded);

			std::cout << "process data->name:[" << name << "]->(" << task_name_decoded <<  ")[" << task_name_encoded << "]" << std::endl;
			std::cout << "process data->content:[" << content << "]->(" << task_content_decoded << ")[" << task_content_encoded << "]" << std::endl;
			std::cout << "process data->name:[" << remark << "]->(" << task_remark_decoded << ")[" << task_remark_encoded << "]" << std::endl;
		}
		catch (...){
			std::cout << "get error" << std::endl;
		}



		std::string sSQLUpdate;
		sSQLUpdate = "UPDATE task SET name = '" + task_name_encoded + "',"
			+ "content = '" + task_content_encoded + "',"
			+ "remark = '" + task_remark_encoded + "'"
			+ " WHERE id = " + boost::lexical_cast<std::string>(userid);
		sql << sSQLUpdate;
		std::cout << sSQLUpdate << std::endl;

		it++;
	}

	strSQL = "update task set remark='' where remark='10'";
	sql << strSQL;
	strSQL = "update task set content='' where content='10'";
	sql << strSQL;

	std::cout << "start upgrade table finished!" << std::endl;
}
void upgradetable_adminuser(soci::session& sql)
{
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string> BTUPLE;
	sSQLSelect = "SELECT id,password FROM adminuser";
	std::string password;
	int userid;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		password = dbvalues.get<1>();
		std::string adminuser_password_encoded;
		bool bSuc = myEncodeString(password, adminuser_password_encoded);
		std::string sSQLUpdate;
		sSQLUpdate = "UPDATE adminuser SET password = '" + adminuser_password_encoded + "' WHERE id = " + boost::lexical_cast<std::string>(userid);
		sql << sSQLUpdate;
		std::cout << sSQLUpdate << std::endl;

		it++;
	}
}

void upgradetable_user(soci::session& sql)
{
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string,std::string,std::string,std::string> BTUPLE;
	sSQLSelect = "SELECT id,phonenumber,emailaddress,realname,password FROM user";
	std::string phonenumber;
	std::string emailaddress;
	std::string realname;
	std::string password;
	int userid;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		phonenumber = dbvalues.get<1>();
		emailaddress = dbvalues.get<2>();
		realname = dbvalues.get<3>();
		password = dbvalues.get<4>();

		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;
		std::string user_realname_encoded;
		std::string user_password_encoded;

		
		myEncodeString(phonenumber, user_phonenumber_encoded);
		myEncodeString(emailaddress, user_emailaddress_encoded);
		myEncodeString(realname, user_realname_encoded);
		myEncodeString(password, user_password_encoded);


		std::string sSQLUpdate;
		sSQLUpdate = "UPDATE user SET phonenumber = '" + user_phonenumber_encoded + "',"
			+ "emailaddress = '" + user_emailaddress_encoded + "',"
			+ "realname = '" + user_realname_encoded + "',"
			+ "password = '" + user_password_encoded + "'"
			+ " WHERE id = " + boost::lexical_cast<std::string>(userid);
		sql << sSQLUpdate;

		std::cout << sSQLUpdate << std::endl;
		it++;
	}
}

void upgradetable_getpassword(soci::session& sql)
{
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string, std::string> BTUPLE;
	sSQLSelect = "SELECT id,phonenumber,emailaddress FROM getpassword";
	std::string phonenumber;
	std::string emailaddress;
	int userid;

	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		phonenumber = dbvalues.get<1>();
		emailaddress = dbvalues.get<2>();

		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;

		myEncodeString(phonenumber, user_phonenumber_encoded);
		myEncodeString(emailaddress, user_emailaddress_encoded);

		std::string sSQLUpdate;
		sSQLUpdate = "UPDATE getpassword SET phonenumber = '" + user_phonenumber_encoded+ "',"
			+ "emailaddress = '" + user_emailaddress_encoded + "'"
			+ " WHERE id = " + boost::lexical_cast<std::string>(userid);
		sql << sSQLUpdate;

		std::cout << sSQLUpdate << std::endl;
		it++;
	}
}

void upgradetable_registerinvition(soci::session& sql)
{
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string, std::string, std::string> BTUPLE;
	sSQLSelect = "SELECT id,phonenumber,emailaddress,realname FROM registerinvition";
	std::string phonenumber;
	std::string emailaddress;
	std::string realname;
	int userid;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		phonenumber = dbvalues.get<1>();
		emailaddress = dbvalues.get<2>();
		realname = dbvalues.get<2>();

		std::string user_phonenumber_encoded;
		std::string user_emailaddress_encoded;
		std::string user_realname_encoded;

		myEncodeString(phonenumber, user_phonenumber_encoded);
		myEncodeString(emailaddress, user_emailaddress_encoded);
		myEncodeString(realname, user_realname_encoded);

		std::string sSQLUpdate;
		sSQLUpdate = "UPDATE registerinvition SET phonenumber = '" + user_phonenumber_encoded + "',"
			+ "emailaddress = '" + user_emailaddress_encoded + "',"
			+ "realname = '" + user_realname_encoded + "'"
			+ " WHERE id = " + boost::lexical_cast<std::string>(userid);
		sql << sSQLUpdate;

		std::cout << sSQLUpdate << std::endl;
		it++;
	}
}


void upgradetable_company(soci::session& sql)
{
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string> BTUPLE;
	sSQLSelect = "SELECT id,name FROM company";
	std::string name;
	int userid;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		name = dbvalues.get<1>();
		std::string adminuser_name_encoded;
		bool bSuc = myEncodeString(name, adminuser_name_encoded);
		std::string sSQLUpdate;
		sSQLUpdate = "UPDATE company SET name = '" + adminuser_name_encoded + "' WHERE id = " + boost::lexical_cast<std::string>(userid);
		sql << sSQLUpdate;
		std::cout << sSQLUpdate << std::endl;

		it++;
	}
}

void upgradetable_task(soci::session& sql)
{
	std::string sSQLSelect;
	typedef boost::tuple<int, std::string, std::string, std::string> BTUPLE;
	sSQLSelect = "SELECT id,name,content,remark FROM task";
	std::string name;
	std::string content;
	std::string remark;
	int userid;
	soci::rowset<BTUPLE> rows = sql.prepare << sSQLSelect;
	soci::rowset<BTUPLE>::const_iterator it = rows.begin();
	while (it != rows.end())
	{
		const BTUPLE& dbvalues = *it;
		userid = dbvalues.get<0>();
		name = dbvalues.get<1>();
		content = dbvalues.get<2>();
		remark = dbvalues.get<3>();

		std::string task_name_encoded;
		std::string task_content_encoded;
		std::string task_remark_encoded;

		myEncodeString(name, task_name_encoded);
		myEncodeString(content, task_content_encoded);
		myEncodeString(remark, task_remark_encoded);

		std::string sSQLUpdate;
		sSQLUpdate = "UPDATE task SET name = '" + task_name_encoded + "',"
			+ "content = '" + task_content_encoded + "',"
			+ "remark = '" + task_remark_encoded + "'"
			+ " WHERE id = " + boost::lexical_cast<std::string>(userid);
		sql << sSQLUpdate;
		std::cout << sSQLUpdate << std::endl;

		it++;
	}
}