#include "EncryptWrap.h"
#include "mylib/encrypt/md5.h"
#include "mylib/encrypt/aes.h"
#include <boost/algorithm/hex.hpp>
#include <glog.h>
#include <boost/random.hpp>
#include <boost/uuid/uuid.hpp>   
#include <boost/uuid/uuid_generators.hpp>   
#include <boost/uuid/uuid_io.hpp> 

EncryptWrap::EncryptWrap()
{

}

EncryptWrap::~EncryptWrap()
{

}


std::string EncryptWrap::getEncryptStringHex(const std::string& src, const std::string& key)
{
	std::string sRetHex;
	std::string sRawData = boost::algorithm::unhex(src);
	std::string sRawKey = boost::algorithm::unhex(key);
	size_t sizeRawData = sRawData.size();
	sRawData.resize(sizeRawData + (16 - sizeRawData % 16));
	for (size_t n = 0; n<(16 - sizeRawData % 16); n++)
	{
		sRawData[sizeRawData + n] = (16 - sizeRawData % 16);
	}
	uint8_t cSrc[16] = { 0 };
	uint8_t cDst[16] = { 0 };
	int nEncryptedLen = 16;
	for (size_t i = 0; i < sRawData.size(); i += 16)
	{
		nEncryptedLen = 16;
		memset(cSrc, 0, sizeof(cSrc));
		memcpy(cSrc, sRawData.data() + i, nEncryptedLen);
		ecb_aes_encrypt(cSrc, 16, cDst, (const uint8_t*)sRawKey.data());
		std::string sHexData = boost::algorithm::hex(std::string((char*)cDst, 16));
		sRetHex += sHexData;
	}
	return sRetHex;
}

std::string EncryptWrap::getDecryptStringHex(const std::string& src, const std::string& key)
{
	std::string sRetHex;
	std::string sRawData = boost::algorithm::unhex(src);
	std::string sRawKey = boost::algorithm::unhex(key);
	std::string sRawRetData;
	uint8_t cSrc[16] = { 0 };
	uint8_t cDst[16] = { 0 };
	int nEncryptedLen = 16;
	for (size_t i = 0; i < sRawData.size(); i += 16)
	{
		nEncryptedLen = (i + 16) < sRawData.size() ? 16 : sRawData.size() - i;
		memset(cSrc, 0, sizeof(cSrc));
		memcpy(cSrc, sRawData.data() + i, nEncryptedLen);
		ecb_aes_decrypt(cSrc, 16, cDst, (const uint8_t*)sRawKey.data());
		sRawRetData += std::string((char*)cDst, 16);
	}
	size_t n = sRawRetData.size() - 1;
	for (; n >= sRawRetData.size() - 16; n--)
	{
		if (1 <= sRawRetData[n] && 16 >= sRawRetData[n])
		{
			sRawRetData[n] = 0;
		}
		else
		{
			break;
		}
	}
	sRawRetData.resize(n+1);
	sRetHex = boost::algorithm::hex(sRawRetData);
	return sRetHex;	
}



std::string EncryptWrap::getMD5(const std::string&s)
{
	MD5 md5(s.data(), s.length());
	return md5.toString();
}

std::string EncryptWrap::encryptString(const std::string&txt, const std::string&password)
{
	std::string sHexTxt = boost::algorithm::hex(txt);
	std::string sMD5Key = getMD5(password);
	std::string sHex = getEncryptStringHex(sHexTxt, sMD5Key);
	return sHex;
}

std::string EncryptWrap::decryptString(const std::string&txt, const std::string&password)
{
	std::string sMD5Key = getMD5(password);
	std::string sHex = getDecryptStringHex(txt, sMD5Key);
	sHex = boost::algorithm::unhex(sHex);
	return sHex;
}

std::string EncryptWrap::generateRandomSn()
{
	boost::uuids::random_generator rgen;
	boost::uuids::uuid ranUUID = rgen();
	std::stringstream ss;
	ss << ranUUID;
	return ss.str();
}

std::string EncryptWrap::hashedSn(const std::string&sSn)
{
	return getMD5(sSn + "\niteasysoft");
}