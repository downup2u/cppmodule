#ifndef _MYSQLDBUTIL_INCLUDE
#define _MYSQLDBUTIL_INCLUDE
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>

#include "soci.h"

class mysqlDBUtil
{
public:
	mysqlDBUtil();
	~mysqlDBUtil();
public:
	bool initDB(size_t nPoolSize, const std::string&connectstring, std::string&err);
	bool exitDB();
public:
	boost::shared_ptr<soci::connection_pool> getConnection();
private:
	boost::shared_ptr<soci::connection_pool> pConnectionPool_;
};

#endif