// \see http://soci.sourceforge.net/doc/index.html
// STL
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <exception>
// SOCI
#include <soci.h>
#include "soci-mysql.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>


soci::backend_factory const &backEnd = *soci::factory_mysql();

// ////////////////////// M A I N /////////////////////////
int main() {

  try {

    std::string name;
    // int count = 0;
	std::string connectstring = "db=task host=192.168.1.199 user=root password=itadmin charset=utf8";
	boost::shared_ptr<soci::connection_pool> pConnectionPool_ = boost::make_shared<soci::connection_pool>(4);
	{
		soci::session & sql = pConnectionPool_->at(0);
		sql.open(backEnd, connectstring);

	}
	soci::session sql(*pConnectionPool_);
    sql << "select name from task where id = 355", soci::into(name);

    std::cout << "task id 355 name:" << name << std::endl;

  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
