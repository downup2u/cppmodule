#include <boost/algorithm/hex.hpp>
#include <boost/uuid/uuid.hpp>  
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <json2pb.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <fstream> 
#include <iostream>

#include "../../src/mylib/util/utilstring.h"
#include "../../src/mylib/util/codec.h"
#include "../../src/mylib/util/utillanguage.h"
#include "../../src/gcomm/comm.pb.h"
#include "../../src/DataWrapComm.h"

int main(int argc, char** argv) 
{
	std::string sXML = "./dbinfo.xml";
	bool bSuc = DataWrapComm::get_mutable_instance().initDB(sXML);

	return 0;
}
