#include "wssrvmain.h"

int main() {
	std::string dbconn = "couchbase://localhost/xkcrm";
	wssrvmain::get_mutable_instance().start(9002,dbconn,"123456");
	char ch = getchar();
	while (ch != 'q'){
		ch = getchar();
	}
	wssrvmain::get_mutable_instance().stop();
}
