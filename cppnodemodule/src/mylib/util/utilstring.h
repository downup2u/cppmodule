#ifndef _UTIL_STRING
#define _UTIL_STRING
#include <string>

namespace UtilString
{
	//UTF-8 -> ASCII(���ر��룬��:GB2312,�ɹ���ӡ��д�뵽�ļ�
	std::string convertUTF8ToLocalString(const std::string &sUTF8);
	//UNICODE -> ASCII(���ر��룬��:GB2312,�ɹ���ӡ��д�뵽�ļ�
	std::string convertWstringToLocalString(const std::wstring&ws);

	//Local -> UNICODE
	std::wstring convertLocalStringToWString(const std::string &localString);
	//UTF-8 -> UNICODE(UTF-8ת��UNICODE)
	std::wstring convertUTF8ToWstring(const std::string &sUTF8);

	//UNICODE -> UTF-8�����뵽���ݿ⣬���䵽�����)
	std::string convertWstringToUTF8(const std::wstring&ws);
	//Local -> UTF-8
	std::string convertLocalStringToUTF8(const std::string&localString);
}


#endif