#include "mysqldbutil.h"
#include "soci-mysql.h"

soci::backend_factory const &backEnd = *soci::factory_mysql();

mysqlDBUtil::mysqlDBUtil()
{

}

mysqlDBUtil::~mysqlDBUtil()
{

}

bool mysqlDBUtil::initDB(size_t nPoolSize, const std::string&connectstring, std::string&err)
{
	pConnectionPool_ = boost::make_shared<soci::connection_pool>(nPoolSize);
	for (std::size_t i = 0; i < nPoolSize; ++i)
	{
		soci::session & sql = pConnectionPool_->at(i);
		sql.open(backEnd, connectstring);
	}
	return true;
}
bool mysqlDBUtil::exitDB()
{
	pConnectionPool_.reset();
	return true;
}

boost::shared_ptr<soci::connection_pool> mysqlDBUtil::getConnection()
{
	return pConnectionPool_;
}
