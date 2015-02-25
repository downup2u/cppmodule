#ifndef _EncryptWrap_Include
#define _EncryptWrap_Include
#include <string>
#include <boost/serialization/singleton.hpp> 

class EncryptWrap :public boost::serialization::singleton<EncryptWrap>
{
public:
	EncryptWrap();
	~EncryptWrap();
public:
	std::string getEncryptStringHex(const std::string& src, const std::string& key);
	std::string getDecryptStringHex(const std::string& src, const std::string& key);
	std::string getMD5(const std::string&s);

	std::string encryptString(const std::string&txt, const std::string&password);
	std::string decryptString(const std::string&txt, const std::string&password);

	std::string generateRandomSn();
	std::string hashedSn(const std::string&sSn);
};
#endif