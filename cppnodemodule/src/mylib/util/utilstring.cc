#include "utilstring.h"
#include <boost/bind.hpp>
#include <iostream> 
#include <streambuf>
#include <fstream>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/locale.hpp>



namespace UtilString
{
	//UTF-8 -> ASCII(本地编码，例:GB2312,可供打印或写入到文件
	std::locale get_chinese_locale()
	{
		// 系统默认的locale不一定是中文的
		std::locale chslocale;
		try
		{
			chslocale = std::locale("chs");
		}
		catch (std::exception &e)
		{
			chslocale = std::locale("");
		}
		return chslocale;
	}
//	std::locale oldLocale = std::locale::global(get_chinese_locale());

	std::string getCurrentEncoding(const std::locale& locale)
	{				
		return "GBK";
	}

	std::string convertUTF8ToLocalString(const std::string &sUTF8)
	{//X
		std::wstring ws = convertUTF8ToWstring(sUTF8);		
		return convertWstringToLocalString(ws);
	}

	//UNICODE -> ASCII(本地编码，例:GB2312,可供打印或写入到文件
	std::string convertWstringToLocalString(const std::wstring&ws)
	{//Y
		std::string sLocalString;
		const std::locale locale = get_chinese_locale();//系统字符集
		typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
		const converter_type& converter = std::use_facet<converter_type>(locale);
		size_t nBufLen = ws.length() * converter.max_length();
		char*pBuf = new char[nBufLen];
		boost::shared_ptr<char> pT(pBuf);
		std::mbstate_t state;
		const wchar_t* from_next;
		char* to_next;
		try
		{
			const converter_type::result result = converter.out(state, ws.data(), ws.data() + ws.length(), from_next, pBuf, pBuf + nBufLen, to_next);
			if (result == converter_type::ok)
			{
				sLocalString.assign(pBuf, to_next);
			}
		}
		catch (...)
		{
			sLocalString = "Convert Data Error!";
		}
		return sLocalString;
	}
	//https://github.com/yyzybb/Bex/blob/f356ef83620a7ec8152a557773f90784fd47e373/Bex/src/Bex/locale/charset_cvt.h
	//Local -> UNICODE
	std::wstring convertLocalStringToWString(const std::string &localString)
	{//Y
		std::wstring ws;
		const std::locale locale = get_chinese_locale();//系统字符集	
		try{
			ws = boost::locale::conv::to_utf<wchar_t>(localString, getCurrentEncoding(locale));
		}
		catch (...)
		{
			ws = L"Convert Data Error!";
		}
		return ws;
	}

	//UTF-8 -> UNICODE(UTF-8转成UNICODE)
	std::wstring convertUTF8ToWstring(const std::string &sUTF8)
	{//Y
		std::wstring ws;
		try
		{
			ws = boost::locale::conv::utf_to_utf<wchar_t>(sUTF8);
		}
		catch (...)
		{
			ws = L"Convert Data Error!";
		}
		return ws;
	}


	//UNICODE -> UTF-8(UNICODE转成UTF-8)
	std::string convertWstringToUTF8(const std::wstring&ucs4_data)
	{//Y
		std::string sUtf8;
		try
		{
			sUtf8 = boost::locale::conv::utf_to_utf<char>(ucs4_data);
		}
		catch (...)
		{
			sUtf8 = "Convert Data Error!";
		}
		return sUtf8;
	}

	//Local -> UTF-8
	std::string convertLocalStringToUTF8(const std::string&localString)
	{//Y	
		std::string sUtf8;
		const std::locale locale = get_chinese_locale();//系统字符集	
		try{//throws std::bad_cast if the loc does not have info facet installed
			sUtf8 = boost::locale::conv::to_utf<char>(localString, getCurrentEncoding(locale));
		}
		catch (...){
			sUtf8 = "Convert Data Error!";
		}
		return sUtf8;
	}
};

