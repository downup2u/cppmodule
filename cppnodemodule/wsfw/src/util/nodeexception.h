#ifndef _nodeexception_include_h
#define _nodeexception_include_h
#include <boost/exception/all.hpp>
#include <string>

class nodeexception : virtual public std::exception, virtual public boost::exception
{

};

typedef boost::error_info<struct tag_err_no, int> err_no;
typedef boost::error_info<struct tag_errtype_str, std::string> errtype_str;
typedef boost::error_info<struct tag_err_str, std::string> err_str;

const int ERROR_DB = 100;
const int ERROR_SESSION = 101;
const int ERROR_MSG = 102;
const int ERROR_LOGIC = 103;


#endif

