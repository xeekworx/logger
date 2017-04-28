// example.cpp : Defines the entry point for the console application.
//
#include <logger.h>
#include <chrono>
#include <thread>

void call_from_thread();

int main()
{
	// Use default log:
	xeekworx::log.enable();
	xeekworx::log << LOGSTAMP << xeekworx::logtype::INFO << L"Logger Version: " << xeekworx::logger::get_version() << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::NOTICE << L"Test notice logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG << L"Test debug logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG2 << L"Test debug2 logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG3 << L"Test debug3 logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::WARNING << L"Test warning logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::FATAL << L"Test fatal logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::ERR << L"Test error logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::EMPTY << L"Test empty logtype" << std::endl;

	// Log from main thread:
	xeekworx::log << LOGSTAMP << xeekworx::logtype::NOTICE << L"Called from main thread " << std::this_thread::get_id() << std::endl;

	// Let a second go by so the time stamp changes, just for testing:
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// Test the thread safety of the logger:
	std::thread t1(call_from_thread);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	std::thread t2(call_from_thread);
	std::this_thread::sleep_for(std::chrono::milliseconds(40));
	std::thread t3(call_from_thread);
	std::this_thread::sleep_for(std::chrono::milliseconds(60));
	std::thread t4(call_from_thread);

	t4.join();
	t3.join();
	t2.join();
	t1.join();

	// Use custom log:
	xeekworx::logger::config config = xeekworx::logger::default_config();
	config.enable = true;
	config.file = L"mylog.log";
	xeekworx::logger mylog(config);
	mylog << LOGSTAMP << xeekworx::logtype::INFO << L"Logger Version: " << xeekworx::logger::get_version() << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::NOTICE << L"Test notice logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::DEBUG << L"Test debug logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::DEBUG2 << L"Test debug2 logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::DEBUG3 << L"Test debug3 logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::WARNING << L"Test warning logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::FATAL << L"Test fatal logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::ERR << L"Test error logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::EMPTY << L"Test empty logtype" << std::endl;

	return 0;
}

void call_from_thread() {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	xeekworx::log << LOGSTAMP << xeekworx::logtype::NOTICE << L"Called from other thread " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG << L"Called from other thread " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG2 << L"Called from other thread " << std::this_thread::get_id() << std::endl;
}