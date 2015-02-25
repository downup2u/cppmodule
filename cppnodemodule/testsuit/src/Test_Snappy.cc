#include <gtest/gtest.h>   
#include <string>
#include <snappy.h>
#include <boost/algorithm/hex.hpp>
class Test_Snappy: public testing::Test
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

TEST_F(Test_Snappy, CodecString)
{
	std::string sCorrectUtf8 = "b2D4常州简易软件有限公司A1a3";
	sCorrectUtf8 = boost::algorithm::hex(sCorrectUtf8);
	std::string sOut;
	snappy::Compress(sCorrectUtf8.data(), sCorrectUtf8.size(), &sOut);

	std::string sIn;
	snappy::Uncompress(sOut.data(), sOut.size(), &sIn);

	EXPECT_EQ(sCorrectUtf8, sIn);
}
