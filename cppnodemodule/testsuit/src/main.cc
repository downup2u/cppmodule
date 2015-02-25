// TestSuitUseGTest.cpp : Defines the entry point for the console application.
//

#include <gtest/gtest.h>

#include <glog.h>
int main(int argc, char** argv) 
{
	testing::InitGoogleTest(&argc, argv);
	try
	{
		RUN_ALL_TESTS();
	}
	catch (std::exception &e)
	{
		std::string err = e.what();
	}
	return 0;
}

