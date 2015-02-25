#ifndef _ESCAPE_STRING_INCLUDE_H
#define _ESCAPE_STRING_INCLUDE_H

#include <string>

std::string escapeString(const std::string& s);
std::string unescapeString(const std::string& s);
std::string escapeSql(const std::string& source);

std::string encodeString(const std::string& source);
std::string decodeString(const std::string& source);
#endif // _ESCAPE_STRING_INCLUDE_H

