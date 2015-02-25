#include <gtest/gtest.h>   
#include <string>
#include "../../src/mylib/util/utilstring.h"

class Test_Util: public testing::Test
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

TEST_F(Test_Util, UtilStringCodec)
{
	std::string sCorrectUtf8 = UtilString::convertWstringToUTF8(L"b2D4常州简易软件有限公司A1a3");
	std::string sCorrentLocalString = UtilString::convertWstringToLocalString(L"b2D4常州简易软件有限公司A1a3");// "b2D4常州简易软件有限公司A1a3";
	std::wstring wsCorrect = L"b2D4常州简易软件有限公司A1a3";


	std::string sLocalString = UtilString::convertUTF8ToLocalString(sCorrectUtf8);
	EXPECT_EQ(sLocalString, sCorrentLocalString);

	std::wstring ws = UtilString::convertUTF8ToWstring(sCorrectUtf8);
	EXPECT_EQ(ws, wsCorrect);

	std::string sUtf8_2 = UtilString::convertWstringToUTF8(wsCorrect);
	EXPECT_EQ(sUtf8_2, sCorrectUtf8);

	std::string sLocalString_2 = UtilString::convertWstringToLocalString(wsCorrect);
	EXPECT_EQ(sLocalString_2,sCorrentLocalString);

	std::wstring ws_2 = UtilString::convertLocalStringToWString(sCorrentLocalString);
	EXPECT_EQ(ws_2, wsCorrect);


	std::string sUtf8_3 = UtilString::convertLocalStringToUTF8(sCorrentLocalString);
	EXPECT_EQ(sUtf8_3, sCorrectUtf8);


}
