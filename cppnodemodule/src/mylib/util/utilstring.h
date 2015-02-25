#ifndef _UTIL_STRING
#define _UTIL_STRING
#include <string>

namespace UtilString
{
	//UTF-8 -> ASCII(本地编码，例:GB2312,可供打印或写入到文件
	std::string convertUTF8ToLocalString(const std::string &sUTF8);
	//UNICODE -> ASCII(本地编码，例:GB2312,可供打印或写入到文件
	std::string convertWstringToLocalString(const std::wstring&ws);

	//Local -> UNICODE
	std::wstring convertLocalStringToWString(const std::string &localString);
	//UTF-8 -> UNICODE(UTF-8转成UNICODE)
	std::wstring convertUTF8ToWstring(const std::string &sUTF8);

	//UNICODE -> UTF-8（插入到数据库，传输到服务端)
	std::string convertWstringToUTF8(const std::wstring&ws);
	//Local -> UTF-8
	std::string convertLocalStringToUTF8(const std::string&localString);
}


#endif