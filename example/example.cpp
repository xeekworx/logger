// example.cpp : Defines the entry point for the console application.
//
#include <logger.h>

int main()
{
	// Use default log:
	xeekworx::log.enable();
	xeekworx::log << LOGSTAMP << xeekworx::logtype::INFO << L"Logger Version: " << xeekworx::logger::get_version() << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::NOTICE << L"Test 1234" << std::endl;

	// Use custom log:
	xeekworx::logger::config config = xeekworx::logger::default_config();
	config.enable = true;
	config.file = L"mylog.log";
	xeekworx::logger mylog(config);
	mylog << LOGSTAMP << xeekworx::logtype::INFO << L"Logger Version: " << xeekworx::logger::get_version() << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::NOTICE << L"Test 1234" << std::endl;

	return 0;
}

