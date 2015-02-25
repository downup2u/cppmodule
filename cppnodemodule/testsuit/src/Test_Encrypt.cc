#include <gtest/gtest.h>   
#include <string>
#include <boost/algorithm/hex.hpp>

#include "../../src/mylib/util/utilstring.h"
#include "../../src/EncryptWrap.h"

#include <encryptstring.h>

class Test_Encrypt: public testing::Test
{
public:
	virtual void SetUp()
	{
	}
	virtual void TearDown()
	{
	}
protected:
};

TEST_F(Test_Encrypt, test_encryptstring)
{
	{
		std::string sOut;
		std::string encrypt = "168082814909ED86DBEAB589E3E71A1E";
		bool bSuc = decodeString(encrypt, sOut);
	}

	{
		std::string sTxt = "123456";
		std::string sOut, sOutDec;
		bool bSuc = false;

		bSuc = encodeString(sTxt, sOut);
		EXPECT_EQ(bSuc, true);
		bSuc = decodeString(sOut, sOutDec);
		EXPECT_EQ(bSuc, true);
		EXPECT_EQ(sOutDec, sTxt);

		sTxt.clear();
		sTxt = "我是北京天安门";
		bSuc = encodeString(sTxt, sOut);
		EXPECT_EQ(bSuc, true);
		bSuc = decodeString(sOut, sOutDec);
		EXPECT_EQ(bSuc, true);
		EXPECT_EQ(sOutDec, sTxt);

		sTxt.clear();
		sTxt = "我是北京天安门";
		sTxt.push_back('\0');
		sTxt += "AAAA我是北京天安门BB";
		sTxt.push_back('\0');
		sTxt += "CC";
		bSuc = encodeString(sTxt, sOut);
		EXPECT_EQ(bSuc, true);
		bSuc = decodeString(sOut, sOutDec);
		EXPECT_EQ(bSuc, true);
		EXPECT_EQ(sOutDec, sTxt);

	}
}
TEST_F(Test_Encrypt, EnDecryptString)
{

	{
		std::string sTxt = "0123456789012345";
		std::string sOut;
		std::string sMd5 = EncryptWrap::get_mutable_instance().getMD5("wxq");
		std::string sHex = boost::algorithm::hex(sTxt);
		std::string sEncrypted = EncryptWrap::get_mutable_instance().getEncryptStringHex(sHex, sMd5);
		std::string sDecrypted = EncryptWrap::get_mutable_instance().getDecryptStringHex(sEncrypted, sMd5);
		std::string sRawPassword = boost::algorithm::unhex(sDecrypted);
	}

	{
		std::string sTxt = "0123456789012345";
		std::string sPassword = "wxq";
		std::string sHexData = EncryptWrap::get_mutable_instance().encryptString(sTxt, sPassword);
		std::string sDeData = EncryptWrap::get_mutable_instance().decryptString(sHexData,sPassword);
		EXPECT_EQ(sTxt, sDeData);
		
		std::string sHdd = "aaaaaaabbbbbbbcccccccc";
		sDeData = EncryptWrap::get_mutable_instance().decryptString(sHdd, sPassword);

	}

}

TEST_F(Test_Encrypt, InitDBPassword)
{
	{
		std::string sPassword = "123456";
		std::string sKey = EncryptWrap::get_mutable_instance().generateRandomSn();
		std::string sKeyMd5 = EncryptWrap::get_mutable_instance().getMD5(sKey);
		std::string sEncryptPassword = EncryptWrap::get_mutable_instance().encryptString(sPassword, sKeyMd5);

		std::string sDecryptPassword = EncryptWrap::get_mutable_instance().decryptString(sEncryptPassword, sKeyMd5);
	}

}
